//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_COORDS_HPP
#define HAR_COORDS_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <variant>

#include <har/exception.hpp>
#include <har/types.hpp>

namespace har {

    /// For your convenience, please use the `har::direction` struct instead of this enum
    /// when writing code that utilizes directions.
    /// \brief Encodes directions from one cell to another
    /// \sa har::direction
    enum struct raw_direction : int_t {
        ///In upward direction, corresponds to relative coordinates [0, -1]
        UP = std::numeric_limits<int_t>::min() + 0,
        ///In downward direction, corresponds to relative coordinates [0, 1]
        DOWN = std::numeric_limits<int_t>::min() + 1,
        ///In rightward direction, corresponds to relative coordinates [1, 0]
        RIGHT = std::numeric_limits<int_t>::min() + 2,
        ///In leftward direction, corresponds to relative coordinates [-1, 0]
        LEFT = std::numeric_limits<int_t>::min() + 3,
        ///In no direction, corresponds to relative coordinates [0, 0]
        NONE = std::numeric_limits<int_t>::min() + 4,
        ///In direction of the `n`th connected cell, where `n` is the numeric value of this enum
        PIN = 0
    };

    /// \brief Defines the raw type to be used for encoding directions
    /// \sa har::raw_direction
    typedef raw_direction direction_t;

    enum cell_cat : char_t {
        INVALID_CELL = 0,
        GRID_CELL = 1,
        CARGO_CELL = 2
    };

    /// \brief A filter type, to specify cell dependant valid and invalid directions
    enum dir_cat : char_t {
        ALL_DIRECTIONS = 0, ///<Allow all possible directions
        CARDINAL_DIRECTIONS = 1, ///<Allow cardinal directions only
        PIN_DIRECTIONS = 2, ///<Allow pin directions only
        CONNECTED_ONLY = 3 ///<Allow all valid directions
    };

    enum struct raw_cargo : uint_t {
        CARGO = 0u
    };

    typedef raw_cargo cargo_h;

    enum struct raw_artifact : uint_t {
        ARTIFACT = 0u
    };

    typedef raw_cargo artifact_h;

    /// This struct is used to encode the spatial relations between cells in raw values.<br/>
    /// Cells on the same grid can address their adjacent neighbors with cardinal directions
    /// `UP`, `DOWN`, `RIGHT` and `LEFT`.
    /// Cells that share a connection between each other can be addressed via `PIN` and indexes on it.
    /// The implicit direction `NONE` was added to streamline the access of cell properties,
    /// regardless of whether a cell's neighbor's properties or it's own properties are targeted.<br/>
    /// \brief Helper struct for writing code using the `direction_t` type
    struct direction final {
    public:
        static constexpr direction_t UP = direction_t::UP; ///<In upward direction, corresponds to relative coordinates [0, -1]
        static constexpr direction_t DOWN = direction_t::DOWN; ///<In downward direction, corresponds to relative coordinates [0, 1]
        static constexpr direction_t RIGHT = direction_t::RIGHT; ///<In rightward direction, corresponds to relative coordinates [1, 0]
        static constexpr direction_t LEFT = direction_t::LEFT; ///<In leftward direction, corresponds to relative coordinates [-1, 0]
        static constexpr direction_t NONE = direction_t::NONE; ///<In no direction, corresponds to relative coordinates [0, 0]


        static constexpr struct {
            ///In direction of a cell connected to this one
            /// \param [in] n Ordinal value of the cell to be addressed, must be >= 0
            /// \return The direction of the cell, specified through `n`
            constexpr direction_t operator[](const int_t n) const {
                if (n < 0)
                    raise(std::logic_error("n must be greater than or equal zero"));
                return static_cast<direction_t>(int_t(direction_t::PIN) + n);
            }

            /// \brief Implicit cast
            /// \return A direction equivalent to `PIN[0]`
            constexpr operator direction_t() const { //NOLINT
                return direction::PIN[0];
            }
        } PIN = { };

        /// \brief Returns an array that contains the four cardinal directions in clockwise order.
        /// Intended to be used as base for iterators.
        /// \return An array of cardinal directions
        static constexpr std::array<direction_t, 4> cardinal{ direction::UP,
                                                              direction::RIGHT,
                                                              direction::DOWN,
                                                              direction::LEFT };

    };

