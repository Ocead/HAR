//
// Created by Johannes on 26.05.2020.
//

#include "world/grid_cell_base.hpp"

using namespace har;

//region unresolved_connection

istream & har::operator>>(istream & is, unresolved_connection & conn) {
    is >> conn.use;
    is.ignore();
    is >> conn.pos;

    return is;
}

//endregion

//region grid_cell_base

grid_cell_base & grid_cell_base::invalid() {
    static grid_cell_base igclb{ part::invalid(), gcoords_t(INVALID_GRID, ~0, ~0) };
    return igclb;
}; //NOLINT

grid_cell_base::grid_cell_base(const part & part, const gcoords_t & gc, std::array<grid_cell_base *, 4> neighbors) :
        cell_base(part),
        _position(gc),
        _connected(),
        _iconnected(),
        _cargo(),
        _artifacts(),
        _no_artifacts(),
        _neighbors(neighbors) {
    for (auto d : direction::cardinal) {
        set_neighbor(d, _neighbors[d]);
    }
}

grid_cell_base::grid_cell_base(const part & part, const gcoords_t & gc,
                               grid_cell_base * up,
                               grid_cell_base * right,
                               grid_cell_base * down,
                               grid_cell_base * left) : grid_cell_base(part, gc, { up, right, down, left }) {

}

grid_cell_base::grid_cell_base(const cell_base & cl, const gcoords_t & gc, std::array<grid_cell_base *, 4> neighbors) :
        cell_base(cl),
        _position(gc),
        _connected(),
        _iconnected(),
        _cargo(),
        _artifacts(),
        _no_artifacts(),
        _neighbors(neighbors) {
    for (auto d : direction::cardinal) {
        set_neighbor(d, _neighbors[d]);
    }
}

grid_cell_base::grid_cell_base(const cell_base & cl, const gcoords_t & gc,
                               grid_cell_base * up,
                               grid_cell_base * right,
                               grid_cell_base * down,
                               grid_cell_base * left) : grid_cell_base(cl, gc, { up, right, down, left }) {

}

grid_cell_base::grid_cell_base(grid_cell_base && fref) noexcept: cell_base(std::forward<cell_base>(fref)),
                                                                 _position(fref._position),
                                                                 _connected(std::move(fref._connected)),
                                                                 _iconnected(std::move(fref._iconnected)),
                                                                 _cargo(std::move(fref._cargo)),
                                                                 _artifacts(std::move(fref._artifacts)),
                                                                 _no_artifacts(std::move(fref._artifacts)),
                                                                 _neighbors(fref._neighbors) {
    for (auto d : direction::cardinal) {
        auto ptr = _neighbors[d];
        if (ptr)
            ptr->_neighbors[!d] = this;
    }

    for (auto & pair : _connected) {
        pair.second.get().bend_connection(*&fref, *this);
    }
}

void grid_cell_base::add_connection_inverse(grid_cell_base & cell) {
    _iconnected[&cell]++;
}

void grid_cell_base::bend_connection(grid_cell_base & from, grid_cell_base & to) {
    std::for_each(_connected.begin(), _connected.end(), [&](auto & pair) {
        if (&pair.second.get() == &from) {
            pair.second = std::ref(to);
        }
    });

    auto it = _iconnected.find(&from);
    if (it != _iconnected.end()) {
        auto node = _iconnected.extract(it);
        node.key() = &to;
        _iconnected.insert(std::move(node));
    }
}

void grid_cell_base::remove_connection_inverse(grid_cell_base & cell) {
    if (!--_iconnected.at(&cell)) {
        _iconnected.erase(&cell);
    }
}

void grid_cell_base::remove_inverse_connection_inverse(grid_cell_base & cell) {
    for (auto it = _connected.begin(); it != _connected.end();) {
        if (&it->second.get() == &cell) {
            it = _connected.erase(it);
        } else {
            ++it;
        }
    }
};

const gcoords_t & grid_cell_base::position() const {
    return _position;
}

const decltype(grid_cell_base::_connected) & grid_cell_base::connected() const {
    return _connected;
}

const decltype(grid_cell_base::_iconnected) & grid_cell_base::iconnected() const {
    return _iconnected;
}

const decltype(grid_cell_base::_cargo) & grid_cell_base::cargo() const {
    return _cargo;
}

