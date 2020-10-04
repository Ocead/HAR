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

//region process_tab

process_tab::process_tab() = default;

void process_tab::wake(const cell_h & hnd, cell_base & clb) {
    _waking.try_emplace(hnd, clb);
}

void process_tab::tire(const cell_h & hnd, cell_base & clb) {
    _tiring.try_emplace(hnd, clb);
}

void process_tab::start(const cell_h & hnd, cell_base & clb) {
    _starting.try_emplace(hnd, clb);
}

void process_tab::halt(const cell_h & hnd, cell_base & clb) {
    _halting.try_emplace(hnd, clb);
}

void process_tab::apply() {
    /*Tire*/ {
        for (auto &[hnd, clb] : _tiring) {
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto prc = static_cast<process>(~process::CYCLE);
                {
                    auto &[cl, tab] = it->second;
                    prc = tab = static_cast<process>(tab & prc);
                }
                if (!prc) {
                    _active.erase(it);
                    _inactive.try_emplace(hnd, clb);
                }
            } else {
                _inactive.try_emplace(hnd, clb);
            }
        }
        _tiring.clear();
    }

    /*Halt*/ {
        for (auto &[hnd, clb] : _halting) {
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto prc = static_cast<process>(~process::MOVE);
                {
                    auto &[cl, tab] = it->second;
                    prc = tab = static_cast<process>(tab & prc);
                }
                if (!prc) {
                    _active.erase(it);
                    _inactive.try_emplace(hnd, clb);
                }
            } else {
                _inactive.try_emplace(hnd, clb);
            }
        }
        _halting.clear();
    }

    /*Wake*/ {
        for (auto &[hnd, clb] : _waking) {
            _inactive.erase(hnd);
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto &[cl, tab] = it->second;
                tab = static_cast<process>(tab | process::CYCLE);
            } else {
                _active.try_emplace(hnd, clb, process::CYCLE);
            }
        }
        _waking.clear();
    }

    /*Start*/ {
        for (auto &[hnd, clb] : _starting) {
            _inactive.erase(hnd);
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto &[cl, tab] = it->second;
                tab = static_cast<process>(tab | process::MOVE);
            } else {
                _active.try_emplace(hnd, clb, process::MOVE);
            }
        }
        _starting.clear();
    }
}

void process_tab::remove(const cell_h & hnd) {
    _active.erase(hnd);
    _waking.erase(hnd);
    _tiring.erase(hnd);
    _starting.erase(hnd);
    _halting.erase(hnd);
    _inactive.erase(hnd);
}

const decltype(process_tab::_active) & process_tab::get_active() const {
    return _active;
}

const decltype(process_tab::_waking) & process_tab::get_waking() const {
    return _waking;
}

const decltype(process_tab::_tiring) & process_tab::get_tiring() const {
    return _tiring;
}

const decltype(process_tab::_starting) & process_tab::get_starting() const {
    return _starting;
}

const decltype(process_tab::_halting) & process_tab::get_halting() const {
    return _halting;
}

const decltype(process_tab::_inactive) &  process_tab::get_inactive() const {
    return _inactive;
}

process_tab::~process_tab() = default;

//endregion

//region automaton

automaton::automaton(inner_simulation & sim) : automaton(sim, std::thread::hardware_concurrency() - 1) {

}

automaton::automaton(inner_simulation & sim, ushort_t workers) : _sim(sim),
                                                                 _state(state::INIT),
                                                                 _substep(substep::INIT),
                                                                 _threads(workers),
                                                                 _workers(),
                                                                 _waiting(),
                                                                 _cyclex(),
                                                                 _autoex(),
                                                                 _workdone(0),
                                                                 _first(false),
                                                                 _time_delta(16667us),
                                                                 _last_cycle_begin(clock::now()) {
    _cyclex.lock();
    _workers.reserve(_threads);
    for (auto i = 0u; i < _threads; ++i) {
        _workers.emplace_back(std::make_unique<worker>(*this, i));
    }
}

void automaton::i_am_done() {
    if (++_workdone % _threads == 0) {
        _autoex.unlock();
    }
}

void automaton::wait_for_all() {
    _autoex.lock();
}

