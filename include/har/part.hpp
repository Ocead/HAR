//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_PART_HPP
#define HAR_PART_HPP

#include <initializer_list>
#include <map>
#include <utility>
#include <set>
#include <variant>

#include <har/cell.hpp>
#include <har/property.hpp>
#include <har/traits.hpp>
#include <har/value.hpp>

namespace har {

    class cell;

    class cell_base;

    ///
    enum ui_access : ushort_t {
        INVISIBLE = 0u, ///<Invisible in UIs
        VISIBLE = 1u, ///<Visible in UIs
        CHANGEABLE = 2u ///<Changeable in UIs
    };

    enum serialize : ushort_t {
        SERIALIZE = 0u, ///<Serialize if value is not standard
        NO_SERIALIZE = 1u, ///<Never serialize
        ANYWAY = 2u ///<Serialize even if value is standard
    };

    /// \brief Defines an entry in the property model of a part
    struct entry {
        using Specifics = std::variant<
                std::monostate,
                std::array<int_t, 3>,
                std::array<uint_t, 3>,
                std::array<double_t, 3>,
                std::shared_ptr<std::map<uint_t, string_t>>,
                dir_cat>; ///<Type holding additional information on valid values for this entry

        entry_h id; ///<Unique ID of the entry
        string_t unique_name; ///<Unique name for SerDes
        string_t friendly_name; ///<Name for display in UIs
        value type_and_default; ///<Type and default value
        ui_access access; ///<Accessibility in UIs
        serialize serializable; ///<Whether the property should be serialized
        Specifics specifics; ///<Additional information on valid values

        /// \brief Constructor
        /// \param [in] id Unique ID of the entry
        /// \param [in] unique_name Unique name of the entry
        /// \param [in] friendly_name Friendly name of the entry
        /// \param [in] type Datatype of the entry
        /// \param [in] access Accessibility in UIs
        /// \param [in] serializable Whether the property should be serialized
        /// \param [in] specifics Additional information on valid values
        entry(entry_h id, string_t unique_name, string_t friendly_name,
              value && type_and_default, ui_access access, serialize serializable, const Specifics & specifics);

        /// \brief Constructor
        /// \tparam SpecArgs Parameter pack to pass to the specifics constructor
        /// \param [in] id Unique ID of the entry
        /// \param [in] unique_name Unique name of the entry
        /// \param [in] friendly_name Friendly name of the entry
        /// \param [in] type Datatype of the entry
        /// \param [in] access Accessibility in UIs
        /// \param [in] serializable Whether the property should be serialized
        /// \param [in] specifics Additional information on valid values
        template<typename... SpecArgs>
        entry(entry_h id, string_t unique_name, string_t friendly_name,
              value && type_and_default, ui_access access, serialize serializable,
              SpecArgs && ... specifics) : id(id),
                                           unique_name(std::move(unique_name)),
                                           friendly_name(std::move(friendly_name)),
                                           type_and_default(std::forward<value>(type_and_default)),
                                           access(access),
                                           serializable(serializable),
                                           specifics() {
            if (this->type_and_default.type() == value::datatype::UNSIGNED) {
                if constexpr (std::is_constructible_v<std::map<uint_t, string_t>, SpecArgs ...>) {
                    new(&this->specifics) Specifics(
                            std::make_shared<std::map<uint_t, string_t>>(std::forward<SpecArgs>(specifics) ...));
                } else {
                    new(&this->specifics) Specifics(std::forward<SpecArgs>(specifics) ...);
                }
            } else {
                if constexpr (!std::is_constructible_v<std::map<uint_t, string_t>, SpecArgs ...>) {
                    new(&this->specifics) Specifics(std::forward<SpecArgs>(specifics) ...);
                }
            }
        }

        entry(entry_h id, string_t unique_name, string_t friendly_name,
              value type_and_default, ui_access access, serialize serializable,
              std::initializer_list<std::pair<uint_t, string_t>> && ilist) : id(id),
                                                                             unique_name(std::move(unique_name)),
                                                                             friendly_name(std::move(friendly_name)),
                                                                             type_and_default(std::forward<value>(type_and_default)),
                                                                             access(access),
                                                                             serializable(serializable),
                                                                             specifics() {
            if (this->type_and_default.type() == value::datatype::UNSIGNED) {
                specifics = std::make_shared<std::variant_alternative_t<4, Specifics>::element_type>();
                auto ptr = std::get<4>(specifics);
                for (auto & p : ilist) {
                    ptr->insert(p);
                }
            }
        }

        /// Converts a value to string, using additional information from this entry
        /// \param [in] val The value to serialize
        /// \return The serialized value
        [[nodiscard]]
        string_t to_string(const value & val) const;

        /// \brief Creates a value from string while upholding contraints set by this entry
        /// \param [in] str The serialized value
        /// \return The deserialized value
        [[nodiscard]]
        value from_string(const string_t & str) const;