    using PIN = decltype(direction::PIN);

    static constexpr struct {

        constexpr cargo_h operator[](const int_t n) const {
            return cargo_h(int_t(cargo_h::CARGO) + n);
        }

        constexpr operator cargo_h() const { //NOLINT
            return operator[](0);
        }
    } CARGO = { };

    static constexpr struct {

        constexpr cargo_h operator[](const int_t n) const {
            return cargo_h(int_t(artifact_h::CARGO) + n);
        }

        constexpr operator artifact_h() const { //NOLINT
            return operator[](0);
        }
    } ARTIFACT = { };

    enum class cargo_cat {
        CARGO,
        ARTIFACT,
        NO_ARTIFACT
    };

    namespace exception {
        ///\brief Exception on use of an in-cell invalid use of a direction
        class invalid_direction : public exception {
        private:
            direction_t _actual; ///<The faulty direction
        public:
            /// Constructs a new exception
            /// \param source Source of the exception
            /// \param dir Faulty direction
            explicit invalid_direction(std::string source, direction_t dir);

            [[nodiscard]]
            const char * what() const noexcept override;
        };
    }

    /// Inverts a cardinal direction, does nothing otherwise
    /// \param [in] d A direction
    /// \return The inverse of `d`, if `d` is a cardinal direction
    static constexpr direction_t operator!(const direction_t & d) {
        switch (d) {
            case direction::UP:
                return direction::DOWN;
            case direction::DOWN:
                return direction::UP;
            case direction::RIGHT:
                return direction::LEFT;
            case direction::LEFT:
                return direction::RIGHT;
            default:
                return d;
        }
    }

    /// \brief Rotates a cardinal direction by 90° clockwise
    /// \param [in] d A cardinal direction
    /// \return `d`, rotated 90° clockwise
    /// \throws invalid_direction if `d` is not a cardinal direction
    static constexpr direction_t cw(direction_t d) {
        switch (d) {
            case direction::UP:
                return direction::RIGHT;
            case direction::RIGHT:
                return direction::DOWN;
            case direction::DOWN:
                return direction::LEFT;
            case direction::LEFT:
                return direction::UP;
            default:
                raise(exception::invalid_direction("direction::cw", d));
        }
    }

    /// \brief Rotates a cardinal direction by 90° counter-clockwise
    /// \param [in] d A cardinal direction
    /// \return `d`, rotated 90° counter-clockwise
    /// \throws invalid_direction if `d` is not a cardinal direction
    static constexpr direction_t ccw(direction_t d) {
        switch (d) {
            case direction::UP:
                return direction::LEFT;
            case direction::RIGHT:
                return direction::UP;
            case direction::DOWN:
                return direction::RIGHT;
            case direction::LEFT:
                return direction::DOWN;
            default:
                raise(std::exception());
        }
    }

    /// \brief Serializes a direction into a string
    /// \param [in,out] os The output stream
    /// \param [in] d A direction
    /// \return The output stream
    ostream & operator<<(ostream & os, const har::direction_t & d);

    /// \brief Deserializes a direction from a string
    /// \param [in,out] os The input stream
    /// \param [out] d A direction
    /// \return The input stream
    istream & operator>>(istream & is, har::direction_t & d);

    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type *>
    struct coords;

    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type * = nullptr>
    struct coord {
        T v;

        constexpr coord() noexcept: v() {

        }

        template<typename U>
        constexpr coord(const U val = 0) : v(T(val)) {

        };

        constexpr coord(const coord & ref) = default;

        constexpr coord(coord && fref) noexcept = default;

        inline constexpr coord operator+() const noexcept {
            return coord(+v);
        }

        template<typename U>
        inline constexpr coord operator+(const U & add) const noexcept {
            return coord(v + add);
        }

        inline constexpr coord operator+(const coord & add) const noexcept {
            return coord(v + add.v);
        }

        inline constexpr coord operator-() const noexcept {
            return coord(-v);
        }

        template<typename U>
        inline constexpr coord operator-(const U & sub) const noexcept {
            return coord(v - sub);
        }