void automaton::inner_exec(std::pair<participant_h, participant::callback_t> & pack) {
    auto & worker = *_workers[0];
    worker.process_exec(pack);
    worker.cycle_commit_and_draw();
    worker.clean();
}

enum automaton::state automaton::state() {
    return _state.load(std::memory_order_acquire);
}

enum automaton::substep automaton::substep() {
    return _substep.load(std::memory_order_acquire);
}

enum automaton::state automaton::set_state(enum state to) {
    auto old = _state.exchange(to, std::memory_order_acq_rel);
    if (old != to) {
        debug(switch (to) {
            case state::INIT:
                debug_log("to INIT");
                break;
            case state::RUN:
                debug_log("to RUN");
                break;
            case state::STEP:
                debug_log("to STEP");
                break;
            case state::STOP:
                debug_log("to STOP");
                break;
        });

        for (auto &[id, parti] : _sim.participants()) {
            switch (to) {
                case automaton::state::RUN: {
                    parti->on_run();
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

std::mutex & automaton::get_autoex() {
    return _autoex;
}

void automaton::request(participant_h id) {
    switch (_state) {
        case automaton::state::RUN: {
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
    _workers[0]->process_single_request(iparti);
}

bool_t automaton::begin(bool_t and_block) {
    uint_t c = _waiting++;
    debug_log("Lock automaton from " + std::to_string(c));
    if (c > 0 && and_block) {
        debug_log("Wait on mutex");
        _cyclex.lock();
        debug_log("Locked mutex");
    }
    return c == 0;
}

bool_t automaton::end(bool_t and_unblock) {
    uint_t c = --_waiting;
    debug_log("Unlock automaton to " + std::to_string(c));
    if (c > 0 && and_unblock) {
        _cyclex.unlock();
    }
    return c == 0;
}

void automaton::cycle() {
    debug_log("begin");
    begin(true);
    auto now = clock::now();
    auto wait_for = now - _last_cycle_begin;
    _last_cycle_begin = now;
    std::this_thread::sleep_for(_time_delta - wait_for);

    _substep = automaton::substep::PROCESS_REQUEST;
    auto & worker0 = *_workers[0];
    if (worker0.process_requests()) {
        worker0.cycle_commit_and_draw();
        worker0.clean();
    }

    _substep = automaton::substep::CYCLE_AND_MOVE;
    for (auto & w : _workers) {
        w->unblock();
    }
    _workers[0]->cycle_and_move();
    wait_for_all();

    _substep = automaton::substep::COMMIT_AND_DRAW;
    for (auto & w : _workers) {
        w->unblock();
    }
    _workers[0]->cycle_commit_and_draw();
    wait_for_all();

    _substep = automaton::substep::CLEAN;
    for (auto & w : _workers) {
        w->unblock();
    }
    _workers[0]->clean();
    wait_for_all();
    end(true);
    debug_log("end");
}

automaton::~automaton() = default;

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
                    cycle_and_move();
                    break;
                }
                case automaton::substep::COMMIT_AND_DRAW: {
                    cycle_commit_and_draw();
                    break;
                }
                case automaton::substep::CLEAN: {
                    clean();
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
    auto worker_num = _auto._workers.size();

    for (int_t it = offset; it < int_t(size); it += worker_num) {
        dcoords_t i{ it % dim.x, it / dim.x };
        grid_cell gcl{ _ctx, grid.at(i) };
        gcl.logic().cycle(gcl);
    }
}

void automaton::worker::process_cargo(world & world) {
    auto & cargos = world.cargo();
    auto size = cargos.size();
    auto worker_num = _auto._workers.size();
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

void automaton::worker::context_commit_and_draw(context & ctx) {
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
    if (_thread.get_id() != std::thread::id()) {
        _thread = std::thread(&automaton::worker::work, this);
    }
}

void automaton::worker::process_single_request(inner_participant & iparti) {
    auto & ctx = iparti.get_context();
    context_commit_and_draw(ctx);
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

void automaton::worker::cycle_and_move() {
    world & model = _auto._sim.get_model();
    process_grid(model.get_model());
    process_grid(model.get_bank());
}

void automaton::worker::cycle_commit_and_draw() {
    context_commit_and_draw(_ctx);
}

void automaton::worker::clean() {
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
