//
// Created by Johannes on 09.06.2020.
//

#pragma once

#ifndef HAR_VALUE_HPP
#define HAR_VALUE_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <variant>

#include <har/coords.hpp>
#include <har/exception.hpp>
#include <har/platform.hpp>

namespace har {

    /// \brief RGBA32 color type
    struct color {
        std::uint8_t r; ///<Red component
        std::uint8_t g; ///<Green component
        std::uint8_t b; ///<Blue component
        std::uint8_t a; ///<Alpha component

        /// Creates a color struct for solid black
        /// \brief Default constructor
        constexpr color() noexcept: r(0),
                                    g(0),
                                    b(0),
                                    a(std::numeric_limits<std::uint8_t>::max()) {

        }

        /// \brief Constructs a color from separate channels
        /// \param [in] r Red component
        /// \param [in] g Green component
        /// \param [in] b Blue component
        /// \param [in] a Alpha component
        constexpr color(std::uint8_t r,
                        std::uint8_t g,
                        std::uint8_t b,
                        std::uint8_t a) noexcept: r(r),
                                                  g(g),
                                                  b(b),
                                                  a(a) {

        }

        /// \brief Copy constructor
        /// \param [in] ref Reference to the original
        constexpr color(const color & ref) = default;

        /// \brief Move constructor
        /// \param [in,out] fref Forwarding reference to the original
        color(color && fref) noexcept = default;

        /// \brief Copy assignemnt
        /// \param [in] ref Reference to the original
        /// \return The assigned color
        color & operator=(const color & ref) = default;

        /// \brief Move assignemtn
        /// \param [in,out] fref Forwarding reference to the original
        /// \return The assigned color
        color & operator=(color && fref) noexcept = default;

        /// \brief Default destructor
        ~color() = default;

        /// \brief Compares two colors for equality
        /// \param [in] rhs Other color
        /// \return `TRUE`, if both colors are equal
        constexpr bool_t operator==(const color & rhs) const {
            return (r == rhs.r)
                   && (g == rhs.g)
                   && (b == rhs.b)
                   && (a == rhs.a);
        }

        /// \brief Compares two colors for inequality
        /// \param [in] rhs Other color
        /// \return `TRUE`, if both colors are not equal
        constexpr bool_t operator!=(const color & rhs) const {
            return !(*this == rhs);
        }

        /// \brief Checks if the color is not transparent black
        /// \return Wheter the color is not transparent black
        explicit constexpr operator bool_t() const {
            return r || g || b || a;
        }

        /// \brief Converts the color into a tuple
        /// \return The color as a tuple
        explicit operator std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>() const {
            return std::make_tuple(r, g, b, a);
        }

        friend ostream & operator<<(ostream & os, const color & c);

        friend istream & operator>>(istream & is, color & c);

    };

    /// \brief Writes a color into an output stream
    /// \param [in,out] os The output stream
    /// \param [in] c The color to be serialized
    /// \return The output stream
    ostream & operator<<(ostream & os, const color & c);

    /// \brief Reads a color from an input stream
    /// \param [in,out] is The input stream
    /// \param [in] c The color to be deserialized
    /// \return The input stream
    istream & operator>>(istream & is, color & c);

    /// \brief 32-bit RGBA color type
    typedef color color_t;

    /// \brief Type to transport arbitrary data for visualization
    typedef std::any image_t;

    /// \brief Class template for comparable function objects
    /// \tparam Sig Function signature
    template<typename Sig>
    class function;

    /// \brief Class template for comparable function objects
    /// \tparam R Return type
    /// \tparam Args Parameter pack for function arguments
    template<typename R, typename... Args>
    class function<R(Args...)> : public std::function<R(Args...)> {
        using Base = std::function<R(Args...)>;

    public:
        using Base::Base;

        function(const function & ref) = default;

        function(function && fref) noexcept = default;

