//
// Created by Johannes on 10.06.2020.
//

#pragma once

#ifndef HAR_GRID_HPP
#define HAR_GRID_HPP

#include <har/coords.hpp>

#include "world/grid_cell_base.hpp"

namespace har {
    class grid {
    private:
        grid_t _cat;
        dcoords_t _size;
        string_t _title;
        map<dcoords_t, grid_cell_base> _data;

        grid_cell_base create_cell(const part &, const dcoord_t & x, const dcoord_t & y);

    public:
        grid();

        grid(const gcoords_t & size, const part & blank_part);

        grid(gcoords_t && size, const part & blank_part);

        grid(grid && fref) noexcept;

        [[nodiscard]]
        grid_t cat() const;

        [[nodiscard]]
        grid_cell_base & at(const dcoords_t & pos);

        [[nodiscard]]
        const grid_cell_base & at(const dcoords_t & pos) const;

        [[nodiscard]]
        dcoords_t dim() const;

        [[nodiscard]]
        string_t & title();

        [[nodiscard]]
        const string_t & title() const;

        void insert_column(const part & pt);

        void insert_column(const part & pt, const dcoord_t & x);

        void insert_row(const part & pt);

        void insert_row(const part & pt, const dcoord_t & y);

        void remove_column();

        void remove_column(dcoord_t x);

        void remove_row();

        void remove_row(const dcoord_t & y);

        void resize_to(const part & pt, const dcoords_t & to);

        void resize_by(const part & pt, const dcoords_t & by);

        void minimize();

        void purge_part(const part & pt, const part & with);

        decltype(_data)::iterator begin();

        decltype(_data)::const_iterator begin() const;

        decltype(_data)::iterator end();

        decltype(_data)::const_iterator end() const;

        decltype(_data)::const_iterator cbegin() const;

        decltype(_data)::const_iterator cend() const;

        grid & operator=(grid && fref) noexcept;

        ~grid();
    };

}

#endif //HAR_GRID_HPP
