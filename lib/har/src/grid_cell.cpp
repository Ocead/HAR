//
// Created by Johannes on 10.06.2020.
//

#include <har/grid_cell.hpp>
#include <har/part.hpp>

#include "logic/context.hpp"

using namespace har;

//region grid_cell

grid_cell::grid_cell(context & ctx,
                     grid_cell_base & cell_base,
                     direction_t dir) : cell(ctx, *cell_base.get_cell(dir)),
                                        _dir(dir) {

}

connection_iterable grid_cell::connected() {
    return connection_iterable(_ctx, as_grid_cell_base());
}

grid_cell_base & grid_cell::as_grid_cell_base() noexcept {
    return static_cast<grid_cell_base &>(_cell);
}

const grid_cell_base & grid_cell::as_grid_cell_base() const noexcept {
    return static_cast<grid_cell_base &>(_cell);
}

cargo_cell grid_cell::spawn(const part & pt, ccoords_t pos) {
    auto & ncell = _ctx.spawn(cargo_cell_base(CARGO[-1], pt, std::clamp(pos,
                                                                        ccoords_t(0, 0),
                                                                        ccoords_t(.999, .999))));
    return cargo_cell(_ctx, ncell, as_grid_cell_base());
}

cargo_iterable grid_cell::cargo() {
    return cargo_iterable(_ctx, as_grid_cell_base(), cargo_cat::CARGO);
}


cargo_cell grid_cell::cargo(cargo_h num) {
    return cargo_cell(_ctx, as_grid_cell_base().cargo().at(num).base(), as_grid_cell_base());
}

const cargo_iterable grid_cell::artifacts() { //NOLINT
    return cargo_iterable(_ctx, as_grid_cell_base(), cargo_cat::ARTIFACT);
}

const cargo_cell grid_cell::artifacts(artifact_h num) { //NOLINT
    return cargo_cell(_ctx, as_grid_cell_base().artifacts().at(num).base(), as_grid_cell_base());
}

const cell grid_cell::operator[](direction_t dir) { //NOLINT
    //TODO: Probably fix for access from participant contexts
    auto * ptr = as_grid_cell_base().get_cell(dir);
    if (ptr) {
        return cell(_ctx, *ptr);
    } else {
        return cell(_ctx, grid_cell_base::invalid());
    }
}

grid_cell::~grid_cell() = default;

//endregion

//region connection

connection::connection(direction_t use, context & ctx,
                       grid_cell_base & base, direction_t cell_dir) : use(use),
                                                                      cell(ctx, base, cell_dir) {

}

connection::~connection() noexcept = default;

//endregion

//region connection_iterator

connection_iterator::connection_iterator(context & ctx, grid_cell_base & cell, direction_t dir) : _ctx(ctx),
                                                                                                  _gclb(cell),
                                                                                                  _dir(dir) {
    auto it = _gclb.connected().find(_dir);
    if (it != _gclb.connected().end()) {
        build_cell(it->second);
    }
}

connection & connection_iterator::build_cell(grid_cell_base & cell) {
    if (_dir != direction_t(-1)) {
        _conn.emplace(_dir, _ctx, cell, direction::NONE);
    } else {
        _conn.emplace(_dir, _ctx, grid_cell_base::invalid(), direction::NONE);
    }
    return _conn.value();
}

connection_iterator & connection_iterator::operator++() {
    auto it = _gclb.connected().find(_dir);
    if (++it != _gclb.connected().end()) {
        if (_dir != it->first) {
            _dir = it->first;
            auto & cell = it->second;
            build_cell(cell);
        } else {
            _dir = direction_t(-1);
        }
    } else {
        _dir = direction_t(-1);
    }
    return *this;
}

const connection & connection_iterator::operator*() {
    return _conn.value();
}

const connection * connection_iterator::operator->() {
    return &_conn.value();
}

bool_t connection_iterator::operator==(const connection_iterator & rhs) const {
    return &_ctx == &rhs._ctx && &_gclb == &rhs._gclb && _dir == rhs._dir;
}

bool_t connection_iterator::operator!=(const connection_iterator & rhs) const {
    return !(&_ctx == &rhs._ctx && &_gclb == &rhs._gclb && _dir == rhs._dir);
}

connection_iterator::~connection_iterator() = default;

//endregion

//region connection_iterable

connection_iterable::connection_iterable(context & ctx, grid_cell_base & cell) : _ctx(ctx), _cell(cell) {

}

bool_t connection_iterable::empty() const {
    return _cell.connected().empty();
}

size_t connection_iterable::size() const {
    return _cell.connected().size();
}

connection_iterator connection_iterable::begin() {
    if (!empty()) {
        return connection_iterator(_ctx, _cell, _cell.connected().begin()->first);
    } else {
        return connection_iterator(_ctx, _cell, direction_t(-1));
    }
}