        /// Compares two functions for equality
        /// \param [in] rhs Other function
        /// \return `TRUE`, if both objects have the same function pointer
        inline bool_t operator==(const function & rhs) const noexcept {
            using Sig = R(*)(Args...);
            return this->template target<Sig>() == rhs.template target<Sig>();
        }

        /// Compares two functions for inequality
        /// \param [in] rhs Other function
        /// \return `TRUE`, if both objects have different function pointers
        inline bool_t operator!=(const function & rhs) const noexcept {
            return !operator==(rhs);
        }

        using Base::operator=;

        function & operator=(const function & ref) = default;

        function & operator=(function && fref) noexcept = default;

        /// \brief Default destructor
        ~function() = default;
    };

    /// \brief Type for callbacks that accept a cell
    typedef function<void(class cell &)> callback_t;

    /// \brief Type for callbacks that accept a const cell
    typedef function<void(const class cell &)> ccallback_t;

    enum raw_part : std::size_t;

    /// \brief Numeric type referencing a defined part
    typedef raw_part part_h;

    constexpr static struct {
        constexpr part_h operator[](std::size_t val) const {
            return part_h(val);
        }
    } PART;

    constexpr part_h operator+(part_h lhs, part_h rhs) {
        return part_h(std::size_t(lhs) + std::size_t(rhs));
    }

    /// To reduce the size of `har::value`,
    /// values of greater size than two times the target architectures bit width are stored via a pointer
    /// \brief Size invariant wrapper for objects to be stored in the `har::value` variant
    /// \tparam T Outer type
    /// \tparam Ptr Pointer type of `T`
    /// \sa har::value
    template<typename T, typename Ptr = std::unique_ptr<T>>
    struct possibly_pointed {
        static constexpr bool is_pointed =
                sizeof(T) > 2 * sizeof(std::size_t); ///<Checks, if the value should be indirected
        using inner_type = typename std::conditional<is_pointed, Ptr, T>::type; ///<Type that will be used internally to store the value
        using outer_type = T; ///<Type, this class outwardly appears as

    private:
        inner_type _data; ///<Variable that holds the atual value direct or indirect

    public:

        /// \brief Default constructor
        inline possibly_pointed() {
            if constexpr (is_pointed) {
                new(&_data) inner_type(new T());
            } else {
                new(&_data) inner_type();
            }
        }

        /// \brief Constructs an instance from a copy of the value to be stored
        /// \param [in] val Value to be stored
        inline explicit possibly_pointed(const T & val) {
            if constexpr (is_pointed) {
                new(&_data) inner_type(new T(val));
            } else {
                new(&_data) inner_type(val);
            }
        }

        /// \brief Copy constructor
        /// \param [in] ref Reference to original
        inline possibly_pointed(const possibly_pointed & ref) {
            if constexpr (is_pointed) {
                new(&_data) inner_type(new T(*ref._data));
            } else {
                new(&_data) inner_type(ref._data);
            }
        }

        /// \brief Constructs an instance from the value to be stored
        /// \param [in] fref Forwarding reference to the value to be stored
        inline explicit possibly_pointed(T && val) {
            if constexpr (is_pointed) {
                new(&_data) inner_type(new T(std::forward<T>(val)));
            } else {
                new(&_data) inner_type(std::forward<T>(val));
            }
        }

        /// \brief Constructs the value to store in-place
        /// \tparam Args Constructor parameter pack for the stored type
        /// \param args Constructor arguments for the stored type
        template<typename... Args>
        inline explicit possibly_pointed(Args && ...args) {
            if constexpr (is_pointed) {
                new(&_data) inner_type(new T(std::forward<T>(args)...));
            } else {
                new(&_data) inner_type(std::forward<T>(args)...);
            }
        }

        /// \brief Move constructore
        /// \param [in,out] fref Forwarding reference to the original
        possibly_pointed(possibly_pointed && fref) noexcept = default;

