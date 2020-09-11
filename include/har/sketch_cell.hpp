//
// Created by Johannes on 19.07.2020.
//

#ifndef HAR_SKETCH_CELL_HPP
#define HAR_SKETCH_CELL_HPP

#include <har/cargo_cell.hpp>
#include <har/grid_cell.hpp>

namespace har {
    class sketch_cargo_cell : public cargo_cell {
    private:
        std::unique_ptr<cargo_cell_base> _cclb;

    public:
        explicit sketch_cargo_cell(const part & pt);

        void transit();

        ~sketch_cargo_cell() noexcept override;
    };

    class sketch_grid_cell : public grid_cell {
    private:
        std::unique_ptr<grid_cell_base> _gclb;

    public:
        explicit sketch_grid_cell(const part & pt);

        void add_connection(direction_t use, sketch_grid_cell & to);

        void remove_connection(direction_t use);

        void transit();

        ~sketch_grid_cell() noexcept override;
    };
}

#endif //HAR_SKETCH_CELL_HPP