const decltype(grid_cell_base::_artifacts) & grid_cell_base::artifacts() const {
    return _artifacts;
}

const decltype(grid_cell_base::_no_artifacts) & grid_cell_base::no_artifacts() const {
    return _no_artifacts;
}

grid_cell_base * grid_cell_base::get_neighbor(direction_t dir) noexcept {
    return _neighbors[dir];
}

const grid_cell_base * grid_cell_base::get_neighbor(direction_t dir) const noexcept {
    return _neighbors[dir];
}

void grid_cell_base::set_neighbor(direction_t dir, grid_cell_base * ptr) noexcept {
    _neighbors[dir] = ptr;
    if (ptr) {
        ptr->_neighbors[!dir] = this;
    }
}

void grid_cell_base::unset_neighbor(direction_t dir) noexcept {
    auto ptr = _neighbors[dir];
    if (ptr) {
        ptr->_neighbors[!dir] = nullptr;
    }
    _neighbors[dir] = nullptr;
}

grid_cell_base * grid_cell_base::get_connected(direction_t dir) noexcept {
    auto it = _connected.find(dir);
    if (it != _connected.end()) {
        return &it->second.get();
    } else {
        return nullptr;
    }
}

const grid_cell_base * grid_cell_base::get_connected(direction_t dir) const noexcept {
    auto it = _connected.find(dir);
    if (it != _connected.end()) {
        return &it->second.get();
    } else {
        return nullptr;
    }
}

grid_cell_base * grid_cell_base::get_cell(direction_t dir) noexcept {
    grid_cell_base * ptr;
    if (dir == direction::NONE) {
        ptr = this;
    } else if (int_t(dir) < 0) {
        ptr = get_neighbor(dir);
    } else {
        ptr = get_connected(dir);
    }
    return ptr ? ptr : &grid_cell_base::invalid();
}

const grid_cell_base * grid_cell_base::get_cell(direction_t dir) const noexcept {
    if (int_t(dir) < 0) {
        return get_neighbor(dir);
    } else {
        return get_connected(dir);
    }
}

void grid_cell_base::add_connection(direction_t use, grid_cell_base & cell) {
    _connected.emplace(use, std::ref(cell));
    cell.add_connection_inverse(*this);
}

void grid_cell_base::change_connection_use(direction_t use, direction_t new_use) {
    auto node = _connected.extract(use);
    node.key() = new_use;
    _connected.insert(std::move(node));
}

void grid_cell_base::remove_connection(direction_t use) {
    auto conn = _connected.at(use);
    conn.get().remove_connection_inverse(*this);
    _connected.erase(use);
}

void grid_cell_base::add_cargo(cargo_h num, artifact && arti) {
    _cargo.emplace(num, std::forward<artifact>(arti));
}

artifact grid_cell_base::remove_cargo(cargo_h num) {
    auto node = _cargo.extract(num);
    return std::move(node.mapped());
}

void grid_cell_base::add_artifact(artifact_h num, artifact && arti) {
    _artifacts.emplace(num, std::forward<artifact>(arti));
}

artifact grid_cell_base::remove_artifact(artifact_h num) {
    auto node = _artifacts.extract(num);
    return std::move(node.mapped());
}

void grid_cell_base::add_no_artifact(artifact_h num, artifact && arti) {
    _no_artifacts.emplace(num, std::forward<artifact>(arti));
}

artifact grid_cell_base::remove_no_artifact(artifact_h num) {
    auto node = _no_artifacts.extract(num);
    return std::move(node.mapped());
}

void grid_cell_base::move_to(const dcoords_t & pos) {
    auto delta = pos - _position.pos;
    for (auto & c : _cargo) {
        c.second.move_by(delta);
    }

    _position.pos = pos;
}

void grid_cell_base::swap_with(grid_cell_base & rhs) {
    std::swap(_logic, rhs._logic);
    std::swap(_properties, rhs._properties);
    std::swap(_intermediate, rhs._intermediate);
    std::swap(_connected, rhs._connected);
    bend_connection(rhs, *this);
    bend_connection(*this, rhs);
}

bool_t grid_cell_base::is_placed() const {
    for (auto dir : direction::cardinal) {
        if (_neighbors[dir]) {
            return true;
        }
    }
    return _position.cat != INVALID_GRID;
}