        /// \param ref
        /// \return
        inline bool_t operator==(const possibly_pointed & ref) const {
            if constexpr (!std::is_same_v<outer_type, special_t>) {
                if constexpr (is_pointed) {
                    auto * lhs = _data.get();
                    auto * rhs = ref._data.get();
                    return lhs == rhs || *lhs == *rhs;
                } else {
                    return _data == ref._data;
                }
            } else {
                return false;
            }
        }

        /// \param ref
        /// \return
        inline bool_t operator!=(const possibly_pointed & ref) const {
            return !operator==(ref);
        }

        /// \tparam U
        /// \param ref
        /// \return
        template<typename U>
        inline possibly_pointed & operator=(const U & ref) {
            if constexpr (is_pointed) {
                *_data = T(ref);
            } else {
                _data = T(ref);
            }
            return *this;
        }

        /// \tparam U
        /// \param ref
        /// \return
        template<typename U>
        inline possibly_pointed & operator=(U && ref) noexcept {
            if constexpr (is_pointed) {
                *_data = T(std::forward<U>(ref));
            } else {
                _data = T(std::forward<U>(ref));
            }
            return *this;
        }

        /// \param ref
        /// \return
        inline possibly_pointed & operator=(const possibly_pointed & ref) {
            if (this != &ref) {
                this->~possibly_pointed();
                new(this) possibly_pointed(ref);
            }
            return *this;
        }

        /// \param fref
        /// \return
        possibly_pointed & operator=(possibly_pointed && fref) noexcept = default;

        /// \return
        inline operator T &() { //NOLINT
            if constexpr (is_pointed) {
                return *_data;
            } else {
                return _data;
            }
        }

        /// \return
        inline operator const T &() const { //NOLINT
            if constexpr (is_pointed) {
                return *_data;
            } else {
                return _data;
            }
        }

        /// \brief Default destructor
        ~possibly_pointed() = default;
    };

    /// \brief Determines whether a type can be stored in `har::value`
    /// \tparam T Type to check
    /// \sa har::value
    template<typename T>
    struct is_value_type : std::disjunction<
            std::is_same<T, har::bool_t>,
            std::is_same<T, har::int_t>,
            std::is_same<T, har::uint_t>,
            std::is_same<T, har::double_t>,
            std::is_same<T, har::string_t>,
            std::is_same<T, har::direction_t>,
            std::is_same<T, har::dcoords_t>,
            std::is_same<T, har::ccoords_t>,
            std::is_same<T, har::color_t>,
            std::is_same<T, har::special_t>,
            std::is_same<T, har::callback_t>,
            std::is_same<T, har::part_h>> {

    };

    /// \enum
    /// \brief Special property IDs used by the simulation logic and standard parts
    enum of : uint_t {
        /// <b>Type:</b><br/>undefined
        /// \brief Invalid property ID
        VOID = 0,

        /// <b>Type:</b><br/><tt>har::part_h</tt>
        /// \brief Runtime ID of a cell's associated part
        TYPE,
        /// <b>Type:</b><br/><tt>har::string_t</tt>
        /// \brief Name of the cell for display in GUIs
        NAME,

