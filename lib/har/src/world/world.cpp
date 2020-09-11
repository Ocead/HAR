//
// Created by Johannes on 26.05.2020.
//

#include <iomanip>

#include "world/world.hpp"

using namespace har;

world::world() : _model({ MODEL_GRID, dcoords_t() }, part::invalid()),
                 _bank({ BANK_GRID, dcoords_t() }, part::invalid()),
                 _cargo() {

}

world::world(const dcoords_t & model_size, const part & model_blank,
             const dcoords_t & bank_size, const part & bank_blank) : _model({ MODEL_GRID, model_size }, model_blank),
                                                                     _bank({ BANK_GRID, bank_size }, bank_blank),
                                                                     _cargo() {

}

world::world(dcoords_t && model_size, const part & model_blank,
             dcoords_t && bank_size, const part & bank_blank) : _model({ MODEL_GRID, model_size }, model_blank),
                                                                _bank({ BANK_GRID, bank_size }, bank_blank),
                                                                _cargo() {

}

world::world(const world & ref) : _model({ ref._model.cat(), ref._model.dim() }, part::invalid()),
                                  _bank({ ref._bank.cat(), ref._bank.dim() }, part::invalid()),
                                  _cargo() {
    for (uint_t i = 0; i < 2; ++i) {
        auto & grid = (i == 0) ? _model : _bank;
        for (auto &[pos, oclb] : ref._model) {
            auto & clb = grid.at(pos);
            clb = static_cast<const cell_base &>(oclb);
            for (auto &[use, to] : oclb.connected()) {
                clb.add_connection(use, at(to.get().position()));
            }
        }
    }
}

world::world(world && fref) noexcept: _model(std::move(fref._model)),
                                      _bank(std::move(fref._bank)),
                                      _cargo(std::move(fref._cargo)) {

}

grid & world::get_model() {
    return _model;
}

const grid & world::get_model() const {
    return _model;
}

grid & world::get_bank() {
    return _bank;
}

const grid & world::get_bank() const {
    return _bank;
}

decltype(world::_cargo) & world::cargo() {
    return _cargo;
}

const decltype(world::_cargo) & world::cargo() const {
    return _cargo;
}


grid_cell_base & world::at(const gcoords_t & pos) {
    switch (pos.cat) {
        case MODEL_GRID: {
            return _model.at(pos.pos);
        }
        case BANK_GRID: {
            return _bank.at(pos.pos);
        }
        case INVALID_GRID:
        default: {
            return grid_cell_base::invalid();
        }
    }
}

cargo_cell_base & world::at(cargo_h num) {
    return _cargo.at(num);
}

cell_base & world::at(const cell_h & hnd) {
    switch (cell_cat(hnd.index())) {
        case cell_cat::INVALID_CELL: {
            return cell_base::invalid();
        }
        case cell_cat::GRID_CELL: {
            return at(std::get<uint_t(cell_cat::GRID_CELL)>(hnd));
        }
        case cell_cat::CARGO_CELL: {
            return at(std::get<uint_t(cell_cat::CARGO_CELL)>(hnd));
        }
        default: {
            return cell_base::invalid();
        }
    }
}

void world::resize(grid_t grid, const part & pt, const dcoords_t & to) {
    switch (grid) {
        case MODEL_GRID:
            _model.resize_to(pt, to);
            return;
        case BANK_GRID:
            _bank.resize_to(pt, to);
            return;
        case INVALID_GRID:
            return;
    }
}

void world::resize(grid_t grid, const part & pt, dcoords_t && to) {
    switch (grid) {
        case MODEL_GRID:
            _model.resize_to(pt, to);
            return;
        case BANK_GRID:
            _bank.resize_to(pt, to);
            return;
        case INVALID_GRID:
            return;
    }
}

void world::minimize(grid_t grid) {
    if (grid != grid_t::INVALID_GRID) {
        ((grid == grid_t::MODEL_GRID) ? _model : _bank).minimize();
    }
}