        inline constexpr coord operator-(const coord & sub) const noexcept {
            return coord(v - sub.v);
        }

        template<typename U>
        inline constexpr coord operator*(const U & mul) const noexcept {
            return coord(v * mul);
        }

        inline constexpr coord operator*(const coord & mul) const noexcept {
            return coord(v * mul.v);
        }

        template<typename U>
        inline constexpr coord operator/(const U & div) const noexcept {
            return coord(v / div);
        }

        inline constexpr coord operator/(const coord & div) const noexcept {
            return coord(v / div.v);
        }

        template<typename U>
        inline constexpr coord operator%(const U & mod) const noexcept {
            if constexpr (std::is_integral_v<U>) {
                return coord(v % mod);
            } else {
                return std::fmod(v, mod);
            }
        }

        inline constexpr coord operator%(const coord & mod) const noexcept {
            if constexpr (std::is_integral_v<T>) {
                return coord(v % mod.v);
            } else {
                return std::fmod(v, mod.v);
            }
        }

        inline constexpr coord & operator+=(const coord & rhs) noexcept {
            v += rhs.v;
            return *this;
        }

        inline constexpr coord & operator+=(const T & add) noexcept {
            v += add;
            return *this;
        }

        inline constexpr coord & operator-=(const coord & rhs) noexcept {
            v -= (rhs.v <= v) ? rhs.v : v;
            return *this;
        }

        inline constexpr coord & operator-=(const T & sub) noexcept {
            v -= (sub <= v) ? sub : v;
            return *this;
        }

        inline constexpr coord & operator*=(const coord & rhs) noexcept {
            v *= rhs.v;
            return *this;
        }

        inline constexpr coord & operator*=(const T & mul) noexcept {
            v *= mul;
            return *this;
        }

        inline constexpr coord & operator/=(const coord & rhs) noexcept {
            v /= rhs.v;
            return *this;
        }

        inline constexpr coord & operator/=(const T & div) noexcept {
            v /= div;
            return *this;
        }

        inline constexpr coord & operator%=(const coord & mod) noexcept {
            v %= mod.v;
            return *this;
        }

        inline constexpr coord & operator%=(const T & mod) noexcept {
            v %= mod;
            return *this;
        }

        inline constexpr coord & operator++() noexcept {
            v++;
            return *this;
        }

        inline constexpr coord & operator--() noexcept {
            v--;
            return *this;
        }

        inline constexpr const coord & operator++(int) noexcept {
            v++;
            return *this;
        }

        inline constexpr const coord & operator--(int) noexcept {
            v--;
            return *this;
        }

        template<typename U>
        inline constexpr bool_t operator<(const U & rhs) const noexcept {
            return v < rhs;
        }

        inline constexpr bool_t operator<(const T & rhs) const noexcept {
            return v < rhs;
        }

        template<typename U>
        inline constexpr bool_t operator>(const U & rhs) const noexcept {
            return v > rhs;
        }

        inline constexpr bool_t operator>(const T & rhs) const noexcept {
            return rhs < v;
        }

        template<typename U>
        inline constexpr bool_t operator<=(const U & rhs) const noexcept {
            return !(v > rhs);
        }

        inline constexpr bool_t operator<=(const T & rhs) const noexcept {
            return !(rhs < v);
        }

        template<typename U>
        inline constexpr bool_t operator>=(const U & rhs) const noexcept {
            return !(v < rhs);
        }

        inline constexpr bool_t operator>=(const T & rhs) const noexcept {
            return !(v < rhs);
        }

        template<bool_t i = std::is_integral<T>::value>
        inline constexpr bool_t operator==(const T & rhs) const noexcept {
            if (i)
                return v == rhs;
            else
                return (std::fabs(v - rhs) < std::numeric_limits<T>::epsilon());
        }

        inline constexpr bool_t operator!=(const T & rhs) const noexcept {
            return !(v == rhs);
        }

        inline constexpr bool_t operator<(const coord & rhs) const noexcept {
            return v < rhs.v;
        }

        inline constexpr bool_t operator>(const coord & rhs) const noexcept {
            return rhs.v < v;
        }