        /// <b>Type:</b><br/><tt>har::color_t</tt>
        /// \brief Color of the cell
        COLOR,

        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powered from in UP direction
        POWERED_UP,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powered from in DOWN direction
        POWERED_DOWN,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powered from in RIGHT direction
        POWERED_RIGHT,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powered from in LEFT direction
        POWERED_LEFT,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powered from in PIN directions
        POWERED_PIN,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powering it's neighbor in DOWN direction
        POWERING_DOWN,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powering it's neighbor in UP direction
        POWERING_UP,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powering it's neighbor in LEFT direction
        POWERING_LEFT,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powering it's neighbor in RIGHT direction
        POWERING_RIGHT,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is powering it's connected cells
        POWERING_PIN,
        /// This may be used for loop detection in complex models<br/>
        /// <b>Type:</b><br/><tt>har::bool_t</tt>
        /// \brief <tt>true</tt>, if the cell is the origin of powering
        POWERING_FIRST,

        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moved from UP direction
        MOVED_UP,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moved from DOWN direction
        MOVED_DOWN,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moved from RIGHT direction
        MOVED_RIGHT,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moved from LEFT direction
        MOVED_LEFT,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moving the cell in DOWN direction
        MOVING_DOWN,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moving the cell in UP direction
        MOVING_UP,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moving the cell in LEFT direction
        MOVING_LEFT,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The value with which the cell is moving the cell in RIGHT direction
        MOVING_RIGHT,
        /// <b>Type:</b><br/><tt>har::bool_t</tt>
        /// \brief <tt>true</tt>, if the cell is the origin of a movement
        MOVING_FIRST,
        /// <b>Type:</b><br/><tt>har::direction_t</tt><br/>
        /// <b>Values:</b><br/><tt>direction::UP</tt>, <br/> <tt>direction::DOWN</tt>, <br/> <tt>direction::RIGHT</tt>, <br/> <tt>direction::LEFT</tt>, <br/>
        /// \brief The direction from which the cell receives movement
        MOVING_FROM,
        /// <b>Type:</b><br/><tt>har::direction_t</tt><br/>
        /// <b>Values:</b><br/><tt>direction::UP</tt>, <br/> <tt>direction::DOWN</tt>, <br/> <tt>direction::RIGHT</tt>, <br/> <tt>direction::LEFT</tt>, <br/>
        /// \brief The direction from which the cell propagates movement
        MOVING_TO,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The speed that a motor transfers to other moving parts
        MOTOR_SPEED,
        /// This can be used for loop detection in complex models<br/>
        /// <b>Type:</b><br/><tt>har::uint_t</tt>
        /// \brief The distance to the next movement source
        MOTOR_DISTANCE,


        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The weight of a cargo cell
        WEIGHT,

        /// This property can automatically be set by the automaton when calculating moving cargo cells<br/>
        /// <b>Type:</b><br/><tt>har::bool_t</tt>
        /// \brief <tt>true</tt>, if the cell has cargo centered over it
        HAS_CARGO,
        /// This property can automatically be set by the automaton when calculating moving cargo cells<br/>
        /// <b>Type:</b><br/><tt>har::bool_t</tt>
        /// \brief <tt>true</tt>, if the cell has cargo overlapping onto it
        HAS_ARTIFACTS,

        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The maximal voltage the part is designed for
        MAX_VOLTAGE,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The minimal voltage of the cell considered high
        HIGH_VOLTAGE,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The voltage of the cell in inactive state
        NEUTRAL_VOLTAGE,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The maximum voltage of the cell considered low
        LOW_VOLTAGE,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The minimal voltage the part is designed for
        MIN_VOLTAGE,

        /// <b>Type:</b><br/><tt>har::uint_t</tt><br/>
        /// <b>Values:</b><br/><tt>0</tt>: Tri-state, <br/> <tt>1</tt>: Output, <br/> <tt>2</tt>: Input
        /// \brief The mode a cell of a GPIO part is set to
        PIN_MODE,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The analog voltage of a cell
        ANALOG_VOLTAGE,
        /// <b>Type:</b><br/><tt>har::bool_t</tt>
        /// \brief The digital voltage of a cell
        DIGITAL_VOLTAGE,
        /// <b>Type:</b><br/><tt>har::double_t</tt>
        /// \brief The voltage cycle of a PWM configuration
        PWM_VOLTAGE,
        /// <b>Type:</b><br/><tt>har::double_t</tt><br/>
        /// <b>Values:</b><br/><tt>0..1</tt>
        /// \brief The duty cycle of a PWM configuration
        PWM_DUTY,

        /// <b>Type:</b><br/><tt>har::bool_t</tt>
        /// \brief <tt>true</tt>, if the sensor cell is firing
        FIRING,

