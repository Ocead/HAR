//
// Created by Johannes on 19.06.2020.
//

#pragma once

#ifndef HAR_CARGO_CELL_HPP
#define HAR_CARGO_CELL_HPP

#include <har/cell.hpp>
#include <har/coords.hpp>
#include <har/property.hpp>

namespace har {

    /// This class provides an interface to `part` and `participant` objects
    /// to read and modify cargo of a simulated model
    /// \brief A cargo cell in a simulation
    class cargo_cell : public cell {
    protected:
        [[nodiscard]]
        cargo_cell_base & as_cargo_cell_base() noexcept;

        [[nodiscard]]
        const cargo_cell_base & as_cargo_cell_base() const noexcept;

    protected:
        std::reference_wrapper<grid_cell_base> _base; ///<Cell base

    public:
        /// This constructor is only called by a cargo_base when a part or participant wants to operate on a cargo_base.
        /// \brief Creates a cargo over a cargo_base for parts and participants to operate upon
        /// \param [in,out] ctx Inner context that connects to the logic of the automaton
        /// \param [in] cargo_base The cargo to be accessed
        /// \param [in] cell_base A grid cell, that the cargo cell overlays
        explicit cargo_cell(context & ctx, cargo_cell_base & cargo_base, grid_cell_base & cell_base);

        /// \brief Returns the relative position of the cargo the the cell from which's cell this object was created
        /// \return The position of the cargo relative to the current cell
        [[nodiscard]]
        ccoords_t position() const;

        /// Makes the cargo be moved between the current and the next cycle
        /// \param [in] by Delta to move the cargo by
        void move(ccoords_t by);

        /// Makes the cargo be destroyed between the current and the next cycle
        void destroy();

        using cell::operator[];

        /// \brief Default destructor
        ~cargo_cell() override;
    };

}

#endif //HAR_CARGO_CELL_HPP
