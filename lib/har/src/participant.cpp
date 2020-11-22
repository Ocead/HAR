//
// Created by Johannes on 29.06.2020.
//

#include <har/participant.hpp>

#include "logic/inner_participant.hpp"

using namespace har;

//region participant

participant::participant() : _iparti(nullptr), _destroying(false) {

}

participant::context participant::request(request_type req_type) {
    return _iparti->request(req_type);
}

void participant::exec(participant::callback_t && fun) {
    _iparti->exec(std::forward<participant::callback_t>(fun));
}

void participant::start() {
    _iparti->start();
}

void participant::step() {
    _iparti->step();
}

void participant::stop() {
    _iparti->stop();
}

void participant::select(const cell_h & hnd) {
    _iparti->select(hnd);
}

void participant::include_part(const part & pt) {
    _iparti->include_part(pt);
}

void participant::load_model(string_t & ser) {
    _iparti->load_model(ser);
}

void participant::load_model(string_t && ser) {
    _iparti->load_model(std::forward<string_t>(ser));
}

void participant::load_model(istream & is) {
    _iparti->load_model(is);
}

void participant::store_model(string_t & ser) {
    _iparti->store_model(ser);
}

void participant::store_model(ostream & os) {
    _iparti->store_model(os);
}

void participant::redraw_all() {
    _iparti->redraw_all();
}

bool_t participant::attached() const {
    return _iparti;
}

void participant::detach() {
    _iparti->detach();
}

void participant::exit() {
    _iparti->exit();
}

void participant::attach(inner_participant & iparti, int argc, char * argv[], char * envp[]) {
    _iparti = &iparti;
    on_attach(argc, argv, envp);
}

void participant::detach(inner_participant & iparti) {
    if (!_destroying) {
        on_detach();
    }
    _iparti = nullptr;
}

participant::~participant() {
    _destroying = true;
    if (attached()) {
        detach();
    }
}

//endregion

//region participant::context

participant::context::context(inner_participant & parti, request_type type, bool_t blocking) : _parti(parti),
                                                                                               _type(type),
                                                                                               _blocking(blocking) {
    if (blocking) {
        _parti.get().wait_for_automaton(_type);
    }
}

participant::context::context(context && fref) noexcept: _parti(fref._parti),
                                                         _type(fref._type),
                                                         _blocking(fref._blocking) {
    fref._blocking = false;
}

full_grid_cell participant::context::at(const gcoords_t & pos) {
    return _parti.get().at(pos);
}

full_cargo_cell participant::context::at(cargo_h num) {
    return _parti.get().at(num);
}

void participant::context::resize_grid(const gcoords_t & to) {
    return _parti.get().resize_grid(to);
}

void participant::context::cycle() {
    _parti.get().cycle();
}

void participant::context::commit() {
    _parti.get().commit();
}

participant::context & participant::context::operator=(participant::context && fref) noexcept {
    if (this != &fref) {
        this->~context();
        new(this) context(std::forward<context>(fref));
    }
    return *this;
}

participant::context::~context() {
    if (_blocking) {
        _parti.get().unlock_automaton_and_commit(_type);
    }
}

//endregion