void world::purge_part(const part & pt, const part & with) {
    _model.purge_part(pt, with);
    _bank.purge_part(pt, with);

    for (auto & c : _cargo) {
        if (c.second.logic().id() == pt.id()) {
            c.second.set_type(with);
        }
    }
}

world & world::operator=(const world & ref) {
    if (this != &ref) {
        this->~world();
        new(this) world(ref);
    }
    return *this;
}

world & world::operator=(world && fref) noexcept = default;

world::~world() = default;

ostream & har::operator<<(ostream & os, const world & world) {
    os << text('\n');
    os << string_t(text("model\n")) +
          text("name ") << std::quoted(world._model.title()) << text("\n") <<
       text("size ") << world._model.dim() << text("\n");
    os << string_t(text("bank\n")) +
          text("name ") << std::quoted(world._bank.title()) << text("\n") <<
       text("size ") << world._bank.dim() << text("\n");
    for (auto & c : world._model) {
        if (!(c.second.logic().traits() & traits::EMPTY_PART)) {
            os << '\n' << c.second;
        }
    }
    for (auto & c : world._bank) {
        if (!(c.second.logic().traits() & traits::EMPTY_PART)) {
            os << '\n' << c.second;
        }
    }
    for (auto & c : world._cargo) {
        os << '\n' << c.second;
    }
    return os;
}

std::tuple<istream &, const std::map<part_h, part> &>
har::operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, std::tuple<world &, bool_t &> world_ok) {
    string_t line;
    auto &[is, inv] = is_inv;
    auto &[world, ok] = world_ok;
    std::queue<unresolved_connection> conns;
    bool_t done = false;

    std::getline(is, line, text('\n'));
    remove_r(line);
    if (line.substr(0, 5) == text("model")) {
        std::getline(is, line, text(' '));
        remove_r(line);
        if (line.substr(0, 4) == text("name")) {
            is >> std::quoted(world._model.title());
        } else {
            ok = false;
            return is_inv;
        }
        std::getline(is >> std::ws, line, text('\n'));
        remove_r(line);
        if (line.substr(0, 5) == text("size ")) {
            stringstream ss{ line.substr(5) };
            dcoords_t dim;
            ss >> dim;
            world._model.resize_to(inv.at(PART[0]), dim);
        } else {
            ok = false;
            return is_inv;
        }
    } else {
        ok = false;
        return is_inv;
    }
    std::getline(is, line, text('\n'));
    remove_r(line);
    if (line.substr(0, 4) == text("bank")) {
        std::getline(is, line, text(' '));
        remove_r(line);
        if (line.substr(0, 4) == text("name")) {
            is >> std::quoted(world._bank.title());
        } else {
            ok = false;
            return is_inv;
        }
        std::getline(is >> std::ws, line, text('\n'));
        remove_r(line);
        if (line.substr(0, 5) == text("size ")) {
            stringstream ss{ line.substr(5) };
            dcoords_t dim;
            ss >> dim;
            world._bank.resize_to(inv.at(PART[0]), dim);
        } else {
            ok = false;
            return is_inv;
        }
    } else {
        ok = false;
        return is_inv;
    }

    std::getline(is, line, text('\n'));
    remove_r(line);
    while (!is.eof() && !done) {
        char c = is.peek();
        switch (c) {
            case 'c': {
                cargo_cell_base cclb{ CARGO[0], part::invalid() };
                is_inv >> cclb;
                cclb.transit();
                world.cargo().try_emplace(cclb.id(), std::forward<cargo_cell_base>(cclb));
                break;
            }
            case 'g': {
                grid_cell_base gclb{ part::invalid() };
                is_inv >> std::tie(gclb, conns);
                gclb.transit();
                while (!conns.empty()) {
                    unresolved_connection uconn = conns.front();
                    uconn.base.get().add_connection(uconn.use, world.at(uconn.pos));
                    conns.pop();
                }
                world.at(gclb.position()).adopt(std::move(gclb));
                break;
            }
            default: {
                done = true;
                break;
            }
        }
    }
    return is_inv;
}
