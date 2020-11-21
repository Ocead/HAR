//
// Created by Johannes on 17.06.2020.
//

#include <har/cargo_cell.hpp>
#include <har/grid_cell.hpp>

#include "logic/automaton.hpp"
#include "logic/inner_participant.hpp"
#include "logic/inner_simulation.hpp"

using namespace std::chrono_literals;

using namespace har;

//region automaton

automaton::automaton(inner_simulation & sim) : automaton(sim, std::thread::hardware_concurrency() - 1) {

}

automaton::automaton(inner_simulation & sim, ushort_t workers) : _sim(sim),
                                                                 _state(state::INIT),
                                                                 _substep(substep::INIT),
                                                                 _threads(workers),
                                                                 _self_worker(*this, 0u),
                                                                 _workers(),
                                                                 _waiting(),
                                                                 _cyclex(),
                                                                 _autoex(),
                                                                 _stepex(),
                                                                 _tab(),
                                                                 _workdone(0),
                                                                 _first(false) {
    _cyclex.lock();
    _workers.reset(static_cast<worker *>(::operator new(workers * sizeof(worker))));
    for (auto i = 0u; i < _threads; ++i) {
        new(&_workers[i]) worker(*this, i + 1);
    }
}

void automaton::do_step(enum automaton::substep step) {
    _stepex.lock();
    _substep.store(step, std::memory_order_release);
    std::for_each_n(_workers.get(), _threads, [](worker & w) {
        w.unblock();
    });
    switch (step) {
        case substep::CYCLE_AND_MOVE:
            _self_worker.cycle_and_move(worker::step_type::CYCLE);
            break;
        case substep::COMMIT_AND_DRAW:
            _self_worker.commit_and_draw(worker::step_type::CYCLE);
            break;
        case substep::CLEAN:
            _self_worker.clean(worker::step_type::CYCLE);
            break;
        default:
            break;
    }
    wait_for_all();
}

void automaton::i_am_done() {
    if (++_workdone % _threads == 0) {
        _stepex.unlock();
    }
}

void automaton::wait_for_all() {
    std::scoped_lock{ _stepex };
}

void automaton::inner_exec(std::pair<participant_h, participant::callback_t> & pack) {
    _self_worker.process_exec(pack);
    _self_worker.commit_and_draw(worker::step_type::REQUEST);
    _self_worker.clean(worker::step_type::REQUEST);
}

void automaton::commence() {
    std::for_each_n(_workers.get(), _threads, [](worker & w) {
        w.start();
    });
}

enum automaton::state automaton::state() {
    return _state.load(std::memory_order_acquire);
}

enum automaton::substep automaton::substep() {
    return _substep.load(std::memory_order_acquire);
}

