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

    /// \brief Class for access to grid cell cell and cargo cells
    class cell {
    protected:
        context & _ctx; ///<Inner cell object
        cell_base & _cell; ///<The cell from which the cargo is accessed
        cell_cat _cat; ///<Category of the cell

    public:

        /// \brief Creates a base over a base for parts and participants to operate upon
        /// \param [in,out] ctx Inner base that connects to the logic of the automaton
        /// \param [in] base The base to be accessed
        ///
        /// This constructor is only called by the automaton when a part or participant wants to operate on a base.
        cell(context & ctx, cargo_cell_base & base);

        /// \brief Creates a base over a base for parts and participants to operate upon
        /// \param [in,out] ctx Inner base that connects to the logic of the automaton
        /// \param [in] base The base to be accessed
        ///
        /// This constructor is only called by the automaton when a part or participant wants to operate on a base.
        cell(context & ctx, grid_cell_base & base);

        cell(const cell & ref) = delete;

        /// \brief Casts this cell into a cargo cell
        /// \return This cell as a cargo cell
        [[nodiscard]]
        cargo_cell & as_cargo_cell();

        /// \brief Casts this cell into a grid cell
        /// \return This cell as a grid cell
        [[nodiscard]]
        grid_cell & as_grid_cell();

        /// \brief Gets the part definition of the cell
        /// \return The current part associated with this cell
        [[nodiscard]]
        const part & logic();

        /// \brief Checks for the category of this cell.
        /// \return The category of this cell
        ///
        /// This check should precede a cast of this cell, when in doubt
        [[nodiscard]]
        cell_cat cat() const;

        /// \brief Gets the traits of the part this cell is an instance of
        /// \return The traits of this cells part definition
        [[nodiscard]]
        traits_h traits() const;

        /// \brief Checks if a cell has a certain property
        /// \param id ID of the property in question
        /// \return <tt>TRUE</tt>, if the cell has this property
        [[nodiscard]]
        bool_t has(of id) const;

        /// \brief Copies all properties of a <tt>har::cell_base</tt> into this cell
        /// \param [in] ref <tt>har::cell_base</tt> to copy from
        ///
        /// Effectively calls <tt>this->set</tt> for each property in <tt>ref</tt>
        void adopt(const cell_base & ref);

        /// \brief Moves all properties of a <tt>har::cell_base</tt> into this cell
        /// \param [in] fref <tt>har::cell_base</tt> to move from
        ///
        /// Effectively calls <tt>this->set</tt> for each property in <tt>fref</tt>
        void adopt(cell_base && fref);

        /// \brief Sends a message to all participants of the simulation
        /// \param [in] header Header of the message
        /// \param [in] content Content of the message
        void message(const string_t & header, const string_t & content);

        void redraw();

        /// \brief Access a property of the cell
        /// \param [in] id ID of the entry in the cell's property model
        /// \param [in] now
        /// \return A proxy object for the requested property
        [[nodiscard]]
        property get(of id, bool_t now = false);

        /// \brief Checks if the cell is placed on a grid
        /// \return <tt>true</tt>, if the cell is placed on a grid
        [[nodiscard]]
        bool_t is_placed() const;

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

        cell & operator=(const cell & ref) = delete;

        /// \brief Default destructor
        virtual ~cell();
    };

}

#endif //HAR_CELL_HPP