connection_iterator connection_iterable::begin() const {
    if (!empty()) {
        return connection_iterator(_ctx, _cell, _cell.connected().begin()->first);
    } else {
        return connection_iterator(_ctx, _cell, direction_t(-1));
    }
}

connection_iterator connection_iterable::end() {
    return connection_iterator(_ctx, _cell, direction_t(-1));
}

connection_iterator connection_iterable::end() const {
    return connection_iterator(_ctx, _cell, direction_t(-1));
}

connection_iterable::~connection_iterable() = default;

//endregion connection_iterable

//region overlay

overlay::overlay(context & ctx, cargo_cell_base & cargo_base, grid_cell_base & cell_base,
                 cargo_h num) : num(num),
                                cell(ctx, cargo_base, cell_base) {

}

overlay::~overlay() noexcept = default;

//endregion

//region cargo_iterator

cargo_base_iterator::cargo_base_iterator(context & ctx, grid_cell_base & cell,
                                         cargo_cat cat, size_t pos) : _ctx(ctx),
                                                                      _gclb(cell),
                                                                      _cat(cat),
                                                                      _pos(pos),
                                                                      _cell() {

}

overlay & cargo_base_iterator::build_cell() {
    switch (_cat) {
        case cargo_cat::CARGO: {
            auto & cell = _gclb.cargo().at(CARGO[_pos]).base();
            _cell.emplace(_ctx, cell, _gclb, CARGO[_pos]);
            break;
        }
        case cargo_cat::ARTIFACT: {
            auto & cell = _gclb.artifacts().at(ARTIFACT[_pos]).base();
            _cell.emplace(_ctx, cell, _gclb, ARTIFACT[_pos]);
            break;
        }
        case cargo_cat::NO_ARTIFACT: {
            _cell.emplace(_ctx, cargo_cell_base::invalid(), _gclb, ARTIFACT[_pos]);
            break;
        }
    }

    return _cell.value();
}

cargo_base_iterator & cargo_base_iterator::operator++() {
    ++_pos;
    return *this;
}

bool_t cargo_base_iterator::operator==(const cargo_base_iterator & rhs) const {
    return _cat == rhs._cat && _pos == rhs._pos;
}

bool_t cargo_base_iterator::operator!=(const cargo_base_iterator & rhs) const {
    return _cat != rhs._cat || _pos != rhs._pos;
}

cargo_base_iterator::~cargo_base_iterator() = default;

overlay & cargo_iterator::operator*() {
    return build_cell();
}

overlay * cargo_iterator::operator->() {
    return &build_cell();
}

cargo_iterator::~cargo_iterator() = default;

const overlay & const_cargo_iterator::operator*() {
    return build_cell();
}

const overlay * const_cargo_iterator::operator->() {
    return &build_cell();
}

const_cargo_iterator::~const_cargo_iterator() = default;

//endregion

//region cargo_iterable

cargo_iterable::cargo_iterable(context & ctx, grid_cell_base & cell, cargo_cat cat) : _ctx(ctx),
                                                                                      _cell(cell),
                                                                                      _cat(cat) {

}

bool_t cargo_iterable::empty() const {
    switch (_cat) {
        case cargo_cat::CARGO: {
            return _cell.cargo().empty();
        }
        case cargo_cat::ARTIFACT: {
            return _cell.artifacts().empty();
        }
        case cargo_cat::NO_ARTIFACT: {
            return _cell.no_artifacts().empty();
        }
        default: {
            return true;
        }
    }
}

size_t cargo_iterable::size() const {
    switch (_cat) {
        case cargo_cat::CARGO: {
            return _cell.cargo().size();
        }
        case cargo_cat::ARTIFACT: {
            return _cell.artifacts().size();
        }
        case cargo_cat::NO_ARTIFACT: {
            return _cell.no_artifacts().size();
        }
        default: {
            return 0u;
        }
    }
}

cargo_iterator cargo_iterable::begin() {
    return cargo_iterator(_ctx, _cell, _cat, 0);
}

const_cargo_iterator cargo_iterable::begin() const { //NOLINT
    return const_cargo_iterator(_ctx, _cell, _cat, 0);
}

cargo_iterator cargo_iterable::end() {
    return cargo_iterator(_ctx, _cell, _cat, size());
}

const_cargo_iterator cargo_iterable::end() const { //NOLINT
    return const_cargo_iterator(_ctx, _cell, _cat, size());
}

const_cargo_iterator cargo_iterable::cbegin() const { //NOLINT
    return const_cargo_iterator(_ctx, _cell, _cat, 0);
}

const_cargo_iterator cargo_iterable::cend() const { //NOLINT
    return const_cargo_iterator(_ctx, _cell, _cat, size());
}

cargo_iterable::~cargo_iterable() = default;

//endregion
