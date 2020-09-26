//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_TRAITS_HPP
#define HAR_TRAITS_HPP

#include "types.hpp"

namespace har {

    ///
    enum traits : ushort_t {
        EMPTY_PART = 1u << 0u, ///<Denotes a part, that acts only as absence of other parts
        COMPONENT_PART = 1u << 1u, ///<Denotes a part for the model grid
        BOARD_PART = 1u << 2u, ///<Denotes a part for the bank grid
        CARGO_PART = 1u << 3u, ///<Denotes a part for use as cargo

        SENSOR = 1u << 4u, ///<Part that provides information

        INPUT = 1u << 5u, ///<
        OUTPUT = 1u << 6u, ///<

        SOLID = 1u << 7u, ///<Cargo can't move over this part
        MOVING = 1u << 8u, ///<Involved in moving cargo

        ORIENTABLE = 1u << 9u, ///<Part can rotate
        COLORED = 1u << 10u, ///<Part can be colored for better overview
    };


    /// \brief Bitwise ANDs two traits
    /// \param [in] lhs First traits
    /// \param [in] rhs Second traits
    /// \return ANDed traits
    inline constexpr traits operator&(traits lhs, traits rhs) {
        return traits(ushort_t(lhs) & ushort_t(rhs));
    }

    /// \brief Bitwise ORs two traits
    /// \param [in] lhs First traits
    /// \param [in] rhs Second traits
    /// \return ORed traits
    inline constexpr traits operator|(traits lhs, traits rhs) {
        return traits(ushort_t(lhs) | ushort_t(rhs));
    }

    /// \brief Bitwise NOTs traits
    /// \param [in] val traits
    /// \return Inverted traits
    inline constexpr traits operator~(traits val) {
        return traits(~ushort_t(val));
    }

    ///
    typedef enum traits traits_h;
}

#endif //HAR_TRAITS_HPP
