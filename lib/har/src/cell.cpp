//
// Created by Johannes on 05.07.2020.
//

#include <har/cell.hpp>
#include <har/cargo_cell.hpp>
#include <har/grid_cell.hpp>

#include "logic/context.hpp"
#include "world/cargo_cell_base.hpp"
#include "world/grid_cell_base.hpp"

using namespace har;

cell::cell(context & ctx, cargo_cell_base & base) : _ctx(ctx), _cell(base), _cat(cell_cat::CARGO_CELL) {

}

cell::cell(context & ctx, grid_cell_base & base) : _ctx(ctx), _cell(base), _cat(cell_cat::GRID_CELL) {

}

cargo_cell & cell::as_cargo_cell() {
    return dynamic_cast<cargo_cell &>(*this);
}

const cargo_cell & cell::as_cargo_cell() const {
    return dynamic_cast<const cargo_cell &>(*this);
}

grid_cell & cell::as_grid_cell() {
    return dynamic_cast<grid_cell &>(*this);
}

const grid_cell & cell::as_grid_cell() const {
    return dynamic_cast<const grid_cell &>(*this);
}

const part & cell::logic() {
    return _cell.logic();
}

cell_cat cell::cat() const {
    return _cat;
}

traits_h cell::traits() const {
    return _cell.logic().traits();
}

bool_t cell::has(of id) const {
    auto & map = _cell.logic().model();
    return map.find(id) != map.end();
}

void cell::adopt(const cell_base & ref) {
    if (_cell.adopt(ref) && &_ctx != &context::invalid()) {
        if (_cat == cell_cat::GRID_CELL) {
            _ctx.changed().insert(static_cast<grid_cell_base &>(_cell).position());
        } else if (_cat == cell_cat::CARGO_CELL) {
            _ctx.changed().insert(static_cast<cargo_cell_base &>(_cell).id());
        }
    }
}

void cell::adopt(cell_base && fref) {
    if (_cell.adopt(std::forward<cell_base>(fref)) && &_ctx != &context::invalid()) {
        if (_cat == cell_cat::GRID_CELL) {
            _ctx.changed().insert(static_cast<grid_cell_base &>(_cell).position());
        } else if (_cat == cell_cat::CARGO_CELL) {
            _ctx.changed().insert(static_cast<cargo_cell_base &>(_cell).id());
        }
    }
}

void cell::message(const string_t & header, const string_t & content) {
    switch(_cat) {
        case GRID_CELL: {
            stringstream ss;
            ss << static_cast<grid_cell_base &>(_cell).position() << ": " << header;
            _ctx.message(ss.str(), content);
            break;
        }
        case CARGO_CELL: {
            stringstream ss;
            ss << "CARGO[" << uint_t(static_cast<cargo_cell_base &>(_cell).id()) << "]: " << header;
            _ctx.message(ss.str(), content);
            break;
        }
        default: {
            break;
        }
    }
}

property cell::get(of id, bool_t now) {
    return property(_ctx, _cell, _cat, id, datatype(_cell.get(id).index()), now);
}

property cell::operator[](of id) {
    return property(_ctx, _cell, _cat, id, datatype(_cell.get(id).index()));
}

const property cell::operator[](of id) const { //NOLINT
    return property(_ctx, _cell, _cat, id, datatype(_cell.get(id).index()));
}

cell::~cell() = default;
