//
// Created by Johannes on 19.07.2020.
//

#include <har/sketch_cell.hpp>

#include "logic/context.hpp"
#include "world/cargo_cell_base.hpp"
#include "world/grid_cell_base.hpp"

using namespace har;

//region sketch_cargo_cell

sketch_cargo_cell::sketch_cargo_cell(const part & pt) : cargo_cell(context::invalid(),
                                                                   *new cargo_cell_base(CARGO[0], pt),
                                                                   grid_cell_base::invalid()),
                                                        _cclb() {
    _cclb = std::unique_ptr<cargo_cell_base>(&as_cargo_cell_base());
    pt.init_standard(*_cclb);
    _cclb->transit();
}

sketch_cargo_cell::~sketch_cargo_cell() noexcept = default;

//endregion

//region sketch_grid_cell

sketch_grid_cell::sketch_grid_cell(const part & pt) : grid_cell(context::invalid(),
                                                                *new grid_cell_base(pt, gcoords_t()),
                                                                direction::NONE),
                                                                _gclb() {
    _gclb = std::unique_ptr<grid_cell_base>(&as_grid_cell_base());
    pt.init_standard(*_gclb);
    _gclb->transit();
}

sketch_grid_cell::~sketch_grid_cell() noexcept = default;

//endregion
