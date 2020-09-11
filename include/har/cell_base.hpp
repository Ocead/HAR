//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_CELL_BASE_HPP
#define HAR_CELL_BASE_HPP

#include <map>
#include <sstream>

#include <har/exception.hpp>
#include <har/flags.hpp>
#include <har/part.hpp>
#include <har/value.hpp>

namespace har {

    class part;

    namespace exception {
        class cell_format_error : public exception {
        private:
            std::string _line;

        public:
            cell_format_error(const std::string & source, const std::string & line);

            ~cell_format_error() noexcept override;
        };
    }

    /// \brief Underlying data structure for cells in a simulation
    class cell_base {
    protected:
        using Map = map<of, value>;

        std::reference_wrapper<const part> _logic; ///<Currently assigned part
        Map _properties; ///<Map of properties
        Map _intermediate; ///<Map of temporary properties to be changed in a cycle

    public:
        static cell_base & invalid(); ///<Invalid cell_base

        /// \brief Constructor
        /// \param [in] pt
        explicit cell_base(const part & pt);

        /// \brief Copy constructor
        /// \param [in] ref
        cell_base(const cell_base & ref);

        /// \brief Move constructor
        /// \param [in,out] fref
        cell_base(cell_base && fref) noexcept;

        /// \brief Gets the part this cell is assigned to
        /// \return The part this cell is assigned to
        const part & logic() const;

        /// \brief Gets the properties of the cell
        /// \return The map of iproperties
        const Map & properties() const;

        /// \brief Gets the intermediate properties of the cell
        /// \return The map of intermediate properties
        const Map & intermediate() const;

        /// \brief Changes the part the cell is assigned to
        /// \param [in] pt New part
        void set_type(const part & pt);

        /// \brief Gets the value of a property
        /// \param [in] id ID of the property
        /// \param [in] now `TRUE`, if intermediate properties, if existent, should be heeded
        /// \return The requested value
        const value & get(of id, bool_t now = false) const;

        /// \brief Sets a value of a property intermediately
        /// \param [in] id ID of the property
        /// \param [in] val New value
        void set(of id, const value & val) noexcept;

        /// \brief Sets a value of a property intermediately
        /// \param [in] id ID of the property
        /// \param [in] val New value
        void set(of id, value && val) noexcept;

        /// \brief Discards all intermediate properties
        void rollback();

        /// \brief Removes all properties and intermediate properties from the cell
        void clear();

        /// \brief Adopts all intermediate properties
        void transit();

        /// \brief Adopts all properties of another cell
        /// \param [in] cell Another cell
        /// \return `TRUE`, if any properties were adopted
        bool_t adopt(const cell_base & cell);

        /// \brief Adopts all properties of another cell
        /// \param [in] cell Another cell
        /// \return `TRUE`, if any properties were adopted
        bool_t adopt(cell_base && cell);

        /// \brief Compares two cell for equality
        /// \param [in] rhs Another cell
        /// \return `TRUE`, if both cells are equal
        bool_t operator==(const cell_base & rhs) const;

        /// \brief Copy assignment
        /// \param [in] ref Reference to the original
        /// \return This cell
        cell_base & operator=(const cell_base & ref);

        /// \brief Move assignment
        /// \param [in, out] fref Forwarding reference to the original
        /// \return This cell
        cell_base & operator=(cell_base && fref) noexcept;

        /// \brief Default destructor
        ~cell_base() noexcept;

        friend ostream & operator<<(ostream & os, const cell_base & cell);

        friend std::tuple<istream &, const std::map<part_h, part> &>
        operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, cell_base & cell);
    };

    /// \brief Serializes a cell into an output stream
    /// \param [out] os Output stream
    /// \param [in] cell Cell to serialize
    /// \return The output stream
    ostream & operator<<(ostream & os, const cell_base & cell);

    /// \brief Deserializes a cell from an input stream
    /// \param [in] is_inv A tie of an input stream and the inventory of possible parts
    /// \param [out] cell Cell to deserialize into
    /// \return The tie
    std::tuple<istream &, const std::map<part_h, part> &>
    operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, cell_base & cell);
}

#endif //HAR_CELL_BASE_HPP