        /// \brief Determines whether a value is a standard value in the constraints of this entry
        /// \param [in] val Value in question
        /// \return <tt>TRUE</tt>, if the value is a standard value
        [[nodiscard]]
        bool_t is_standard(const value & val) const;

        /// \brief Returns a standard value for this entry that satisfies the specifics on valid values
        /// \return The standard value for the entry
        [[nodiscard]]
        value standard_value() const;
    };

    /// \brief Type to use as base for the property model of a part
    using property_model = std::map<entry_h, entry>;

    namespace exception {

        ///
        class delegate_error : public exception {
        private:
            const std::exception & _inner; ///<

        public:

            ///
            /// \param source
            /// \param inner
            delegate_error(std::string source, std::exception & inner);

            ///
            /// \return
            [[nodiscard]]
            const std::exception & inner() const noexcept;

            ///
            /// \return
            [[nodiscard]]
            const char * what() const noexcept override;
        };
    }

    /// \brief A definition of a part in the simulation
    class part final {
    private:
        part_h _id; ///<Unique ID of the part
        string_t _unique_name; ///<Unique name of the part
        string_t _friendly_name; ///<Friendly name of the part for display in UI
        traits_h _traits; ///<Generic traits of the part
        std::map<entry_h, entry> _model; ///<Definition of properties of the part
        std::map<direction_t, string_t> _conn_use; ///<Names for designated connections
        std::set<of> _visual; ///<Properties that the visuals of the cell depend on
        std::set<of> _waking; ///<Properties that wake this part on change

    public:
        static part & invalid(); ///<

        /// \brief Contains the function delegates that define the behaviour of the part
        struct part_delegates {

            /// \brief Delegate called to initialize a cell
            /// \param [out] cl The cell
            std::function<void(cell & cl)> init_static;
            /// This delegate is invoked after a cell of this type has been placed within a model's grid
            /// and has been initialized with <tt>har::part::init_standard</tt>
            /// \brief Delegate called to initialize a cell in relation to it's neighbors
            /// \param [in,out] cl The cell
            std::function<void(cell & cl)> init_relative;
            /// \brief Delegate called before a cell is destroyed
            /// \param [in,out] cl The cell
            std::function<void(cell & cl)> clear;

            /// \brief Delegate called to change the state of the cell
            /// \param [in,out] cl The cell
            std::function<void(cell & cl)> cycle;
            /// \brief Delegate called to move cargo on the cell
            /// \param [in,out] cl The cell
            std::function<void(cell & cl)> move;
            /// \brief Delegate called to draw the cell
            /// \param [in,out] cl The cell
            /// \param [in,out] im A variable that is to hold the representation of the cell
            std::function<void(cell & cl, image_t & im)> draw;

            /// \brief Delegate called when the cell is clicked in UI (or equivalent action)
            /// \param [in,out] cl The cell
            /// \param [in] pos Relative position of the click
            std::function<void(cell & cl, const ccoords_t & pos)> press;
            /// \brief Delegate called when a click on the cell is ended in UI (or equivalent action)
            /// \param [in,out] cl The cell
            /// \param [in] pos Relative position of the end of the click
            std::function<void(cell & cl, const ccoords_t & pos)> release;
            /// \brief Delegate called, when properties are changed in UIs
            /// \param [in, out] cl The cell
            /// \param [in] id ID of the changed property
            /// \param [in] prop Changed property
            std::function<void(cell & cl, of id, const property & prop)> regulate;
        } delegates;

        /// \brief Constructor
        /// \param [in] id Unique ID of the part
        /// \param [in] name Unique and friendly name of the part
        /// \param [in] traits General traits of the part
        explicit part(part_h id,
                      string_t & name,
                      traits_h traits = traits::EMPTY_PART);

        /// \brief Constructor
        /// \param [in] id Unique ID of the part
        /// \param [in] unique_name Unique name of the part
        /// \param [in] traits General traits of the part
        /// \param [in] friendly_name Friendly name of the part
        explicit part(part_h id = PART[0],
                      string_t unique_name = text(""),
                      traits_h traits = traits::EMPTY_PART,
                      string_t friendly_name = text(""));

        /// \brief Copy constructor
        /// \param [in] ref Reference to the original
        part(const part & ref);

        /// \brief Move constructor
        /// \param [in,out] fref Forwarding reference to the original
        part(part && fref) noexcept;

        /// \brief Returns the unique ID of this part
        /// \return The unique ID
        [[nodiscard]]
        part_h id() const;

        /// \brief Returns the unique name of this part
        /// \return The unique name
        [[nodiscard]]
        const string_t & unique_name() const;

        /// \brief Returns the friendly name of this part
        /// \return The friendly name
        [[nodiscard]]
        string_t & friendly_name();

        /// \brief Returns the friendly name of this part
        /// \return The friendly name
        [[nodiscard]]
        const string_t & friendly_name() const;

