//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_WORLD_HPP
#define HAR_WORLD_HPP

#include "world/grid.hpp"
#include "world/grid_cell_base.hpp"

namespace har {

    namespace exception {
        class out_of_grid : public std::exception {
        private:
            gcoords_t _req;
            gcoords_t _bnd;

        public:
            out_of_grid(std::string source, const gcoords_t & req, const gcoords & bnd);

            const gcoords_t & reqested() const;

            const gcoords_t & bounds() const;

            ~out_of_grid() override;
        };
    }

    class world {
    private:
        grid _model;
        grid _bank;

        map<cargo_h, cargo_cell_base> _cargo;

    public:
        world();

        world(const dcoords_t & model_size, const part & model_blank,
              const dcoords_t & bank_size, const part & bank_blank);

        world(dcoords_t && model_size, const part & model_blank,
              dcoords_t && bank_size, const part & bank_blank);

        world(const world & ref);

        world(world && fref) noexcept;

        grid & get_model();

        const grid & get_model() const;

        grid & get_bank();

        const grid & get_bank() const;

        decltype(_cargo) & cargo();

        const decltype(_cargo) & cargo() const;

        grid_cell_base & at(const gcoords_t & pos);

        const grid_cell_base & at(const gcoords_t & pos) const;

        cargo_cell_base & at(cargo_h num);

        const cargo_cell_base & at(cargo_h num) const;

        cell_base & at(const cell_h & hnd);

        const cell_base & at(const cell_h & hnd) const;

        void resize(grid_t grid, const part & pt, const dcoords_t & to);

        void resize(grid_t grid, const part & pt, dcoords_t && to);

        void minimize(grid_t grid);

        void swap_cells(grid_t grid, const dcoords_t & lhs, const dcoords_t & rhs);

        void swap_cells(grid_t grid, dcoords_t && lhs, dcoords_t && rhs);

        void swap_cells(const gcoords_t & lhs, const gcoords_t & rhs);

        void swap_cells(gcoords_t && lhs, gcoords_t && rhs);

        void swap_rectangles(const gcoords_t & tl_from, const dcoords_t & size, const gcoords_t & tl_to);

        void swap_rectangles(gcoords_t && tl_from, dcoords_t && size, gcoords_t && tl_to);

        void purge_part(const part & pt, const part & with);

        world & operator=(const world & ref);

        world & operator=(world && fref) noexcept;

        ~world();

        friend ostream & operator<<(ostream & os, const world & world);

        friend std::tuple<istream &, const std::map<part_h, part> &>
        operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, std::tuple<world &, bool_t &> world_ok);
    };

    std::ostream & operator<<(std::ostream & os, const world & world);

    std::tuple<std::istream &, const std::map<part_h, part> &>
    operator>>(std::tuple<std::istream &, const std::map<part_h, part> &> is_inv, std::tuple<world &, bool_t &> world_ok);

}


#endif //HAR_WORLD_HPP
