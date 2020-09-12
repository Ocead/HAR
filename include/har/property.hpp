//
// Created by Johannes on 10.06.2020.
//

#pragma once

#ifndef HAR_PROPERTY_HPP
#define HAR_PROPERTY_HPP

#include <har/value.hpp>

namespace har {

    class cell_base;

    class context;

    /// Provides read and write access to a single property of a cell in a simulation
    /// \brief Proxy for a property of a cell in a simulation
    class property final {
    private:
        context & _ctx; ///<Inner cell object
        cell_base & _cell; ///<Cell that holds the property
        const cell_cat _cat; ///<Category of the cell
        const of _id; ///<ID of the property
        const datatype _type; ///<Datatype of the property
        const bool_t _now;

    public:
        /// \brief Constructor
        /// \param [in,out] ctx Inner cell that connects to the logic of the automaton
        /// \param [in,out] cell Cell that holds the property
        /// \param [in] id ID of the property
        /// \param [in] type Datatype of the property
        /// \param [in] now
        property(context & ctx, cell_base & cell, cell_cat cat,
                 of id, datatype type, bool_t now = false);

        /// \brief Gets the supposed datatype of the property
        /// \return The datatype of the value currently held
        [[nodiscard]]
        datatype type() const;

        /// \brief Gets the underlying value of the property
        /// \return The underlying value
        [[nodiscard]]
        const value & val() const;

        /// \brief If the referenced value is not of type `har::special_t`,
        /// returns a copy of it in an instance of `har::special_t`.
        /// Returns the contained `har::special_t` otherwise
        /// \return contained value as `har::special_t`
        [[nodiscard]]
        special_t any() const;

        /// \brief Gets the value of the property
        /// \tparam T Desired type
        /// \return The value currently held
        template<typename T, std::enable_if_t<is_value_type<T>::value, T*> = nullptr>
        [[nodiscard]]
        inline const T & as() const {
            return get<T>(val());
        }

        inline bool_t operator==(const value & val) const {
            return this->val() == val;
        }

        inline bool_t operator!=(const value & val) const {
            return this->val() != val;
        }

        inline bool_t operator==(const value && val) const {
            return this->val() == val;
        }

        inline bool_t operator!=(const value && val) const {
            return this->val() != val;
        }

        inline bool_t operator==(const property & ref) const {
            return this->val() == ref.val();
        }

        inline bool_t operator!=(const property & ref) const {
            return this->val() != ref.val();
        }

        /// \brief Assigns a new value to the property
        /// \param [in] ref New value
        /// \return This property
        property & operator=(const value & ref);

        /// \brief Assigns a new value to the property
        /// \param [in] fref New value
        /// \return This property
        property & operator=(value && ref);

        ///  Assigns a new value to the property
        /// \param [in] ref New value
        /// \return This property
        property & operator=(const property & ref);

        /// \brief Assigns a new value to the property
        /// \param [in] ref New value
        /// \return This proxy
        template<typename T, std::enable_if_t<is_value_type<T>::value, T*> = nullptr>
        inline property & operator=(const T & ref) {
            return operator=(value(ref)); //NOLINT
        }

        /// \brief Assigns a new value to the property
        /// \param [in] ref New value
        /// \return This proxy
        template<typename T, std::enable_if_t<is_value_type<T>::value, T*> = nullptr>
        inline property & operator=(T && fref) {
            return operator=(value(fref)); //NOLINT
        }

#if !defined(__clang__)
        /// \brief Gets the value of the property
        /// \tparam T Desired type
        /// \return The value currently held
        template<typename T, std::enable_if_t<is_value_type<T>::value, T*> = nullptr>
        [[nodiscard]]
        inline explicit operator T() const {
            return get<T>(val());
        }
#endif

        /// \brief Gets the value of the property
        /// \tparam T Desired type
        /// \return The value currently held
        template<typename T, std::enable_if_t<is_value_type<T>::value, T*> = nullptr>
        [[nodiscard]]
        inline explicit operator const T &() const {
            return get<T>(val());
        }

        /// \brief Default destructor
        ~property();
    };

    template<typename T>
    inline void replace(property & prop, const T & val) {
        if (T(prop) != val) {
            prop = val;
        }
    }

    template<typename T>
    inline void replace(property prop, const T & val) {
        if (T(prop) != val) {
            prop = val;
        }
    }

}

#endif //HAR_PROPERTY_HPP