        inline constexpr bool_t operator<=(const coord & rhs) const noexcept {
            return !(rhs.v < v);
        }

        inline constexpr bool_t operator>=(const coord & rhs) const noexcept {
            return !(v < rhs.v);
        }

        template<bool_t i = std::is_integral<T>::value>
        inline constexpr bool_t operator==(const coord & rhs) const noexcept {
            if (i)
                return v == rhs.v;
            else
                return (std::fabs(v - rhs.v) < std::numeric_limits<T>::epsilon());
        }

        inline constexpr bool_t operator!=(const coord & rhs) const noexcept {
            return !(v == rhs.v);
        }

        /// Concatenates this coordinate with another coordinate to a two-dimensional coordinate
        /// \param sec Other coordinate
        /// \return Two-dimensional coordinate with `[*this, sec]`
        inline constexpr coords<T, nullptr> operator,(const coord & sec) const noexcept {
            return coords(v, sec.v);
        }

        friend ostream & operator<<(ostream & os, const coord & c) {
            os << c.v;
            return os;
        }

        friend istream & operator>>(istream & is, coord & c) {
            is >> c.v;
            return is;
        }

        inline constexpr coord & operator=(const T ref) noexcept {
            v = ref;
            return *this;
        }

        inline constexpr coord & operator=(const coord & ref) = default;

        inline constexpr coord & operator=(coord && fref) noexcept = default;

        constexpr operator T() const {
            return v;
        }

        ~coord() = default;
    };

    /// \brief Represents two-dimensional coordinates of variable underlying type
    /// \tparam T to be used as mathematical field for each component of the coordinate. Must fulfill `std::is_arithmetic`
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr>
    struct coords {
        using dim = T;

        coord<T> x; ///<X component
        coord<T> y; ///<Y component

        /// \brief Constructs a new coordinate and initializes it with x = y = T() as the standard value
        coords() noexcept: x(T()), y(T()) { };

        /// \brief Constructs a new coordinate and initializes it with with explicit values
        /// \param [in] x X component
        /// \param [in] y Y component
        constexpr coords(const T x, const T y) : x(x), y(y) { }

        /// \brief Default copy constructor
        constexpr coords(const coords & ref) = default;

        /// \brief Default move constructor
        constexpr coords(coords && fref) noexcept = default;

        /// Constructs a new coordinate from a cardinal direction
        /// \param d A cardinal direction
        /// \throws invalid_direction if `d` is not cardinal
        constexpr explicit coords(direction_t d) {
            switch (d) {
                case direction_t::UP:
                    x = 0;
                    y = -1;
                    break;
                case direction_t::RIGHT:
                    x = 1;
                    y = 0;
                    break;
                case direction_t::DOWN:
                    x = 0;
                    y = 1;
                    break;
                case direction_t::LEFT:
                    x = -1;
                    y = 0;
                    break;
                default:
                    raise(exception::invalid_direction("ctor har::coords", d));
            }
        }

        /// \brief
        /// \return
        inline constexpr coords operator+() const noexcept {
            return coords(+x, +y);
        }

        /// \brief Adds a coordinate and a constant
        /// \tparam U Type of the constant
        /// \param [in] add Constant to add
        /// \return The sum coordinate
        template<typename U>
        inline constexpr coords operator+(const U & add) const noexcept {
            return coords(x + T(add), y + T(add));
        }

        /// \brief Adds two coordinates
        /// \tparam U Template type of the coordinate
        /// \param [in] add Coordinate to add
        /// \return The sum coordinate
        template<typename U>
        inline constexpr coords operator+(const coords<U> & add) const noexcept {
            return coords(x + T(add.x), y + T(add.y));
        }

        /// \brief Inverts the coordinate
        /// \return The inverse of the coordinate

        /// \return The inverse of the coordinate

        inline constexpr coords operator-() const noexcept {
            return coords(-x, -y);
        }

        /// \brief Subtracts a constant from a coordinate
        /// \tparam U Type of the constant
        /// \param [in] sub Constant to subtract
        /// \return The difference coordinate
        template<typename U>
        inline constexpr coords operator-(const U & sub) const noexcept {
            return coords(x - T(sub), y - T(sub));
        }

