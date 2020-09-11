//
// Created by Johannes on 26.05.2020.
//

#include "world/artifact.hpp"

using namespace har;

artifact::artifact(cargo_cell_base & base, dcoords_t section) : _base(base), _section(section) {

}

artifact::artifact(artifact && fref) noexcept: _base(fref._base), _section(fref._section) {

}

cargo_cell_base & artifact::base() const {
    return _base;
}

dcoords_t artifact::section() const {
    return _section;
}

void artifact::move_by(ccoords_t delta) {
    _base.get().move_by(delta);
}

void artifact::move_by(dcoords_t delta) {
    _base.get().move_by(delta);
}

void artifact::bend(cargo_cell_base & now) {
    _base = now;
}

artifact & artifact::operator=(artifact && fref) noexcept {
    if (this != &fref) {
        _base = fref._base;
        _section = fref._section;
    }
    return *this;
}

artifact::~artifact() {
    //TODO: Unregister artifact from cargo before deleting
}