        /// <b>Type:</b><br/><tt>har::direction_t</tt><br/>
        /// <b>Values:</b><br/><tt>direction::UP</tt>, <br/> <tt>direction::DOWN</tt>, <br/> <tt>direction::RIGHT</tt>, <br/> <tt>direction::LEFT</tt>, <br/>
        /// \brief The direction the cell is facing
        FACING,

        OPERATING_VOLTAGE,

        SPEED_FACTOR,

        /// <b>Type:</b><br/><tt>har::uint_t</tt><br/>
        /// <b>Values:</b><br/><tt>0</tt>: Positive, <br/> <tt>1</tt>: Negative
        /// \brief The logic polarity of a cell
        LOGIC_POLARITY,

        /// <b>Type:</b><br/>undefined
        /// \brief A significant value of the cell
        VALUE,
        /// <b>Type:</b><br/>undefined
        /// \brief The lower bound of the significant value of the cell
        MIN_VALUE,
        /// <b>Type:</b><br/>undefined
        /// \brief The upper bound of the significant value of the cell
        MAX_VALUE,
        /// <b>Type:</b><br/>undefined
        /// \brief The delta in which the significant value of the cell increments
        VALUE_STEP,

        /// <b>Type:</b><br/>undefined
        /// \brief The delta in which a timer increments
        TIMER_STEP,
        /// <b>Type:</b><br/><tt>har::uint_t</tt>
        /// \brief The timeout of the timer in cycles
        TIMER_PAUSE,

        /// <b>Type:</b><br/><tt>har::callback_t</tt>
        /// \brief The callback function of an interrupt GPIO pin
        INT_HANDLER,
        /// <b>Type:</b><br/><tt>har::bool_t</tt>
        /// \brief <tt>true</tt> if the interrupt is enabled
        INT_CONDITION,

        /// <b>Type:</b><br/><tt>har::uint_t</tt><br/>
        /// <b>Values:</b><br/>
        /// undefined
        /// \brief Cosmetic variants to otherwise identical parts
        DESIGN,

        /// This can be used as the starting point when enumerating own property IDs
        /// \brief The next free numeric value for personal use
        NEXT_FREE
    };

    template<typename... Tps>
    using possibly_pointed_variant = std::variant<std::monostate, possibly_pointed<Tps>...>;

    /// \brief Base class for `har::value`
    using value_base = possibly_pointed_variant<
            har::bool_t,
            har::int_t,
            har::uint_t,
            har::double_t,
            har::string_t,
            har::ccoords_t,
            har::dcoords_t,
            har::direction_t,
            har::color_t,
            har::special_t,
            har::callback_t,
            har::part_h>;

    template<typename T>
    constexpr uint_t type_index() {
        if constexpr(std::is_same_v<bool_t, T>) {
            return 1u;
        } else if constexpr(std::is_same_v<int_t, T>) {
            return 2u;
        } else if constexpr(std::is_same_v<uint_t, T>) {
            return 3u;
        } else if constexpr(std::is_same_v<double_t, T>) {
            return 4u;
        } else if constexpr(std::is_same_v<string_t, T>) {
            return 5u;
        } else if constexpr(std::is_same_v<ccoords_t, T>) {
            return 6u;
        } else if constexpr(std::is_same_v<dcoords_t, T>) {
            return 7u;
        } else if constexpr(std::is_same_v<direction_t, T>) {
            return 8u;
        } else if constexpr(std::is_same_v<color_t, T>) {
            return 9u;
        } else if constexpr(std::is_same_v<special_t, T>) {
            return 10u;
        } else if constexpr(std::is_same_v<callback_t, T>) {
            return 11u;
        } else if constexpr(std::is_same_v<part_h, T>) {
            return 12u;
        } else {
            return 0u;
        }
    }

    /// \brief Stores the values for the properties of cells
    class value final : public value_base {
    private:
        using value_base::operator=;

    public:
        static value & invalid();

        /// \brief The `I`th variant type of this class
        /// \tparam I Index
        template<std::size_t I>
        using dtype = std::variant_alternative_t<I, value_base>;