        /// \brief Subtracts a coordinate from a coordinate
        /// \tparam U Template type of the coordinate
        /// \param [in] sub Coordinate to add
        /// \return The difference coordinate
        template<typename U>
        inline constexpr coords operator-(const coords<U> & sub) const noexcept {
            return coords(x - T(sub.x), y - T(sub.y));
        }

        /// \brief Multiplies a coordinate with a constant
        /// \tparam U Type of the constant
        /// \param [in] mul Constant to multiply with
        /// \return The product coordinate
        template<typename U>
        inline constexpr coords operator*(const U & mul) const noexcept {
            return coords(x * T(mul), y * T(mul));
        }

        /// \brief Multiplies a coordinate with another coordinate
        /// \tparam U Template type of the coordinate
        /// \param [in] mul Coordinate to multiply with
        /// \return The product coordinate
        template<typename U>
        inline constexpr coords operator*(const coords<U> & mul) const noexcept {
            return coords(x * T(mul.x), y * T(mul.y));
        }

        /// \brief Divides a coordinate by a constant
        /// \tparam U Type of the constant
        /// \param [in] div Constant to divide by
        /// \return The quotient coordinate
        template<typename U>
        inline constexpr coords operator/(const U & div) const noexcept {
            return coords(x / T(div), y / T(div));
        }

        /// \brief Divides a coordinate by another coordinate
        /// \tparam U Template type of the coordinate
        /// \param [in] div Coordinate to divide by
        /// \return The quotient coordinate
        template<typename U>
        inline constexpr coords operator/(const coords<U> & div) const noexcept {
            return coords(x / T(div.x), y / T(div.y));
        }

        /// \brief Calculates the remainder of a coordinate over a constant
        /// \tparam U Type of the constant
        /// \param [in] mod Constant to divide by
        /// \return The modulus coordinate
        template<typename U>
        inline constexpr coords operator%(const U & mod) const noexcept {
            return coords(x % T(mod), y % T(mod));
        }

        /// \brief Calculates the remainder of a coordinate over another coordinate
        /// \tparam U Template type of the constant
        /// \param [in] mod Coordinate to divide by
        /// \return The modulus coordinate
        template<typename U>
        inline constexpr coords operator%(const coords<U> & mod) const noexcept {
            return coords(x % T(mod.x), y % T(mod.y));
        }