grid_cell_base & grid_cell_base::adopt(grid_cell_base && fref) noexcept {
    if (this != &fref) {
        for (auto & conn : _connected) {
            conn.second.get().remove_connection_inverse(*this);
        }
        static_cast<cell_base &>(*this) = static_cast<cell_base &&>(fref);
        _position = fref._position;
        _connected = std::move(fref._connected);
        _cargo = std::move(fref._cargo);
        _artifacts = std::move(fref._artifacts);
        _no_artifacts = std::move(fref._artifacts);
    }
    for (auto & conn : _connected) {
        conn.second.get().bend_connection(*&fref, *this);
    }
    return *this;
}

bool_t grid_cell_base::operator==(const grid_cell_base & rhs) const {
    return _logic.get() == rhs._logic.get() &&
           _position == rhs._position;
}

grid_cell_base & grid_cell_base::operator=(grid_cell_base && fref) noexcept {
    if (this != &fref) {
        for (auto & conn : _connected) {
            conn.second.get().remove_connection_inverse(*this);
        }
        static_cast<cell_base &>(*this) = static_cast<cell_base &&>(fref);
        _position = fref._position;
        _connected = std::move(fref._connected);
        _neighbors = fref._neighbors;
        _cargo = std::move(fref._cargo);
        _artifacts = std::move(fref._artifacts);
        _no_artifacts = std::move(fref._artifacts);
    }
    for (auto dir : direction::cardinal) {
        auto ptr = _neighbors[dir];
        if (ptr) {
            ptr->set_neighbor(!dir, this);
        }
    }
    for (auto & conn : _connected) {
        conn.second.get().bend_connection(*&fref, *this);
    }
    return *this;
}

grid_cell_base::~grid_cell_base() {
    for (auto d : direction::cardinal) {
        unset_neighbor(d);
    }
    for (auto & c : _connected) {
        c.second.get().remove_connection_inverse(*this);
    }
    for (auto & c : _iconnected) {
        c.first->remove_inverse_connection_inverse(*this);
    }
}

ostream & har::operator<<(ostream & os, const grid_cell_base & cell) {
    os << string_t(text("gcell\n"))
          + text("at ") << cell.position() << text('\n')
       << static_cast<const class cell_base &>(cell);
    for (auto & c : cell._connected) {
        const auto & ncell = c.second.get();
        const auto & nneigh = ncell.connected();
        os << text("wire ") << c.first << " " << ncell.position() << '\n';
    }
    return os;
}

std::tuple<istream &, const std::map<part_h, part> &>
har::operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv,
                std::tuple<grid_cell_base &, std::queue<unresolved_connection> &> cell_conn) {
    auto & is = std::get<0>(is_inv);

    auto & cell = std::get<0>(cell_conn);
    auto & conns = std::get<1>(cell_conn);

    string_t line;
    std::getline(is, line, text('\n'));
    remove_r(line);
    if (line == text("gcell")) {
        std::getline(is, line, text('\n'));
        remove_r(line);
        if (line.substr(0, 3) == text("at ")) {
            stringstream ss{ line.substr(3) };
            ss >> cell._position;
            is_inv >> static_cast<class cell_base &>(cell);
            while (std::getline(is, line, text('\n'))) {
                remove_r(line);
                if (line.empty()) {
                    break;
                }
                if (line.substr(0, 5) == text("wire ")) {
                    stringstream wss{ line };
                    wss.ignore(5);

                    unresolved_connection conn{ cell };

                    wss >> conn;
                    conns.push(conn);
                } else {
                    std::string eline{ line.begin(), line.end() };
                    raise(*new exception::cell_format_error("har::operator>>", eline));
                }
            }
        } else {
            std::string eline{ line.begin(), line.end() };
            raise(*new exception::cell_format_error("har::operator>>", eline));
        }
    } else {
        std::string eline{ line.begin(), line.end() };
        raise(*new exception::cell_format_error("har::operator>>", eline));
    }
    return is_inv;
}

//endregion

//region swap

template<>
void std::swap<har::grid_cell_base>(har::grid_cell_base & a, har::grid_cell_base & b) noexcept {
    grid_cell_base c = std::forward<har::grid_cell_base>(b);
    b = std::forward<har::grid_cell_base>(a);
    a = std::forward<har::grid_cell_base>(c);
}

//endregion