        /// \brief Enumeration for supported datatypes
        enum class datatype : ushort_t {
            VOID = 0u, ///<Void or invalid type
            BOOLEAN = 1u, ///<Boolean type (`har::bool_t`)
            INTEGER = 2u, ///<Signed integer type (`har::int_t`)
            UNSIGNED = 3u, ///<Unsigned integer type (`har::uint_t`)
            DOUBLE = 4u, ///<Double precision floating point type (`har::double_t`)
            STRING = 5u, ///<String type (`har::string_t`)
            C_COORDINATES = 6u, ///<Continous coordinates (`har::ccoords_t`)
            D_COORDINATES = 7u, ///<Discrete coordinates (`har::dcoords_t`)
            DIRECTION = 8u, ///<Direction type (`har::direction_t`)
            COLOR = 9u, ///<Color type (`har::color_t`)
            SPECIAL = 10u, ///<Arbitrary type (`har::special_t`)
            CALLBACK = 11u, ///<Callback function type (`har::callback_t`)
            HASH = 12u ///<Type for hashes and handles (`har::part_h`)
        };

        /// \brief Gets the unmangled name of a datatype used in `har::value`
        /// \param type Enum value for a datatype
        /// \return Name of the datatype
        static string_t datatype_name(datatype type);

        /// \param str
        /// \return
        static of from_string(string_t & str);

        /// \param of
        /// \return
        static string_t to_string(of of);

        /// \param dir
        /// \return
        static constexpr of moving(direction_t dir) {
            switch (dir) {
                case direction::UP:
                    return of::MOVING_UP;
                case direction::DOWN:
                    return of::MOVING_DOWN;
                case direction::RIGHT:
                    return of::MOVING_RIGHT;
                case direction::LEFT:
                    return of::MOVING_LEFT;
                default:
                    raise(std::invalid_argument("Argument must be a cardinal direction."));
            }
        }

        /// \param dir
        /// \return
        static constexpr of moved(direction_t dir) {
            switch (dir) {
                case direction::UP:
                    return of::MOVED_UP;
                case direction::DOWN:
                    return of::MOVED_DOWN;
                case direction::RIGHT:
                    return of::MOVED_RIGHT;
                case direction::LEFT:
                    return of::MOVED_LEFT;
                default:
                    raise(std::invalid_argument("Argument must be a cardinal direction."));
            }
        }

        /// \param dir
        /// \return
        static constexpr of powering(direction_t dir) {
            switch (dir) {
                case direction::UP:
                    return of::POWERING_UP;
                case direction::DOWN:
                    return of::POWERING_DOWN;
                case direction::RIGHT:
                    return of::POWERING_RIGHT;
                case direction::LEFT:
                    return of::POWERING_LEFT;
                case direction::PIN:
                    return of::POWERING_PIN;
                default:
                    raise(std::invalid_argument("Argument must be a cardinal direction or PIN."));
            }
        }

        /// \param dir
        /// \return
        static constexpr of powered(direction_t dir) {
            switch (dir) {
                case direction::UP:
                    return of::POWERED_UP;
                case direction::DOWN:
                    return of::POWERED_DOWN;
                case direction::RIGHT:
                    return of::POWERED_RIGHT;
                case direction::LEFT:
                    return of::POWERED_LEFT;
                case direction::PIN:
                    return of::POWERED_PIN;
                default:
                    raise(std::invalid_argument("Argument must be a cardinal direction or PIN."));
            }
        }

        /// \param type
        /// \return
        static value get_standard(datatype type);

        /// Checks if the contained value is equal to a default constructed value of same type
        /// \return `TRUE`, if the contained value is equal to a default constructed
        bool_t is_standard();

        /// \brief Default constructor
        value() : value_base() { }

        /// \brief Constructs a value that hold a given object
        /// \tparam T Type of the held object
        /// \param [in] ref Object to hold
        template<typename T, typename std::enable_if<is_value_type<T>::value, T>::type * = nullptr>
        explicit value(const T & ref) : value_base(possibly_pointed<T>(ref)) { }

