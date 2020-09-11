//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_GRID_CELL_BASE_HPP
#define HAR_GRID_CELL_BASE_HPP

#include <map>
#include <queue>
#include <vector>

#include <har/part.hpp>
#include <har/value.hpp>

#include "world/artifact.hpp"
#include "har/cell_base.hpp"

namespace har {

    template<typename T>
    class adjacent {
    private:
        std::array<T, 4> _data;

        inline T & get_at(direction_t dir) {
            switch (dir) {
                case direction::UP:
                    return _data[0];
                case direction::RIGHT:
                    return _data[1];
                case direction::DOWN:
                    return _data[2];
                case direction::LEFT:
                    return _data[3];
                default:
                    raise(exception::invalid_direction("har::grid_cell_base", dir));
            }
        }

    public:
        explicit adjacent(std::array<T, 4> array) : _data(array) {

        }

        adjacent(const adjacent & ref) = default;

        adjacent(adjacent && fref) noexcept = default;

        adjacent & operator=(const adjacent & ref) = default;

        adjacent & operator=(adjacent && fref) noexcept = default;

        T & operator[](direction_t dir) {
            return get_at(dir);
        }

        const T & operator[](direction_t dir) const {
            return get_at(dir);
        }

        ~adjacent() = default;
    };

    struct connection_base {
        direction_t inv_use;
        std::reference_wrapper<grid_cell_base> cell;
    };

    struct unresolved_connection {
        std::reference_wrapper<grid_cell_base> base;
        direction_t use{ direction::PIN[0] };
        gcoords_t pos{ INVALID_GRID, 0, 0 };

        friend istream & operator>>(istream & is, unresolved_connection & conn);
    };

    istream & operator>>(istream & is, unresolved_connection & conn);

    class grid_cell_base : public cell_base {
    private:
        gcoords_t _position;

        map<direction_t, std::reference_wrapper<grid_cell_base>> _connected;
        map<grid_cell_base *, uint_t> _iconnected;
        map<cargo_h, artifact> _cargo;
        map<cargo_h, artifact> _artifacts;
        map<cargo_h, artifact> _no_artifacts;

        mutable adjacent<grid_cell_base *> _neighbors;

        void add_connection_inverse(grid_cell_base & cell);

        void bend_connection(grid_cell_base & from, grid_cell_base & to);

        void remove_connection_inverse(grid_cell_base & cell);

        void remove_inverse_connection_inverse(grid_cell_base & cell);

    public:
        using connected_map_type = decltype(_connected);

        static grid_cell_base & invalid();

        explicit grid_cell_base(const part & part = part::invalid(), const gcoords_t & gc = gcoords_t(),
                                std::array<grid_cell_base *, 4> neighbors = { nullptr });

        grid_cell_base(const part & part, const gcoords_t & gc,
                       grid_cell_base * up,
                       grid_cell_base * right,
                       grid_cell_base * down,
                       grid_cell_base * left);

        grid_cell_base(const cell_base & cell, const gcoords_t & gc,
                       std::array<grid_cell_base *, 4> neighbors = { nullptr });

        grid_cell_base(const cell_base & cell, const gcoords_t & gc,
                       grid_cell_base * up,
                       grid_cell_base * right,
                       grid_cell_base * down,
                       grid_cell_base * left);

        grid_cell_base(grid_cell_base && fref) noexcept;

        const gcoords_t & position() const;

        const decltype(_connected) & connected() const;

        const decltype(_iconnected) & iconnected() const;

        const decltype(_cargo) & cargo() const;

        const decltype(_artifacts) & artifacts() const;

        const decltype(_no_artifacts) & no_artifacts() const;

        grid_cell_base * get_neighbor(direction_t dir) noexcept;

        const grid_cell_base * get_neighbor(direction_t dir) const noexcept;

        void set_neighbor(direction_t dir, grid_cell_base * ptr) noexcept;

        void unset_neighbor(direction_t dir) noexcept;

        grid_cell_base * get_connected(direction_t dir) noexcept;

        const grid_cell_base * get_connected(direction_t dir) const noexcept;

        grid_cell_base * get_cell(direction_t dir) noexcept;

        const grid_cell_base * get_cell(direction_t dir) const noexcept;

        void add_connection(direction_t use, grid_cell_base & cell);

        void change_connection_use(direction_t use, direction_t new_use);

        void remove_connection(direction_t use);

        void add_cargo(cargo_h num, artifact && arti);

        artifact remove_cargo(cargo_h num);

        void add_artifact(artifact_h num, artifact && arti);

        artifact remove_artifact(artifact_h num);

        void add_no_artifact(artifact_h num, artifact && arti);

        artifact remove_no_artifact(artifact_h num);

        void move_to(const dcoords_t& pos);

        void swap_with(grid_cell_base & rhs);

        bool_t is_placed() const;

        grid_cell_base & adopt(grid_cell_base && fref) noexcept;

        bool_t operator==(const grid_cell_base & rhs) const;

        using cell_base::operator=;

        grid_cell_base & operator=(grid_cell_base && fref) noexcept;

        ~grid_cell_base();

        friend ostream & operator<<(ostream & os, const grid_cell_base & cell);

        friend std::tuple<istream &, const std::map<part_h, part> &>
        operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv,
                   std::tuple<grid_cell_base &, std::queue<unresolved_connection> &> cell_conn);
    };

    ostream & operator<<(ostream & os, const grid_cell_base & cell);

    std::tuple<istream &, const std::map<part_h, part> &>
    operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv,
               std::tuple<grid_cell_base &, std::queue<unresolved_connection> &> cell_conn);
}

namespace std {

    template<>
    void swap<har::grid_cell_base>(har::grid_cell_base & a, har::grid_cell_base & b) noexcept;
}


#endif //HAR_GRID_CELL_BASE_HPP
