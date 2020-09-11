//
// Created by Johannes on 10.06.2020.
//

#pragma once

#ifndef HAR_GRID_CELL_HPP
#define HAR_GRID_CELL_HPP

#include <har/cargo_cell.hpp>
#include <har/cell.hpp>
#include <har/part.hpp>
#include <har/property.hpp>
#include <har/value.hpp>

namespace har {

    class cargo_iterable;

    class connection_iterable;

    /// This class provides an interface to `har::part` and `har::participant` objects
    /// to read and modify cells of a simulated model
    /// \brief Context over a grid's cell in a simulation
    class grid_cell : public cell {
    protected:
        direction_t _dir; ///<Direction to possibly neighbor

        ///
        /// \return
        [[nodiscard]]
        grid_cell_base & as_grid_cell_base() noexcept;

        ///
        /// \return
        [[nodiscard]]
        const grid_cell_base & as_grid_cell_base() const noexcept;

    public:
        /// This constructor is only called by the automaton when a part or participant wants to operate on a cell_base.
        /// \brief Creates a cell_base over a cell_base for parts and participants to operate upon
        /// \param [in,out] ctx Inner cell_base that connects to the logic of the automaton
        /// \param [in] cell_base The cell_base to be accessed
        /// \param [in] dir Direction to address a neighbor or connected cell_base
        grid_cell(context & ctx, grid_cell_base & cell_base, direction_t dir = direction::NONE);

        /// \brief Creates an iterable object over all connected cells
        /// for use in STL algorithms and for-each loops
        /// \return An iterable over all connected cells
        connection_iterable connected();

        /// This method creates a new cargo of specified type and position over the cell.
        /// The retured `har::cargo_cell` object is for the current cycle the only reference to the cargo,
        /// it won't appear in `har::cell::cargo` immediately.
        /// \brief Spawns new cargo over the cell
        /// \param [in] pt Part type of the new cargo
        /// \param [in] pos Relative position of the cargo on the cell ((0, 0) <= (x, y) < (1, 1))
        /// \return A cell for the created cargo
        cargo_cell spawn(const part & pt, ccoords_t pos = ccoords_t(0.5, 0.5));

        /// \brief Creates an iterable object over all cargo over the cell
        /// for use in STL algorithms and for-each loops
        /// \return An iterable over all cargo over the cell
        cargo_iterable cargo();

        /// \brief Accesses a specific cargo over the cell
        /// \param [in] num Index of the cargo
        /// \return A cell over the requested cargo
        cargo_cell cargo(cargo_h num);

        /// \brief Creates an iterable object over all cargo overlapping the cell
        /// for use in STL algorithms and for-each loops
        /// \return An iterable over all cargo overlapping the cell
        const cargo_iterable artifacts();

        /// \brief Accesses a specific cargo overlapping the cell
        /// \param [in] num Index of the cargo
        /// \return A cell over the requested cargo
        const cargo_cell artifacts(artifact_h num);

        /// \brief Checks if the cell is placed on a grid
        /// \return <tt>true</tt>, if the cell is placed on a grid
        [[nodiscard]]
        bool_t is_placed() const;

        /// \brief Access a neighboring or a connected cell
        /// \param [in] dir Direction of the cell
        /// \return A cell over the requested cell
        [[nodiscard]]
        const grid_cell operator[](direction_t dir);

        using cell::operator[];

        /// \brief Default destructor
        ~grid_cell() override;
    };

    ///
    struct connection {
        const direction_t use; ///<
        const grid_cell cell; ///<

        ///
        /// \param use
        /// \param inv_use
        /// \param ctx
        /// \param base
        /// \param cell_dir
        connection(direction_t use, context & ctx, grid_cell_base & base, direction_t cell_dir);

        /// \brief Default destructor
        ~connection() noexcept;
    };

    ///
    class connection_iterator {
    private:
        context & _ctx; ///<
        grid_cell_base & _gclb; ///<
        direction_t _dir; ///<
        std::optional<connection> _conn; ///<

    protected:

        ///
        /// \param cell
        /// \return
        connection & build_cell(grid_cell_base & cell);

    public:

        ///
        /// \param ctx
        /// \param cell
        /// \param dir
        connection_iterator(context & ctx, grid_cell_base & cell, direction_t dir);

        ///
        /// \return
        connection_iterator & operator++();

        ///
        /// \return
        const connection & operator*();

        ///
        /// \return
        const connection * operator->();

        ///
        /// \param rhs
        /// \return
        bool_t operator==(const connection_iterator & rhs) const;

        ///
        /// \param rhs
        /// \return
        bool_t operator!=(const connection_iterator & rhs) const;

        ///
        ~connection_iterator();
    };

    ///
    class connection_iterable {
    public:
        using value_type = connection;
        using reference = std::add_lvalue_reference_t<value_type>;
        using const_reference = std::add_const_t<reference>;
        using iterator = connection_iterator;
        using const_iterator = connection_iterator;
        using difference_type = direction_t;
        using size_type = size_t;

    private:
        context & _ctx; ///<
        grid_cell_base & _cell; ///<

    public:

        ///
        /// \param ctx
        /// \param cell
        connection_iterable(context & ctx, grid_cell_base & cell);

        ///
        /// \return
        [[nodiscard]]
        bool_t empty() const;

        ///
        /// \return
        [[nodiscard]]
        size_t size() const;

        ///
        /// \return
        [[nodiscard]]
        connection_iterator begin();

        ///
        /// \return
        [[nodiscard]]
        connection_iterator begin() const;

        ///
        /// \return
        [[nodiscard]]
        connection_iterator end();

        ///
        /// \return
        [[nodiscard]]
        connection_iterator end() const;

        /// \brief Default destructor
        ~connection_iterable();
    };

    struct overlay {
        cargo_h num;
        cargo_cell cell;

        ///
        /// \param ctx
        /// \param cargo_base
        /// \param cell_base
        /// \param num
        overlay(context & ctx, cargo_cell_base & cargo_base, grid_cell_base & cell_base, cargo_h num);

        ~overlay() noexcept;
    };

    /// Base class for iterator over cargo over a grid_cell
    class cargo_base_iterator {
    private:
        context & _ctx; ///<Inner context that connects to the logic of the automaton
        grid_cell_base & _gclb; ///<Grid cell, which's cargo are viewed
        cargo_cat _cat; ///<Whether the `har::cargo_cell`s centered over the grid cell are targeted or not
        size_t _pos; ///<The ordinal of the `har::cargo_cell` over the `har::grid_cell`, this iterator currently points to
        std::optional<overlay> _cell; ///<Storage for the `har::cell` over the `cargo_cell_base` pointed to

    protected:
        /// \brief Creates a cell object representing the cargo
        /// \return The `har::cargo_cell` over the `har::cargo_cell_base` pointed to
        overlay & build_cell();

    public:
        /// \brief Constructor
        /// \param [in,out] ctx Inner context that connects to the logic of the automaton
        /// \param [in] cell Grid cell, which's cargo are viewed
        /// \param [in] cat Whether the `har::cargo_cell`s centered over the grid cell are targeted or not
        /// \param [in] pos The ordinal of the `har::cargo_cell` over the `har::grid_cell`, this iterator currently points to
        cargo_base_iterator(context & ctx, grid_cell_base & cell, cargo_cat cat, size_t pos = size_t());

        /// \brief Lets the iterator point to the next element
        /// \return This iterator
        cargo_base_iterator & operator++();

        /// \brief Compares for equality with another iterator
        /// \param [in] rhs Other iterator
        /// \return `TRUE`, if both iterators point to the same `har::cargo_cell` from the same `har::grid_cell`
        bool_t operator==(const cargo_base_iterator & rhs) const;

        /// \brief Compares for inequality with another iterator
        /// \param [in] rhs Other iterator
        /// \return `TRUE`, if both iterators don't point to the same `har::cargo_cell` from the same `har::grid_cell`
        bool_t operator!=(const cargo_base_iterator & rhs) const;