enum automaton::state automaton::set_state(participant_h id, enum state to) {
    auto old = _state.exchange(to, std::memory_order_acq_rel);
    if (old != to) {
        DEBUG {
            switch (to) {
                case state::INIT:
                    DEBUG_LOG("to INIT");
                    break;
                case state::RUN:
                    DEBUG_LOG("to RUN");
                    break;
                case state::STEP:
                    DEBUG_LOG("to STEP");
                    break;
                case state::STOP:
                    DEBUG_LOG("to STOP");
                    break;
            }
        };

        for (auto &[pid, parti] : _sim.participants()) {
            switch (to) {
                case automaton::state::RUN: {
                    parti->on_run(id == pid);
                    break;
                }
                case automaton::state::STEP: {
                    parti->on_step();
                    break;
                }
                case automaton::state::STOP: {
                    parti->on_stop();
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    return old;
}

process_tab & automaton::get_tab() {
    return _tab;
}

std::mutex & automaton::get_autoex() {
    return _autoex;
}

void automaton::resize_tab(const gcoords_t & from, const gcoords_t & to) {
    auto & model = _sim.get_model();

    for (auto x = to.pos.x; x < from.pos.x; ++x) {
        for (dcoord_t y = 0; y < from.pos.y; ++y) {
            _tab.remove(gcoords_t{ from.cat, x, y });
        }
    }

    for (auto y = to.pos.y; y < from.pos.y; ++y) {
        for (dcoord_t x = 0; x < to.pos.x; ++x) {
            _tab.remove(gcoords_t{ from.cat, x, y });
        }
    }

    auto ifrom = from.pos;
    auto ito = to.pos;

    if (to < from) {
        ifrom = ito - 1;
    }

    for (auto x = ifrom.x; x < ito.x; ++x) {
        for (dcoord_t y = 0; y < ito.y; ++y) {
            gcoords_t pos{ from.cat, x, y };
            _tab.wake(pos, model.at(pos));
        }
    }

    for (auto y = ifrom.y; y < ito.y; ++y) {
        for (dcoord_t x = 0; x < ifrom.x; ++x) {
            gcoords_t pos{ from.cat, x, y };
            _tab.wake(pos, model.at(pos));
        }
    }
}

void automaton::request(participant_h id) {
    switch (_state) {
        case automaton::state::RUN: {
            begin(true);
            _sim.inner_participants().at(id)->unlock_and_wait_for_request();
            break;
        }
        case automaton::state::STEP: {
            begin(true);
            _sim.inner_participants().at(id)->unlock_and_wait_for_request();
            break;
        }
        case automaton::state::STOP: {
            begin(true);
            _sim.inner_participants().at(id)->unlock_and_wait_for_request();
            break;
        }
        case automaton::state::INIT: {
            begin(true);
            _sim.inner_participants().at(id)->unlock_and_wait_for_request();
            break;
        }
    }
}

void automaton::exec(participant_h id, participant::callback_t && fun) {
    _queue.push({ id, fun });
    if (begin(false)) {
        _queue.process_one([&](auto & pack) { inner_exec(pack); });
    }
    end(false);
}

void automaton::process(inner_participant & iparti) {
    _self_worker.process_single_request(iparti);
    _tab.apply();
}

bool_t automaton::begin(bool_t and_block) {
    uint_t c = _waiting++;
    DEBUG_LOG("Lock automaton from " + std::to_string(c));
    if (c > 0 && and_block) {
        DEBUG_LOG("Wait on mutex");
        _cyclex.lock();
        DEBUG_LOG("Locked mutex");
    }
    return c == 0;
}

bool_t automaton::end(bool_t and_unblock) {
    uint_t c = --_waiting;
    DEBUG_LOG("Unlock automaton to " + std::to_string(c));
    if (c > 0 && and_unblock) {
        _cyclex.unlock();
    }
    return c == 0;
}

void automaton::cycle() {
    switch (_state.load(std::memory_order_acquire)) {
        case state::RUN: {
            break;
        }
        case state::STEP: {
            set_state(PARTICIPANT.no_one(), state::STOP);
            break;
        }
        default: {
            return;
        }
    }
    DEBUG_LOG("begin");
    //begin(true);

    _substep.store(automaton::substep::PROCESS_REQUEST, std::memory_order_release);
    if (_self_worker.process_requests()) {
        _self_worker.commit_and_draw(worker::step_type::REQUEST);
        _self_worker.clean(worker::step_type::REQUEST);
    }

    do_step(substep::CYCLE_AND_MOVE);
    do_step(substep::COMMIT_AND_DRAW);
    do_step(substep::CLEAN);

    //end(true);
    DEBUG_LOG("end");
}

automaton::~automaton() {
    std::for_each_n(_workers.get(), _threads, [](worker & w) {
        w.~worker();
    });
    std::allocator<worker>().deallocate(_workers.release(), _threads);
}

//endregion

//region dispatcher

automaton::dispatcher::dispatcher(automaton & automaton, uint_t interval) : _auto(automaton),
                                                                            _model(automaton._sim.get_model().get_model()),
                                                                            _bank(automaton._sim.get_model().get_bank()),
                                                                            _interval(interval) {

}

std::tuple<cell_h, std::reference_wrapper<cell_base>> automaton::dispatcher::get_my_nth(uint_t num, uint_t offset) {
    uint_t nth = num * _interval + offset;

    if (auto mdim = _model.dim(); mdim.has_nth(nth)) {
        gcoords_t pos{ grid_t::MODEL_GRID, mdim.nth_of(nth) };
        return std::make_tuple(pos, std::ref(_model.at(pos.pos)));
    } else if (auto bdim = _bank.dim(); bdim.has_nth(nth)) {
        nth -= mdim.x * mdim.y;
        gcoords_t pos{ grid_t::BANK_GRID, bdim.nth_of(nth) };
        return std::make_tuple(pos, std::ref(_bank.at(pos.pos)));
    } else {
        gcoords_t pos{ grid_t::INVALID_GRID, -1, -1 };
        return std::make_tuple(pos, std::ref(cell_base::invalid()));
    }
}

automaton::dispatcher::~dispatcher() = default;

//endregion

//region worker

automaton::worker::worker(automaton & automaton, ushort_t id) : _auto(automaton),
                                                                offset(id),
                                                                _valid(false) {
    _workex.lock();
}

void automaton::worker::work() {
    while (true) {
        begin();
        if (_valid.test_and_set()) {
            switch (_auto.substep()) {
                case automaton::substep::CYCLE_AND_MOVE: {
                    cycle_and_move(worker::step_type::CYCLE);
                    break;
                }
                case automaton::substep::COMMIT_AND_DRAW: {
                    commit_and_draw(worker::step_type::CYCLE);
                    break;
                }
                case automaton::substep::CLEAN: {
                    clean(worker::step_type::CYCLE);
                    break;
                }
                default: {
                    break;
                }
            }
            done();
        } else {
            done();
            return;
        }
    }
}

void automaton::worker::begin() {
    _workex.lock();
}

void automaton::worker::done() {
    _auto.i_am_done();
}

void automaton::worker::process_grid(grid & grid) {
    auto dim = grid.dim();
    auto size = grid.dim().size();
    auto worker_num = _auto._threads + 1;

    for (int_t it = offset; it < int_t(size); it += worker_num) {
        dcoords_t i{ it % dim.x, it / dim.x };
        grid_cell gcl{ _ctx, grid.at(i) };
        gcl.logic().cycle(gcl);
    }
}

void automaton::worker::process_cargo(world & world) {
    auto & cargos = world.cargo();
    auto size = cargos.size();
    auto worker_num = _auto._threads;
    uint_t i = 0;
    auto it = cargos.begin();

    //Moves index and iterator to the first position to process
    for (; i < offset; ++i) {
        if (++it == cargos.end()) {
            return;
        }
    }

    do {
        //TODO: Process cargo here
        for (uint_t j = 0; j < worker_num; ++j) {
            ++i;
            if (++it == cargos.end()) {
                return;
            }
        }
    } while (i < size);
}

void automaton::worker::request_commit_and_draw(context & ctx) {
    auto & model = _auto._sim.get_model();
    for (auto & conn : ctx.connected()) {
        auto & from = conn.base.get();
        from.add_connection(conn.use, model.at(conn.pos));
        ctx.change(conn.base.get().position());
        for (auto &[id, parti] : _auto._sim.participants()) {
            parti->on_connection_added(conn.base.get().position(), conn.pos, conn.use);
        }
    }
    for (auto & conn : ctx.disconnected()) {
        auto & from = conn.base.get();
        from.remove_connection(conn.use);
        ctx.change(conn.base.get().position());
        for (auto &[id, parti] : _auto._sim.participants()) {
            parti->on_connection_removed(from.position(), conn.use);
        }
    }
    /*if (!ctx.changed().empty()) {
        context temp_ctx;
        for (auto & hnd : ctx.changed()) {
            cell_base & clb = model.at(hnd);
            clb.transit();
            if (_auto.state() == automaton::state::STOP) {
                auto & pt = clb.logic();
                switch (cell_cat(hnd.index())) {
                    case cell_cat::INVALID_CELL: {
                        break;
                    }
                    case cell_cat::GRID_CELL: {
                        auto & gclb = static_cast<grid_cell_base &>(clb);
                        grid_cell gcl{ temp_ctx, gclb };
                        pt.cycle(gcl);
                        break;
                    }
                    case cell_cat::CARGO_CELL: {
                        auto & cclb = static_cast<cargo_cell_base &>(clb);
                        grid_cell_base & gclb = model.at({ grid_t::MODEL_GRID, dcoords_t(cclb.position()) });
                        cargo_cell ccl{ temp_ctx, cclb, gclb };
                        pt.cycle(ccl);
                        break;
                    }
                }
            }
        }
        ctx.changed().merge(temp_ctx.changed());
        ctx.redraw().merge(temp_ctx.redraw());
    }*/
    cycle_commit_and_draw(ctx);
}

void automaton::worker::cycle_commit_and_draw(context & ctx) {
    auto & model = _auto._sim.get_model();
    for (auto & hnd : ctx.changed()) {
        cell_base & clb = model.at(hnd);
        for (auto & iparti : _auto._sim.inner_participants()) {
            if (iparti.second->get_selected() == hnd) {
                auto parti = _auto._sim.participants().at(iparti.first);
                for (auto &[id, val] : clb.intermediate()) {
                    parti->on_selection_update(hnd, id, val, false);
                }
                if (!clb.intermediate().empty()) {
                    parti->on_commit();
                }
            }
        }
        clb.transit();
    }
    for (auto & hnd : ctx.redraw()) {
        for (auto &[num, parti] : _auto._sim.participants()) {
            auto & clb = model.at(hnd);
            auto & pt = clb.logic();
            auto img = parti->get_image_base(hnd);

            switch (cell_cat(hnd.index())) {
                case cell_cat::INVALID_CELL: {
                    break;
                }
                case cell_cat::GRID_CELL: {
                    auto & gclb = static_cast<grid_cell_base &>(clb);
                    grid_cell gcl{ ctx, gclb };
                    pt.draw(gcl, img);
                    img = parti->process_image(gclb.position(), img);
                    parti->on_redraw(hnd, std::forward<image_t>(img), false);
                    for (auto dir : direction::cardinal) {
                        gcoords_t npos{ gclb.position() };
                        npos.pos += dir;
                    }
                    break;
                }
                case cell_cat::CARGO_CELL: {
                    auto & cclb = static_cast<cargo_cell_base &>(clb);
                    grid_cell_base & gclb = model.at({ grid_t::MODEL_GRID, dcoords_t(cclb.position()) });
                    cargo_cell ccl{ ctx, cclb, gclb };
                    pt.draw(ccl, img);
                    img = parti->process_image(cclb.id(), img);
                    parti->on_redraw(hnd, std::forward<image_t>(img), false);
                    break;
                }
            }
            parti->on_commit();
        }
    }
    for (auto & msg : ctx.messages()) {
        for (auto &[id, parti] : _auto._sim.participants()) {
            parti->on_message(std::get<0>(msg), std::get<1>(msg));
        }
    }
}

void automaton::worker::wait_for_next_step() {
    //TODO: Implement
}

void automaton::worker::start() {
    if (_thread.get_id() == std::thread::id()) {
        _valid.test_and_set(std::memory_order_release);
        _thread = std::thread(&automaton::worker::work, this);
    }
}

void automaton::worker::process_single_request(inner_participant & iparti) {
    auto & ctx = iparti.get_context();
    request_commit_and_draw(ctx);
    ctx.reset();
}

void automaton::worker::process_exec(std::pair<participant_h, participant::callback_t> & pack) {
    auto &[id, fun] = pack;
    participant::context ctx{ *_auto._sim.inner_participants().at(id), false };
    fun(ctx);
}

bool_t automaton::worker::process_requests() {
    bool_t processed = false;
    if (_auto._waiting.load(std::memory_order_acquire) > 1) {
        for (auto &[id, iparti_rw] : _auto._sim.inner_participants()) {
            auto & iparti = *iparti_rw;
            if (iparti.do_cycle()) {
                participant::context ctx{ iparti, false };
                _auto._sim.participants().at(id)->on_cycle(ctx);
                processed = true;
            }
            if (iparti.has_request()) {
                iparti.unlock_and_wait_for_request();
                processed = true;
            }
        }
    }
    return processed;
}

void automaton::worker::cycle_and_move(step_type type) {
    //DEBUG_LOG("WORKER[" << offset << "] does CYCLE_AND_MOVE");
    world & model = _auto._sim.get_model();
    process_grid(model.get_model());
    process_grid(model.get_bank());
}

void automaton::worker::commit_and_draw(step_type type) {
    //DEBUG_LOG("WORKER[" << offset << "] does COMMIT_AND_DRAW");
    if (type == step_type::REQUEST) {
        request_commit_and_draw(_ctx);
    } else {
        cycle_commit_and_draw(_ctx);
    }
}

void automaton::worker::clean(step_type type) {
    //DEBUG_LOG("WORKER[" << offset << "] does CLEAN");
    _ctx.reset();
}

void automaton::worker::unblock() {
    _workex.unlock();
}

automaton::worker::~worker() {
    if (_thread.joinable()) {
        _valid.clear();
        _workex.unlock();
        _thread.join();
    }
}

//endregion
