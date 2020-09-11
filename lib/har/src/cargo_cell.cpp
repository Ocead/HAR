//
// Created by Johannes on 19.06.2020.
//

#include <har/cargo_cell.hpp>

#include "logic/context.hpp"

using namespace har;

cargo_cell::cargo_cell(context & ctx,
                       cargo_cell_base & cargo_base,
                       grid_cell_base & cell_base) : cell(ctx, cargo_base),
                                                     _base(cell_base) {

}

cargo_cell_base & cargo_cell::as_cargo_cell_base() noexcept {
    return static_cast<cargo_cell_base &>(_cell);
}

const cargo_cell_base & cargo_cell::as_cargo_cell_base() const noexcept {
    return static_cast<cargo_cell_base &>(_cell);
}

ccoords_t cargo_cell::position() const {
    return as_cargo_cell_base().position() - _base.get().position().pos;
}

void cargo_cell::move(ccoords_t by) {
    cargo_cell_base & ccell = as_cargo_cell_base();
    ccell.move_by(by);
    if (as_cargo_cell_base().id() >= CARGO[0]) {
        _ctx.move(ccell.id());
    }
}

void cargo_cell::destroy() {
    cargo_cell_base & ccell = as_cargo_cell_base();
    if (as_cargo_cell_base().id() >= CARGO[0]) {
        _ctx.destroy(ccell.id());
    }
}

cargo_cell::~cargo_cell() = default;
