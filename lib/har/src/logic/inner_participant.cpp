//
// Created by Johannes on 25.06.2020.
//

#include "logic/inner_participant.hpp"
#include "logic/inner_simulation.hpp"

using namespace har;

//region asymmetric_lock

asymmetric_lock::asymmetric_lock(std::mutex & inverse) : _mutex(),
                                                         _waiting(false),
                                                         _inverse(inverse) {
    _mutex.lock();
}

void asymmetric_lock::request() {
    _waiting.exchange(true, std::memory_order_release);
    _mutex.lock();
    _waiting.exchange(false, std::memory_order_release);
}

void asymmetric_lock::grant() {
    _mutex.unlock();
    _inverse.get().lock();
}

void asymmetric_lock::hand_back() {
    _inverse.get().unlock();
}

bool_t asymmetric_lock::waiting() const {
    return _waiting.load(std::memory_order_acquire);
}

asymmetric_lock::~asymmetric_lock() = default;

//endregion

//region inner_participant

inner_participant::inner_participant(participant_h id, inner_simulation & sim) : _id(id),
                                                                                 _ctx(),
                                                                                 _simulation(sim),
                                                                                 _automaton(sim.get_automaton()),
                                                                                 _model(sim.get_model()),
                                                                                 _do_cycle(),
                                                                                 _do_draw(),
                                                                                 _selected(std::monostate()),
                                                                                 _alock(sim.get_automaton().get_autoex()) {

}

participant_h inner_participant::id() const {
    return _id;
}

context & inner_participant::get_context() {
    return _ctx;
}

const context & inner_participant::get_context() const {
    return _ctx;
}


const cell_h & inner_participant::get_selected() const {
    return _selected;
}

bool_t inner_participant::do_cycle() const {
    return _do_cycle.load(std::memory_order_acquire);
}

bool_t inner_participant::do_cycle(bool_t cycle) {
    return _do_cycle.exchange(cycle, std::memory_order_acq_rel);
}

bool_t inner_participant::do_draw() const {
    return _do_draw.load(std::memory_order_acquire);
}

bool_t inner_participant::do_draw(bool_t draw) {
    return _do_draw.exchange(draw, std::memory_order_acq_rel);
}

bool_t inner_participant::has_request() const {
    return _alock.waiting();
}

void inner_participant::commit() {
    auto & automaton = _automaton.get();
    automaton.process(*this);
}

void inner_participant::unlock_automaton_and_commit() {
    debug_log("PARTICIPANT[" + std::to_string(_id) + "] ends request");
    auto & automaton = _automaton.get();
    automaton.process(*this);
    _alock.hand_back();
    automaton.end(true);
}

void inner_participant::wait_for_automaton() {
    debug_log("PARTICIPANT[" + std::to_string(_id) + "] begins request");
    _automaton.get().request(_id);
    _alock.request();
}

void inner_participant::unlock_and_wait_for_request() {
    _alock.grant();
}

participant::context inner_participant::request(participant::request_type req_type) {
    return participant::context(*this, true);
}

void inner_participant::exec(std::function<void(participant::context &)> && fun) {
    //TODO: Implement
}

full_grid_cell inner_participant::at(const gcoords_t & pos) {
    return full_grid_cell(_ctx, _model.get().at(pos));
}

full_cargo_cell inner_participant::at(cargo_h num) {
    return full_cargo_cell(_ctx, _model.get().at(num));
}

void inner_participant::select(const cell_h & hnd) {
    _selected = hnd;
}

void inner_participant::include_part(const part & pt) {
    _simulation.get().include_part(pt);
}

void inner_participant::load_model(string_t & ser) {
    stringstream ss{ ser };
    auto & sim = _simulation.get();
    sim.load_model(_ctx, ss);
}

void inner_participant::load_model(string_t && ser) {
    stringstream ss{ std::forward<string_t>(ser) };
    auto & sim = _simulation.get();
    sim.load_model(_ctx, ss);
}

void inner_participant::load_model(istream & is) {
    auto ctx = request();
    auto & sim = _simulation.get();
    sim.load_model(_ctx, is);
}

void inner_participant::store_model(string_t & ser) {
    stringstream ss{ };
    _simulation.get().store_model(ss);
    ser = ss.str();
}

void inner_participant::store_model(ostream & os) {
    auto ctx = request();
    _simulation.get().store_model(os);
}

void inner_participant::resize_grid(const gcoords_t & to) {
    if (to.cat != grid_t::INVALID_GRID) {
        auto & sim = _simulation.get();
        auto & ept = sim.inventory().at(PART[0]);
        auto & model = _model.get();
        auto & grid = to.cat == grid_t::MODEL_GRID ? model.get_model() : model.get_bank();
        if (grid.dim() != to.pos) {
            auto from = grid.dim();
            auto old = dcoords_t::clamp(grid.dim(), dcoords_t(1, 1), to.pos);
            auto max = dcoords_t{ std::numeric_limits<int_t>::max(), std::numeric_limits<int_t>::max() };
            old = dcoords_t::clamp(old, dcoords_t(1, 1), max);
            model.resize(to.cat, ept, to.pos);
            for (auto &[id, parti] : sim.participants()) {
                parti->on_resize_grid(to);
                if (from.x != to.pos.x) {
                    for (dcoord_t x = old.x - 1; x < to.pos.x; ++x) {
                        for (dcoord_t y = 0; y < to.pos.y; ++y) {
                            _ctx.redraw().emplace(gcoords_t(to.cat, (x, y)));
                        }
                    }
                }
                if (from.y != to.pos.y) {
                    for (dcoord_t y = old.y - 1; y < to.pos.y; ++y) {
                        for (dcoord_t x = 0; x < to.pos.x; ++x) {
                            _ctx.redraw().emplace(gcoords_t(to.cat, (x, y)));
                        }
                    }
                }
            }
        }
    }
}

void inner_participant::redraw_all() {
    auto parti = _simulation.get().participants().at(_id);
    for (auto & g : { std::ref(_model.get().get_model()), std::ref(_model.get().get_bank())}) {
        for (auto & c : g.get()) {
            cell_h hnd{ gcoords_t{ g.get().cat(), c.first }};
            auto img = parti->get_image_base(hnd);
            auto & gclb = c.second;
            grid_cell gcl{ _ctx, gclb };
            gclb.logic().draw(gcl, img);
            img = parti->process_image(gclb.position(), img);
            parti->on_redraw(hnd, std::move(img), true);
        }
    }
}

void inner_participant::start() {
    _automaton.get().set_state(automaton::state::RUN);
}

void inner_participant::step() {
    _automaton.get().set_state(automaton::state::STEP);
}

void inner_participant::stop() {
    _automaton.get().set_state(automaton::state::STOP);
}

void inner_participant::detach() {
    _simulation.get().detach(_id);
    delete this;
    //TODO: Don't know if this works with optimizations
}

void inner_participant::exit() {
    _simulation.get().exit();
}

inner_participant::~inner_participant() = default;

//endregion inner_participant