        /// \brief Constructs a value that hold a given object
        /// \tparam T Type of the held object
        /// \param [in,out] ref Object to hold
        template<typename T, typename std::enable_if<is_value_type<T>::value, T>::type * = nullptr>
        explicit value(T && fref) : value_base(possibly_pointed<T>(std::forward<T>(fref))) { } //NOLINT

        /// \brief Constructs a value that holds no value
        /// \param [in] ref Instance of `std::monostate`
        explicit value(const std::monostate & ref) : value_base(ref) { }

        /// \brief Constructs a value that holds no value
        /// \param [in] ref Instance of `std::monostate`
        explicit value(std::monostate && fref) : value_base(fref) { }

        /// \brief Copy constructor
        /// \param [in] ref Reference to the original
        value(const value & ref) = default;

        /// \brief Copy constructor
        /// \param [in] fref Forwarding reference to the original
        value(value && fref) noexcept = default;

        /// \brief Assigns a copy of an object to the value
        /// \tparam T Type of the object
        /// \param [in] ref Object to copy into the value
        /// \return This value
        template<typename T, typename std::enable_if<is_value_type<T>::value, T>::type * = nullptr>
        value & operator=(const T & ref) {
            value_base::operator=(possibly_pointed<T>(ref));
            return *this;
        }

        /// \brief Assigns an object to the value
        /// \tparam T Type of the object
        /// \param [in] fref Object to move into the value
        /// \return This value
        template<typename T, typename std::enable_if<is_value_type<T>::value, T>::type * = nullptr>
        value & operator=(T && fref) noexcept {
            value_base::operator=(possibly_pointed<T>(std::forward<T>(fref)));
            return *this;
        }

        /// \brief Gets the enum value of the contained datatype
        /// \return The contained datatype
        [[nodiscard]]
        constexpr datatype type() const {
            return datatype(index());
        }

        /// \brief Copy assignment
        /// \param [in] ref Reference to the original
        /// \return This value
        value & operator=(const value & ref) = default;

        /// \brief Move assignment
        /// \param [in,out] fref Forwarding reference to the original
        /// \return This value
        value & operator=(value && fref) = default;

        /// \brief Serializes this value into a string
        /// \return The string representation
        explicit operator string_t() const;

        /// \brief Default constructor
        ~value() = default;

        friend ostream & operator<<(ostream & os, const value & val);
    };

    /// \param [in,out] os
    /// \param [in] val
    /// \return
    ostream & operator<<(ostream & os, const value & val);

    /// \brief Numeric value representing a type held by value
    /// \sa value
    typedef value::datatype datatype;

    /// \brief Numeric type referencing a defined parts entry
    typedef of entry_h;

    /// \param val
    /// \return
    datatype datatype_of(std::size_t val);

    /// \brief Constructs a value object from arguments of it's variant types constructors
    /// \tparam T Target type
    /// \tparam Args Argument types
    /// \param args Arguments
    /// \return The new value
    template<typename T, typename ...Args>
    value make_value(Args && ... args) {
        if constexpr (!std::is_same_v<T, std::monostate>) {
            return value(T(std::forward<Args>(args)...));
        } else
            return value(args...);
    }

    namespace exception {
        class datatype_mismatch : public exception {
        private:
            const value::datatype _expected; ///<
            const value::datatype _actual; ///<

        public:
            /// \param source
            /// \param expected
            /// \param actual
            datatype_mismatch(const std::string & source, value::datatype expected, value::datatype actual);

            /// \return
            [[nodiscard]]
            const char * what() const noexcept override;

            /// \return
            [[nodiscard]]
            value::datatype expected() const noexcept;

            /// \return
            [[nodiscard]]
            value::datatype actual() const noexcept;

            ~datatype_mismatch() noexcept override;
        };
    }