        /// \brief Returns thhe traits of this part
        /// \return The traits
        [[nodiscard]]
        traits_h traits() const;

        /// \brief Adds an entry to this part's property model
        /// \param [in] e New entry
        /// \return A reference to the inserted entry
        const entry & add_entry(const entry & e);

        /// \brief Adds an entry to this part's property model
        /// \param [in,out] e New entry
        /// \return A reference to the inserted entry
        const entry & add_entry(entry && e);

        template<typename... Args>
        const entry & add_entry(Args && ... args) {
            return add_entry(entry(std::forward<Args>(args)...));
        }

        void remove_entry(entry_h id);

        /// \brief Returns this part's property model
        /// \return This part's property model
        [[nodiscard]]
        const std::map<entry_h, entry> & model() const;

        /// \brief Adds a valid use for connection
        /// \param [in] dir Direction that corresponds to the connection
        /// \param [in] name Name for the connection use
        void add_connection_use(direction_t dir, string_t name);

        void add_connection_uses(std::initializer_list<std::pair<direction_t, string_t>> && uses);

        void remove_connection_use(direction_t use);

        /// \brief Gets a map of all defined connection uses
        /// \return The map of defined connection uses
        [[nodiscard]]
        const decltype(_conn_use) & connection_uses() const;

        // \brief Adds a property ID, which's change in this cell changes the appearance of the cell
        /// \param id ID of the property
        void add_visual(entry_h id);

        void add_visuals(std::initializer_list<entry_h> && ids);

        void remove_visual(entry_h id);

        /// \brief Gets a set of all waking property IDs
        /// \return Set of waking property IDs
        [[nodiscard]]
        const decltype(_visual) & visual() const;

        /// \brief Adds a property ID, which's change in neighbored and connected cells wakes up cells of this part
        /// \param id ID of the property
        void add_waking(entry_h id);

        void add_wakings(std::initializer_list<entry_h> && ids);

        void remove_waking(entry_h id);

        /// \brief Gets a set of all waking property IDs
        /// \return Set of waking property IDs
        [[nodiscard]]
        const decltype(_waking) & waking() const;

        /// \brief Initializes a cell with standard values
        /// \param [out] cell The cell
        void init_standard(cell_base & base) const;

        ///  \brief Invokes the init_static delegate
        /// \param [in,out] cl The cell
        void init_static(cell & cl) const;

        ///  \brief Invokes the init_relative delegate, or init_static if the former is undefined
        /// \param [in,out] cl The cell
        void init_relative(cell & cl) const;

        ///  \brief Invokes the clear delegate
        /// \param [in,out] cl The cell
        void clear(cell & cl) const;

        ///  \brief Invokes the cycle delegate
        /// \param [in,out] cl The cell
        void cycle(cell & cl) const;

        ///  \brief Invokes the move delegate
        /// \param [in,out] cl The cell
        void move(cell & cl) const;

        /// \brief Invokes the draw delegate
        /// \param [in,out] cl The cell
        /// \param [in,out] im A variable that is to hold the representation of the cell
        void draw(cell & cl, image_t & im) const;

        ///  \brief Invokes the press delegate
        //// \param [in,out] cl The cell
        /// \param [in] pos Relative position of the click
        void press(cell & cl, const ccoords_t & pos) const;

        ///  \brief Invokes the release delegate
        /// \param [in,out] cl The cell
        /// \param [in] pos Relative position of the end of the click
        void release(cell & cl, const ccoords_t & pos) const;

        /// \brief Delegate called, when properties are changed in UIs
        /// \param [in, out] cl The cell
        /// \param [in] id ID of the changed property
        /// \param [in] prop Changed property
        void regulate(cell & cl, of id, const property & prop) const;

        /// \brief Compares two parts for equal ID
        /// \param rhs Other part
        /// \return Equality of the part's IDs
        bool_t operator==(const part & rhs) const;

        /// \brief Compares two parts for unequal ID
        /// \param rhs Other part
        /// \return Inequality of the part's IDs
        bool_t operator!=(const part & rhs) const;

        /// \brief
        /// \param rhs
        /// \return
        bool_t operator<(const part & rhs) const;

        /// \brief
        /// \param rhs
        /// \return
        bool_t operator<=(const part & rhs) const;

        /// \brief
        /// \param rhs
        /// \return
        bool_t operator>(const part & rhs) const;

        /// \brief
        /// \param rhs
        /// \return
        bool_t operator>=(const part & rhs) const;

        /// \brief Copy assignment
        /// \param [in] rhs Refernce to the original
        /// \return The moved part
        part & operator=(const part & ref);

        /// \brief Move assignment
        /// \param [in,out] fref Forwarding reference to the original
        /// \return The moved part
        part & operator=(part && fref) noexcept;

        /// \brief Default destructor
        ~part();
    };

}

#endif //HAR_PART_HPP
