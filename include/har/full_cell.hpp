//
// Created by Johannes on 12.06.2020.
//

#pragma once

#ifndef HAR_FULL_CELL_HPP
#define HAR_FULL_CELL_HPP

#include <har/grid_cell.hpp>

namespace har {

    /// \brief Cargo cell with additional capabilities for use by participants
    class full_cargo_cell : public cargo_cell {
    public:

        ///
        /// \param ctx
        /// \param cell
        full_cargo_cell(context & ctx, cargo_cell_base & cell);

        ///
        /// \return
        [[nodiscard]]
        const ccoords_t & absolute_position() const;

        ///
        /// \param pt
        void set_part(const part & pt);

        ///
        /// \param ccl
        void swap_with(cargo_cell & ccl);

        ///
        /// \return
        [[nodiscard]]
        string_t to_string() const;

        /// \brief Default destructor
        ~full_cargo_cell() override;
    };

    ///
    class full_grid_cell : public grid_cell {
    public:

        ///
        /// \param ctx
        /// \param cell
        /// \param dir
        full_grid_cell(context & ctx, grid_cell_base & cell, direction_t dir = direction::NONE);

        ///
        /// \return
        [[nodiscard]]
        const gcoords_t & position() const;

        ///
        /// \param pt
        void set_part(const part & pt);

        ///
        /// \param use
        /// \return
        bool_t has_connection(direction_t use);

        ///
        /// \param use
        /// \param fgcl
        void add_connection(direction_t use, full_grid_cell & fgcl);

        ///
        /// \param use
        /// \param fgcl
        void add_connection(direction_t use, full_grid_cell && fgcl);

        ///
        /// \param use
        void remove_connection(direction_t use);

        /// \brief Removes all connections of the cell
        void remove_all_connections();

        ///
        /// \param fgcl
        void swap_with(full_grid_cell & fgcl);

        ///
        /// \param fgcl
        void swap_with(full_grid_cell && fgcl);

        /// \brief Serializes the underlying <tt>har::cell_base</tt> or derivative thereof into a string
        /// \return Serialized <tt>har::cell_base</tt>
        [[nodiscard]]
        string_t to_string() const;

        using grid_cell::operator[];

        full_grid_cell operator[](direction_t);

        /// \brief Default destructor
        ~full_grid_cell() override;
    };

}

#endif //HAR_FULL_CELL_HPP
