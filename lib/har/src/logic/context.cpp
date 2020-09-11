//
// Created by Johannes on 10.06.2020.
//

#include "logic/context.hpp"

using namespace har;

context & context::invalid() {
    static context ctx{ };
    return ctx;
}

context::context() : _model(nullptr),
                     _changed(),
                     _redraw(),
                     _connected(),
                     _disconnected(),
                     _spawned(),
                     _moved(),
                     _destroyed(),
                     _messages() {

}

context::context(model & model) : _model(&model),
                                  _changed(),
                                  _redraw(),
                                  _connected(),
                                  _disconnected(),
                                  _spawned(),
                                  _moved(),
                                  _destroyed(),
                                  _messages() {

}

void context::message(const string_t & header, const string_t & content) {
    _messages.emplace_back(std::array{ header, content });
}

grid_cell_base & context::at(const gcoords_t & pos) {
    return _model->at(pos);
}

cargo_cell_base & context::at(cargo_h num) {
    return _model->at(num);
}

decltype(context::_changed) & context::changed() {
    return _changed;
}

const decltype(context::_changed) & context::changed() const {
    return _changed;
}

decltype(context::_redraw) & context::redraw() {
    return _redraw;
}

const decltype(context::_redraw) & context::redraw() const {
    return _redraw;
}

decltype(context::_connected) & context::connected() {
    return _connected;
}

const decltype(context::_connected) & context::connected() const {
    return _connected;
}

decltype(context::_disconnected) & context::disconnected() {
    return _disconnected;
}

const decltype(context::_disconnected) & context::disconnected() const {
    return _disconnected;
}

decltype(context::_spawned) & context::spawned() {
    return _spawned;
}

const decltype(context::_spawned) & context::spawned() const {
    return _spawned;
}

decltype(context::_moved) & context::moved() {
    return _moved;
}

const decltype(context::_moved) & context::moved() const {
    return _moved;
}

decltype(context::_destroyed) & context::destroyed() {
    return _destroyed;
}

const decltype(context::_destroyed) & context::destroyed() const {
    return _destroyed;
}

decltype(context::_messages) & context::messages() {
    return _messages;
}

const decltype(context::_messages) & context::messages() const {
    return _messages;
}

void context::change(const cell_h & hnd) {
    _changed.emplace(hnd);
}

void context::draw(const cell_h & hnd) {
    _redraw.emplace(hnd);
}

void context::connect(unresolved_connection && conn) {
    _connected.emplace_back(conn);
}

void context::disconnect(unresolved_connection && conn) {
    _disconnected.emplace_back(conn);
}

cargo_cell_base & context::spawn(cargo_cell_base && cell) {
    return *_spawned.emplace_back(new cargo_cell_base(std::forward<cargo_cell_base>(cell)));
}

void context::move(cargo_h num) {
    _moved.emplace(num);
}

void context::destroy(cargo_h num) {
    _destroyed.emplace(num);
}

void context::reset() {
    _changed.clear();
    _redraw.clear();
    _connected.clear();
    _disconnected.clear();
    for (auto & ptr : _spawned) {
        delete ptr;
    }
    _spawned.clear();
    _moved.clear();
    _destroyed.clear();
    _messages.clear();
}

context::~context() = default;