        template<typename U>
        inline constexpr coords & operator+=(const coords<U> & add) noexcept {
            x += T(add.x);
            y += T(add.y);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator+=(const U & add) noexcept {
            x += T(add);
            y += T(add);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator-=(const coords<U> & sub) noexcept {
            x -= T(sub.x);
            y -= T(sub.y);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator-=(const U & sub) noexcept {
            x -= T(sub);
            y -= T(sub);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator*=(const coords<U> & mul) noexcept {
            x *= T(mul.x);
            y *= T(mul.y);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator*=(const U & mul) noexcept {
            x *= T(mul);
            y *= T(mul);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator/=(const coords<U> & div) noexcept {
            x /= T(div.x);
            y /= T(div.y);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator/=(const U & div) noexcept {
            x /= T(div);
            y /= T(div);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator%=(const coords<U> & mod) noexcept {
            x %= T(mod.x);
            y %= T(mod.y);
            return *this;
        }

        template<typename U>
        inline constexpr coords & operator%=(const U & mod) noexcept {
            x %= T(mod);
            y %= T(mod);
            return *this;
        }

        inline constexpr bool_t operator!() const noexcept {
            return !x || !y;
        }

        template<typename U>
        inline constexpr bool_t operator==(const coords<U> & ref) const noexcept {
            return (x == T(ref.x) && (y == T(ref.y)));
        }

        template<typename U>
        inline constexpr bool_t operator!=(const coords<U> & ref) const noexcept {
            return !(*this == ref);
        }

        template<typename U>
        inline constexpr bool_t operator<(const coords<U> & ref) const noexcept {
            return (x == T(ref.x)) ? (y < T(ref.y)) : (x < T(ref.x));
        }

        template<typename U>
        inline constexpr bool_t operator>(const coords<U> & ref) const noexcept {
            return (x == T(ref.x)) ? (y > T(ref.y)) : (x > T(ref.x));
        }

        template<typename U>
        inline constexpr bool_t operator<=(const coords<U> & ref) const noexcept {
            return (x == T(ref.x)) ? (y <= T(ref.y)) : (x <= T(ref.x));
        }

        template<typename U>
        inline constexpr bool_t operator>=(const coords<U> & ref) const noexcept {
            return (x == T(ref.x)) ? (y >= T(ref.y)) : (x >= T(ref.x));
        }

        inline friend ostream & operator<<(ostream & os, const coords & c) {
            os << "[" << c.x << ", " << c.y << "]";
            return os;
        }

        friend istream & operator>>(istream & is, coords & c) {
            char_t b[2];
            is.readsome(b, 1);
            if (b[0] != '[')
                raise(std::invalid_argument(std::string(1, b[0])));
            is >> c.x;
            is.readsome(b, 1);
            if (b[0] != ',')
                raise(std::invalid_argument(std::string(1, b[0])));
            is >> std::ws;
            is >> c.y;
            is.readsome(b, 1);
            if (b[0] != ']')
                raise(std::invalid_argument(std::string(1, b[0])));
            return is;
        }

        template<typename U>
        explicit constexpr operator coords<U>() const {
            return coords<U>(U(x), U(y));
        }

        explicit constexpr operator bool() const {
            return bool(x) || bool(y);
        }

        /// Sets this coordinate to the next integral position in a rectangle from including `tl` to not including `br`.
        /// The coordinate moves line by line from left to right, then from top to bottom.<br/>
        /// When finished, the coordinate is set to `br`.<br/>
        /// If the coordinate isn't in the rectangle to begin with, it is set to `tl`.
        /// \brief Iterates a coordinate over a rectangle
        /// \param [in] tl Top-left corner of the rectangle (included)
        /// \param [in] br Bottom-right corner of the rectangle (not included)
        /// \return The moved coordinate
        coords & rectangle(const coords & tl, const coords & br) {
            if (y <= br.y) {
                if (x < br.x - 1) {
                    x++;
                    return *this;
                } else {
                    if (y < br.y - 1) {
                        x = tl.x;
                        y++;
                        return *this;
                    } else
                        return (*this = br);
                }
            }
            return (*this = tl);
        }

        /// \brief Checks whether the coordinate is in a rectangle from [0, 0] to `br`
        /// \param [in] br Bottom-right corner of the rectangle (not included)
        /// \return `true`, if the coordinate is in the rectangle, `false` otherwise
        [[nodiscard]]
        constexpr bool_t in(const coords & br) const {
            return (x >= 0 && x < br.x &&
                    y >= 0 && y < br.y) || (x <= 0 && x > br.x &&
                                            y <= 0 && y > br.y);
        }

        /// \brief Checks whether the coordinate is in a rectangle from `tl` to `br`
        /// \param [in] tl Top-left corner of the rectangle (included)
        /// \param [in] br Bottom-right corner of the rectangle (not included)
        /// \return `true`, if the coordinate is in the rectangle, `false` otherwise
        [[nodiscard]]
        constexpr bool_t in(const coords & tl, const coords & br) const {
            return (x >= tl.x && x < br.x &&
                    y >= tl.y && y < br.y);
        }

        [[nodiscard]]
        constexpr uint_t size() const {
            return x * y;
        }

        [[nodiscard]]
        constexpr bool_t has_nth(uint_t n) const {
            return n / x < y;
        }

        [[nodiscard]]
        constexpr coords nth_of(uint_t n) const {
            return coords(n % x, n / x);
        }

        /// \brief Default copy assignment
        constexpr coords & operator=(const coords & ref) = default;

        /// \brief Default move assignment
        constexpr coords & operator=(coords && fref) noexcept = default;

        constexpr explicit operator std::pair<coord<T>, coord<T>>() const {
            return std::make_pair(x, y);
        }

        constexpr explicit operator std::pair<coord<T> &, coord<T> &>() const {
            return std::tie(x, y);
        }

        inline static constexpr coords clamp(const coords & val, const coords & lo, const coords & hi) {
            return coords((lo.x.v < hi.x.v) ?
                          std::clamp(val.x.v, lo.x.v, hi.x.v) :
                          std::clamp(val.x.v, hi.x.v, lo.x.v), (lo.y.v < hi.y.v) ?
                                                               std::clamp(val.y.v, lo.y.v, hi.y.v) :
                                                               std::clamp(val.y.v, hi.y.v, lo.y.v));
        }

        /// \brief Default destructor
        ~coords() = default;
    };

    /// \brief Specifies a type of a grid
    enum grid_t : int_t {
        MODEL_GRID = 1, ///<A grid with index > 0 for placement of parts
        BANK_GRID = -1, ///<A grid with index < 0 for placement of pins
        INVALID_GRID = 0 ///<Neither a grid for parts nor pins
    };

    /// One-dimensional signed integral coordinate
    typedef har::coord<int_t> dcoord_t;
    /// One-dimensional signed floating point coordinate
    typedef har::coord<double_t> ccoord_t;
    /// Two-dimensional signed integral coordinate
    typedef har::coords<int_t> dcoords_t;
    /// Two-dimensional signed floating point coordinate
    typedef har::coords<double_t> ccoords_t;

    /// \brief Global coordinate
    struct gcoords {
        grid_t cat; ///<The category of the grid
        dcoords_t pos; ///<The position on the grid

        gcoords() : cat(), pos() { }

        gcoords(grid_t cat, dcoords_t pos) : cat(cat), pos(pos) { }

        template<typename... Args>
        explicit gcoords(grid_t cat, Args && ... args) : cat(cat), pos(std::forward<Args>(args)...) { }

        inline bool_t operator<(const gcoords & rhs) const {
            return (cat == rhs.cat) ? pos < rhs.pos : cat < rhs.cat;
        }

        inline bool_t operator>(const gcoords & rhs) const {
            return (cat == rhs.cat) ? pos > rhs.pos : cat > rhs.cat;
        }

        inline bool_t operator<=(const gcoords & rhs) const {
            return !(*this > rhs);
        }

        inline bool_t operator>=(const gcoords & rhs) const {
            return !(*this < rhs);
        }

        inline bool_t operator==(const gcoords & rhs) const {
            return cat == rhs.cat && pos == rhs.pos;
        }

        inline bool_t operator!=(const gcoords & rhs) const {
            return !(*this == rhs);
        }

        inline friend ostream & operator<<(ostream & os, const gcoords & gc) {
            switch (gc.cat) {
                case grid_t::MODEL_GRID:
                    os << "M";
                    break;
                case grid_t::BANK_GRID:
                    os << "B";
                    break;
                case grid_t::INVALID_GRID:
                    os << "I";
                    break;
            }
            return os << gc.pos;
        }

        inline friend istream & operator>>(istream & is, gcoords & gc) {
            char_t g = is.get();
            switch (g) {
                case 'M': {
                    gc.cat = grid_t::MODEL_GRID;
                    break;
                }
                case 'B': {
                    gc.cat = grid_t::BANK_GRID;
                    break;
                }
                case 'I': {
                    gc.cat = grid_t::INVALID_GRID;
                    break;
                }
                default: {
                    raise(*new std::runtime_error("Invalid serialization"));
                }
            }
            is >> gc.pos;
            return is;
        }
    };

    typedef gcoords gcoords_t;

    ///
    class cell_h : public std::variant<std::monostate, gcoords_t, cargo_h> {
    public:
        using std::variant<std::monostate, gcoords_t, cargo_h>::variant;
        using std::variant<std::monostate, gcoords_t, cargo_h>::operator=;
    };
}

namespace std {
    /// Specialization of `std::hash` for coordinates of given type
    /// \tparam T Template type of the coordinate
    template<typename T>
    struct hash<har::coords<T>> {
        constexpr std::size_t operator()(const har::coords<T> & coords) const noexcept {
            return (std::size_t(coords.x) << sizeof(std::size_t) * 4) + std::size_t(coords.y);
        }
    };
}

#endif //HAR_COORDS_HPP
