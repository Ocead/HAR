//
// Created by Johannes on 05.07.2020.
//

#include <har/full_cell.hpp>

#include "logic/context.hpp"

using namespace har;

//region full_cargo_cell

full_cargo_cell::full_cargo_cell(context & ctx, cargo_cell_base & cell) : cargo_cell(ctx,
                                                                                     cell,
                                                                                     grid_cell_base::invalid()) {

}

const ccoords_t & full_cargo_cell::absolute_position() const {
    return as_cargo_cell_base().position();
}

void full_cargo_cell::set_part(const part & pt) {
    auto & cell = as_cargo_cell_base();
    _ctx.redraw().emplace(cell.id());
    cell.set_type(pt);
}

void full_cargo_cell::swap_with(cargo_cell & ccl) {
    //TODO: Implement
}

string_t full_cargo_cell::to_string() const {
    stringstream ss{ };
    ss << as_cargo_cell_base();
    return ss.str();
}

full_cargo_cell::~full_cargo_cell() = default;

//endregion

//region full_grid_cell

full_grid_cell::full_grid_cell(context & ctx, grid_cell_base & cell,
                               direction_t dir) : grid_cell(ctx, cell, dir) {

}

const gcoords_t & full_grid_cell::position() const {
    return as_grid_cell_base().position();
}

void full_grid_cell::set_part(const part & pt) {
    auto & gclb = as_grid_cell_base();
    grid_cell gcl{ _ctx, gclb };
    pt.clear(gcl);
    _ctx.redraw().emplace(gclb.position());
    gclb.set_type(pt);
    pt.init_standard(gclb);
    gclb.transit();
    pt.init_relative(gcl);
    _ctx.changed().emplace(gclb.position());
}

bool_t full_grid_cell::has_connection(direction_t use) {
    return as_grid_cell_base().get_connected(use);
}

void full_grid_cell::add_connection(direction_t use, full_grid_cell & fgcl) {
    _ctx.connect({ as_grid_cell_base(), use, fgcl.position() });
}

void full_grid_cell::add_connection(direction_t use, full_grid_cell && fgcl) {
    _ctx.connect({ as_grid_cell_base(), use, fgcl.position() });
}

void full_grid_cell::remove_connection(direction_t use) {
    _ctx.disconnect({ as_grid_cell_base(), use, gcoords_t() });
}

void full_grid_cell::swap_with(full_grid_cell & fgcl) {
    as_grid_cell_base().swap_with(fgcl.as_grid_cell_base());
    _ctx.change(position());
    _ctx.change(fgcl.position());
    _ctx.draw(position());
    _ctx.draw(fgcl.position());
}

void full_grid_cell::swap_with(full_grid_cell && fgcl) {
    as_grid_cell_base().swap_with(fgcl.as_grid_cell_base());
    _ctx.change(position());
    _ctx.change(fgcl.position());
    _ctx.draw(position());
    _ctx.draw(fgcl.position());
}

string_t full_grid_cell::to_string() const {
    stringstream ss{ };
    ss << as_grid_cell_base();
    return ss.str();
}

full_grid_cell full_grid_cell::operator[](direction_t dir) {
    return full_grid_cell(_ctx, *as_grid_cell_base().get_cell(dir), direction::NONE);
}

full_grid_cell::~full_grid_cell() = default;

//endregion
