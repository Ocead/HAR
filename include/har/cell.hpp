//
// Created by Johannes on 05.07.2020.
//

#ifndef HAR_CELL_HPP
#define HAR_CELL_HPP

#include <har/cell_base.hpp>
#include <har/part.hpp>
#include <har/property.hpp>
#include <har/traits.hpp>
#include <har/value.hpp>

namespace har {

    class cargo_cell;

    class grid_cell;

    class context;

    class cargo_cell_base;

    class grid_cell_base;

    class part;

    /// \brief class for access to grid cell cell and cargo cells
    class cell {
    protected:
        context & _ctx; ///<Inner cell object
        cell_base & _cell; ///<The cell from which the cargo is accessed
        cell_cat _cat; ///<Category of the cell

    public:

        /// This constructor is only called by the automaton when a part or participant wants to operate on a base.
        /// \brief Creates a base over a base for parts and participants to operate upon
        /// \param [in,out] ctx Inner base that connects to the logic of the automaton
        /// \param [in] base The base to be accessed
        cell(context & ctx, cargo_cell_base & base);

        /// This constructor is only called by the automaton when a part or participant wants to operate on a base.
        /// \brief Creates a base over a base for parts and participants to operate upon
        /// \param [in,out] ctx Inner base that connects to the logic of the automaton
        /// \param [in] base The base to be accessed
        cell(context & ctx, grid_cell_base & base);

        [[nodiscard]]
        cargo_cell & as_cargo_cell();

        [[nodiscard]]
        const cargo_cell & as_cargo_cell() const;

        [[nodiscard]]
        grid_cell & as_grid_cell();

        [[nodiscard]]
        const grid_cell & as_grid_cell() const;

        /// \brief Gets the part definition of the cell
        /// \return The current part associated with this cell
        [[nodiscard]]
        const part & logic();

        /// 
        /// \return
        [[nodiscard]]
        cell_cat cat() const;

        [[nodiscard]]
        traits_h traits() const;

        /// \brief
        /// \param id
        /// \return
        [[nodiscard]]
        bool_t has(of id) const;

        /// Effectively calls `_cell.set` for each property in `ref`
        /// \brief Copies all properties of a `har::cell_base` into this cell
        /// \param [in] ref `har::cell_base` to copy from
        void adopt(const cell_base & ref);

        /// Effectively calls `_cell.set` for each property in `fref`
        /// \brief Moves all properties of a `har::cell_base` into this cell
        /// \param [in] ref `har::cell_base` to move from
        void adopt(cell_base && fref);

        /// \brief Sends a message to all participants of the simulation
        /// \param [in] header Header of the message
        /// \param [in] content Content of the message
        void message(const string_t & header, const string_t & content);

        /// \brief Access a property of the cell
        /// \param [in] id ID of the entry in the cell's property model
        /// \param [in] now
        /// \return A proxy object for the requested property
        [[nodiscard]]
        property get(of id, bool_t now = false);

        /// \brief Access a property of the cell
        /// \param [in] id ID of the entry in the cell's property model
        /// \return A proxy object for the requested property
        [[nodiscard]]
        property operator[](of id);

        /// \brief Access a property of the cell
        /// \param [in] id ID of the entry in the cell's property model
        /// \return A proxy object for the requested property
        [[nodiscard]]
        const property operator[](of id) const;

        /// \brief Default destructor
        virtual ~cell();
    };

}

#endif //HAR_CELL_HPP
