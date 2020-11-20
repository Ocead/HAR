//
// Created by Johannes on 10.06.2020.
//

#include "world/grid.hpp"

using namespace har;

grid::grid() : _cat(INVALID_GRID), _size(0, 0), _data() {

}

grid::grid(const gcoords_t & size, const part & blank_part) : _cat(size.cat), _size(), _data() {
    resize_by(blank_part, size.pos);
}

grid::grid(gcoords_t && size, const part & blank_part) : _cat(std::forward<grid_t>(size.cat)),
                                                         _size(),
                                                         _data() {
    dcoords_t sb = size.pos;
    resize_by(blank_part, sb);
}

grid::grid(grid && fref) noexcept: _cat(fref._cat),
                                   _size(fref._size),
                                   _title(std::move(fref._title)),
                                   _data(std::move(fref._data)) {

}

grid_cell_base grid::create_cell(const part & pt, const dcoord_t & x, const dcoord_t & y) {
    return grid_cell_base(
            pt,
            gcoords_t(_cat, x, y),
            { nullptr });
}

grid_t grid::cat() const {
    return _cat;
}

grid_cell_base & grid::at(const har::dcoords_t & pos) {
    if (pos.x < _size.x && pos.y < _size.y) {
        return _data.at(pos);
    } else {
        DEBUG_LOG("index " << pos << " exceeds the dimension of the grid (" << gcoords_t(_cat, _size) << ")");
        return grid_cell_base::invalid();
    }
}

const grid_cell_base & grid::at(const har::dcoords_t & pos) const {
    if (pos.x < _size.x && pos.y < _size.y) {
        return _data.at(pos);
    } else {
        DEBUG_LOG("index " << pos << " exceeds the dimension of the grid (" << gcoords_t(_cat, _size) << ")");
        return grid_cell_base::invalid();
    }
}

dcoords_t grid::dim() const {
    return _size;
}

string_t & grid::title() {
    return _title;
}

const string_t & grid::title() const {
    return _title;
}

void grid::insert_column(const part & pt) {
    insert_column(pt, _size.x);
}

void grid::insert_column(const part & pt, const dcoord_t & x) {
    //Alte Spalten verschieben
    for (dcoord_t ix = _size.x; ix > x; --ix) {
        for (dcoord_t iy = 0; iy < _size.y; ++iy) {
            auto npos = (ix, iy);
            auto pair = _data.extract((ix - 1, iy));
            pair.key() = npos;
            auto & c = _data.insert(std::move(pair)).position->second;
            c.move_to(npos);
        }
    }
    //Neue Spalte einfügen
    for (dcoord_t y = 0; y < _size.y; ++y) {
        auto & c = _data.emplace((x, y), create_cell(pt, x, y)).first->second;
        auto pos = (x, y);
        if (_size.x > 0) {
            if (x > 0) {
                c.set_neighbor(direction::LEFT, &_data.at(pos + dcoords_t(direction::LEFT)));
            }
            if (x < _size.x - 1) {
                c.set_neighbor(direction::RIGHT, &_data.at(pos + dcoords_t(direction::RIGHT)));
            }
        }
        if (y > 0) {
            c.set_neighbor(direction::UP, &_data.at(pos + dcoords_t(direction::UP)));
        }
    }
    //TODO: Alle Cargos bewegter Zellen invalidieren

    _size.x++;
}

void grid::insert_row(const part & pt) {
    insert_row(pt, _size.y);
}

void grid::insert_row(const part & pt, const dcoord_t & y) {
    //Alte Spalten verschieben
    for (dcoord_t iy = _size.y; iy > y; --iy) {
        for (dcoord_t ix = 0; ix < _size.x; ++ix) {
            auto npos = (ix, iy);
            auto pair = _data.extract((ix, iy - 1));
            pair.key() = npos;
            auto & c = _data.insert(std::move(pair)).position->second;
            c.move_to(npos);
        }
    }
    //Neue Spalte einfügen
    for (dcoord_t x = 0; x < _size.x; ++x) {
        auto & c = _data.emplace((x, y), create_cell(pt, x, y)).first->second;
        auto pos = (x, y);
        if (_size.y > 0) {
            if (y > 0) {
                c.set_neighbor(direction::UP, &_data.at(pos + dcoords_t(direction::UP)));
            }
            if (y < _size.y - 1) {
                c.set_neighbor(direction::DOWN, &_data.at(pos + dcoords_t(direction::DOWN)));
            }
        }
        if (x > 0) {
            c.set_neighbor(direction::LEFT, &_data.at(pos + dcoords_t(direction::LEFT)));
        }
    }
    //TODO: Alle Cargos bewegter Zellen invalidieren

    _size.y++;
}

