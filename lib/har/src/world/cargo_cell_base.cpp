//
// Created by Johannes on 26.05.2020.
//

#include <world/cargo_cell_base.hpp>

#include <world/grid_cell_base.hpp>
#include <utility>

using namespace har;

cargo_cell_base & cargo_cell_base::invalid() {
    static cargo_cell_base cclb{ CARGO[0], part::invalid(), ccoords_t(std::nan(""), std::nan("")) };
    return cclb;
}

cargo_cell_base::cargo_cell_base(cargo_h id, const part & part, ccoords_t pos, ccoords_t size, ccoords_t radius) :
        cell_base(part),
        _id(id),
        _position(std::move(pos)),
        _size(std::move(size)),
        _radius(std::move(radius)),
        _valid(true),
        _overlays() {

}

cargo_cell_base::cargo_cell_base(cargo_h id, const cell_base & cl, ccoords_t pos, ccoords_t size, ccoords_t radius) :
        cell_base(cl),
        _id(id),
        _position(pos),
        _size(std::move(size)),
        _radius(std::move(radius)),
        _valid(true),
        _overlays() {

}

cargo_cell_base::cargo_cell_base(cargo_cell_base && fref) noexcept: cell_base(std::forward<cell_base>(fref)),
                                                                    _id(fref._id),
                                                                    _position(fref._position),
                                                                    _size(fref._size),
                                                                    _radius(fref._radius),
                                                                    _valid(fref._valid),
                                                                    _overlays(std::move(fref._overlays)) {
    for (auto & o : _overlays) {
        auto * ptr = o.second.first;
        if (ptr) {
            ptr->bend(*this);
        }
    }
}

cargo_h cargo_cell_base::id() const {
    return _id;
}

const ccoords_t & cargo_cell_base::position() const {
    return _position;
}

const ccoords_t & cargo_cell_base::size() const {
    return _size;
}

const ccoords_t & cargo_cell_base::radius() const {
    return _radius;
}

ccoords_t cargo_cell_base::move_delta() const {
    return _move_delta;
}

void cargo_cell_base::move_by(ccoords_t delta) {
    _move_delta += delta;
}


void cargo_cell_base::move_by(dcoords_t delta) {
    _move_delta += ccoords_t(delta);
}

bool_t cargo_cell_base::operator==(const cargo_cell_base & rhs) const {
    return _logic.get() == rhs._logic.get() &&
           _position == rhs._position &&
           _size == rhs._size &&
           _radius == _radius;
}

cargo_cell_base & cargo_cell_base::operator=(cargo_cell_base && fref) noexcept = default;

cargo_cell_base::~cargo_cell_base() {
    for (auto seg : _overlays) {
        if (seg.second.second) {
            //TODO: Remove artifacts from grid_cells
        }
    }
}

ostream & har::operator<<(ostream & os, const cargo_cell_base & cell) {
    os << "ccell\n"
       << "at " << cell._position << '\n'
       << static_cast<const class cell_base &>(cell);
    return os;
}

std::tuple<istream &, const std::map<part_h, part> &>
har::operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, cargo_cell_base & cell) {
    auto & is = std::get<0>(is_inv);

    string_t line;
    std::getline(is, line, text('\n'));
    if (line == text("ccell")) {
        std::getline(is, line, text('\n'));
        if (line.substr(0, 3) == text("at ")) {
            stringstream ss{ line.substr(3) };
            ss >> cell._position;
            is_inv >> static_cast<class cell_base &>(cell);
        } else {
            std::string eline(line.begin(), line.end());
            raise(*new exception::cell_format_error("har::operator>>", eline));
        }
    } else {
        std::string eline(line.begin(), line.end());
        raise(*new exception::cell_format_error("har::operator>>", eline));
    }
    return is_inv;
}