        /// \brief Default destructor
        ~cargo_base_iterator();
    };

    /// \brief Cargo iterator for non-const selections
    class cargo_iterator : public cargo_base_iterator {
    public:
        using cargo_base_iterator::cargo_base_iterator;

        /// \Returns the `har::cargo_cell` currently pointed to per reference
        /// \return Reference to the `har::cargo_cell`currently pointed to
        overlay & operator*();

        /// \Returns the current pointer to a `har::cargo_cell`
        /// \return Current pointer to a `har::cargo_cell`
        overlay * operator->();

        /// \brief Default destructor
        ~cargo_iterator();
    };

    /// \brief Cargo iterator for const selection (e.g. from neighboring grid cells)
    class const_cargo_iterator : public cargo_base_iterator {
    public:
        using cargo_base_iterator::cargo_base_iterator;

        /// \Returns the `har::cargo_cell` currently pointed to per const reference
        /// \return Const reference to the `har::cargo_cell`currently pointed to
        const overlay & operator*();

        /// \Returns the current const pointer to a `har::cargo_cell`
        /// \return Current const pointer to a `har::cargo_cell`
        const overlay * operator->();

        /// \brief Default destructor
        ~const_cargo_iterator();
    };

    /// \brief Provides a Container-like interface to iterate over cargo visible to a grid cell
    class cargo_iterable {
    public:
        using value_type = cargo_cell;
        using reference = std::add_lvalue_reference_t<value_type>;
        using const_reference = std::add_const_t<reference>;
        using iterator = cargo_iterator;
        using const_iterator = const_cargo_iterator;
        using difference_type = cargo_h;
        using size_type = size_t;

    private:
        context & _ctx; ///<Inner context that connects to the logic of the automaton
        grid_cell_base & _cell; ///<Grid cell, which's cargo are viewed
        cargo_cat _cat; ///<Whether the cargo centered over the grid cell are targeted or not

    public:
        /// \brief Constructor
        /// \param [in,out] ctx Inner context that connects to the logic of the automaton
        /// \param [in] cell Grid cell, which's cargo are viewed
        /// \param [in] cat Whether the cargo centered over the grid cell are targeted or not
        cargo_iterable(context & ctx, grid_cell_base & cell, cargo_cat cat);

        /// \brief Checks, wheter the is cargo in the category provided at construction
        /// \return `TRUE`, if there is no cargo in the provided category
        [[nodiscard]]
        bool_t empty() const;

        /// \brief Checks, how many elements are in the category provided at construction
        /// \return the number of the elements in the provided category
        [[nodiscard]]
        size_t size() const;

        /// \brief \brief Provides a LegacyForwardIterator to the first cargo in the category
        /// \return An iterator pointing to the first cargo in the category
        [[nodiscard]]
        cargo_iterator begin();

        /// \brief \brief Provides a LegacyForwardIterator to the first const cargo in the category
        /// \return A const iterator pointing to the first cargo in the category
        [[nodiscard]]
        const_cargo_iterator begin() const;

        /// \brief \brief Provides an LegacyForwardIterator behind the last cargo in the category
        /// \return An iterator pointing behind the last cargo in the category
        [[nodiscard]]
        cargo_iterator end();

        /// \brief \brief Provides a LegacyForwardIterator begind the last cargo in the category
        /// \return A const iterator pointing behind the last cargo in the category
        [[nodiscard]]
        const_cargo_iterator end() const;

        /// \brief \brief Provides a LegacyForwardIterator to the first const cargo in the category
        /// \return A const iterator pointing to the first cargo in the category
        [[nodiscard]]
        const_cargo_iterator cbegin() const;

        /// \brief \brief Provides a const iterator behind the last const cargo in the category
        /// \return A const iterator pointing behind the last cargo in the category
        [[nodiscard]]
        const_cargo_iterator cend() const;

        /// \brief Default destructor
        ~cargo_iterable();
    };

}

#endif //HAR_GRID_CELL_HPP