void grid::remove_column() {
    remove_column(_size.x - 1);
}

void grid::remove_column(dcoord_t x) {
    //Spalte entfernen
    for (dcoord_t y = 0; y < _size.y; ++y) {
        _data.erase((x, y));
    }

    //Positionen und Nachbarn aller bewegten Zellen aktualisieren
    for (dcoord_t ix = x + 1; ix < _size.x; ++ix) {
        for (dcoord_t iy = 0; iy < _size.y; ++iy) {
            auto node = _data.extract((ix, iy));
            node.key() = (ix - 1, iy);
            node.mapped().move_to((ix - 1, iy));
            _data.insert(std::move(node));
        }
    }

    _size.x--;

    if (x > 0 && x < _size.x) {
        for (dcoord_t y = 0; y < _size.y; ++y) {
            _data.at((x, y)).set_neighbor(direction::LEFT, &_data.at((x - 1, y)));
        }
    }

    //TODO: Alle Cargos bewegter Zellen invalidieren
}

void grid::remove_row() {
    remove_row(_size.y - 1);
}

void grid::remove_row(const dcoord_t & y) {
    //Zeile entfernen
    for (dcoord_t x = 0; x < _size.x; ++x) {
        _data.erase((x, y));
    }

    //Positionen und Nachbarn aller bewegten Zellen aktualisieren
    for (dcoord_t iy = y + 1; iy < _size.y; ++iy) {
        for (dcoord_t ix = 0; ix < _size.x; ++ix) {
            auto node = _data.extract((ix, iy));
            node.key() = (ix, iy - 1);
            node.mapped().move_to((ix, iy - 1));
            _data.insert(std::move(node));
        }
    }

    _size.y--;

    if (y > 0 && y < _size.y) {
        for (dcoord_t x = 0; x < _size.x; ++x) {
            _data.at((x, y)).set_neighbor(direction::UP, &_data.at((x, y - 1)));
        }
    }

    //TODO: Alle Cargos bewegter Zellen invalidieren
}

void grid::resize_to(const part & pt, const dcoords_t & to) {
    return resize_by(pt, to - _size);
}

void grid::resize_by(const part & pt, const dcoords_t & by) {
    if (by.x > 0) {
        for (dcoord_t x = 0; x < by.x; ++x) {
            insert_column(pt);
        }
    } else if (by.x < 0) {
        for (dcoord_t x = by.x; x < 0; ++x) {
            remove_column();
        }
    }

    if (by.y > 0) {
        for (dcoord_t y = 0; y < by.y; ++y) {
            insert_row(pt);
        }
    } else if (by.y < 0) {
        for (dcoord_t y = by.y; y < 0; ++y) {
            remove_row();
        }
    }
}

void grid::minimize() {

}

void grid::purge_part(const part & pt, const part & with) {
    for (auto & c : _data) {
        auto & base = c.second;
        if (base.logic().id() == pt.id()) {
            base.set_type(with);
        }
    }
}

decltype(grid::_data)::iterator grid::begin() {
    return _data.begin();
}

decltype(grid::_data)::const_iterator grid::begin() const {
    return _data.begin();
}

decltype(grid::_data)::iterator grid::end() {
    return _data.end();
}

decltype(grid::_data)::const_iterator grid::end() const {
    return _data.end();
}

decltype(grid::_data)::const_iterator grid::cbegin() const {
    return _data.cbegin();
}

decltype(grid::_data)::const_iterator grid::cend() const {
    return _data.cend();
}

grid & grid::operator=(grid && fref) noexcept = default;

grid::~grid() = default;

//endregion