    /// \brief Gets the object of type `T` held by a `val`, if this type is currently held by `val`
    /// \tparam T The type of the object to get
    /// \param [in] val The containing value
    /// \return Reference to the contained object
    template<typename T>
    [[nodiscard]]
    T & get(value & val) {
        if (type_index<T>() == val.index()) {
            return std::get<possibly_pointed<T>>(val);
        } else {
            raise(*new exception::datatype_mismatch(FUNCTION, value::datatype(type_index<T>()),
                                                    value::datatype(val.index())));
        }
    }

    /// \brief Gets the object of type `T` held by a `val`, if this type is currently held by `val`
    /// \tparam T The type of the object to get
    /// \param [in] val The containing value
    /// \return Const reference to the contained object
    template<typename T>
    [[nodiscard]]
    const T & get(const value & val) {
        if (type_index<T>() == val.index()) {
            return std::get<possibly_pointed<T>>(val);
        } else {
            raise(*new exception::datatype_mismatch(FUNCTION, value::datatype(type_index<T>()),
                                                    value::datatype(val.index())));
        }
    }

    /// \brief Gets the object of type `T` held by a `val`, if this type is currently held by `val`
    /// \tparam T The type of the object to get
    /// \param [in] val The containing value
    /// \return Reference to the contained object
    template<typename T>
    [[nodiscard]]
    T & get(value && val) {
        if (type_index<T>() == val.index()) {
            return std::get<possibly_pointed<T>>(std::forward<value>(val));
        } else {
            raise(*new exception::datatype_mismatch(FUNCTION, value::datatype(type_index<T>()),
                                                    value::datatype(val.index())));
        }
    }

    /// \brief Gets the object of type `T` held by a `val`, if this type is currently held by `val`
    /// \tparam T The type of the object to get
    /// \param [in] val The containing value
    /// \return Const reference to the contained object
    template<typename T>
    [[nodiscard]]
    const T & get(const value && val) {
        if (type_index<T>() == val.index()) {
            return std::get<possibly_pointed<T>>(std::forward<const value>(val));
        } else {
            raise(*new exception::datatype_mismatch(FUNCTION, value::datatype(type_index<T>()),
                                                    value::datatype(val.index())));
        }
    }

    /// \brief Adds a constant to a property ID
    /// \tparam T Type of the constant
    /// \param [in] lhs Property ID
    /// \param [in] rhs The constant to add
    /// \return Sum of both
    template<typename T>
    har::of operator+(har::of lhs, T rhs) {
        return har::of(har::uint_t(lhs) + har::uint_t(rhs));
    }

    /// \brief Subtracts a constant to a property ID
    /// \tparam T Type of the constant
    /// \param [in] lhs Property ID
    /// \param [in] rhs The constant to subtract
    /// \return Difference of both
    template<typename T>
    har::of operator-(har::of lhs, T rhs) {
        return har::of(har::uint_t(lhs) - har::uint_t(rhs));
    }
}

/// \brief STL namespace
namespace std { //NOLINT

    /// \tparam T
    /// \param x
    /// \return
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr>
    inline constexpr har::coords<T> ceil(har::coords<T> x) {
        return har::coords(T(std::ceil(x.x)), T(std::ceil(x.y)));
    };

    /// \tparam T
    /// \param x
    /// \return
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr>
    inline constexpr har::coords<T> floor(har::coords<T> x) {
        return har::coords(T(std::floor(x.x)), T(std::floor(x.y)));
    }

    /// \tparam T
    /// \param x
    /// \return
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr>
    inline constexpr har::coords<T> abs(har::coords<T> x) {
        return har::coords(T(std::abs(x.x)), T(std::abs(x.y)));
    };

    /// \tparam T
    /// \param x
    /// \return
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr>
    inline constexpr har::coords<T> fabs(har::coords<T> x) {
        return har::coords(T(std::fabs(x.x)), T(std::fabs(x.y)));
    };

}

#endif //HAR_VALUE_HPP
