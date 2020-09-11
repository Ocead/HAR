//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_CARGO_CELL_BASE_HPP
#define HAR_CARGO_CELL_BASE_HPP

#include <map>

#include <har/coords.hpp>
#include <har/part.hpp>

#include "har/cell_base.hpp"

namespace har {

    class artifact;

    class grid_cell_base;

    class cargo_cell_base : public cell_base {
    private:
        cargo_h _id;
        ccoords_t _position;

        ccoords_t _size;
        ccoords_t _radius;
        ccoords_t _move_delta;
        bool_t _valid;

        std::map<dcoords_t, std::pair<artifact *, grid_cell_base *>> _overlays;

    public:
        static cargo_cell_base & invalid();

        explicit cargo_cell_base(cargo_h id, const part & part = part::invalid(), ccoords_t pos = ccoords_t(),
                                 ccoords_t size = ccoords_t(1.0f, 1.0f),
                                 ccoords_t radius = ccoords_t(0.1f, 0.1f));

        cargo_cell_base(cargo_h id, const cell_base & cell, ccoords_t pos = ccoords_t(),
                        ccoords_t size = ccoords_t(1.0f, 1.0f),
                        ccoords_t radius = ccoords_t(0.1f, 0.1f));

        cargo_cell_base(cargo_cell_base && fref) noexcept;

        [[nodiscard]]
        cargo_h id() const;

        [[nodiscard]]
        const ccoords_t & position() const;

        [[nodiscard]]
        const ccoords_t & size() const;

        [[nodiscard]]
        const ccoords_t & radius() const;

        [[nodiscard]]
        ccoords_t move_delta() const;

        void move_by(ccoords_t delta);

        void move_by(dcoords_t delta);

        bool_t operator==(const cargo_cell_base & rhs) const;

        cargo_cell_base & operator=(cargo_cell_base && fref) noexcept;

        ~cargo_cell_base();

        friend ostream & operator<<(ostream & os, const cargo_cell_base & ref);

        friend std::tuple<istream &, const std::map<part_h, part> &>
        operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, cargo_cell_base & ref);
    };

    ostream & operator<<(ostream & os, const cargo_cell_base & ref);

    std::tuple<istream &, const std::map<part_h, part> &>
    operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, cargo_cell_base & ref);
}


#endif //HAR_CARGO_CELL_BASE_HPP
