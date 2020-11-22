//
// Created by Johannes on 28.05.2020.
//

#pragma once

#ifndef HAR_PARTICIPANT_HPP
#define HAR_PARTICIPANT_HPP

#include <har/coords.hpp>
#include <har/full_cell.hpp>
#include <har/part.hpp>

namespace har {

    class inner_participant;

    static constexpr struct {
        constexpr participant_h operator[](const size_t n) const {
            return static_cast<participant_h>(n);
        }

        [[nodiscard]]
        constexpr participant_h no_one() const { //NOLINT
            return ~participant_h();
        }
    } PARTICIPANT = { };

    enum request_type {
        PROGRAM = 0,
        UI = 1
    };

    ///Participants are used for interaction with HAR simulations.
    class participant {
    public:
        class context;

        using callback_t = std::function<void(participant::context &)>;

    private:
        inner_participant * _iparti; ///<Pointer to inner participant (if attached)
        bool_t _destroying; ///<Whether the participant has begun detaching itself from a simulation

    protected:

        /// \brief Default constructor
        participant();

        /// \brief Requests operation on the simulation
        ///
        /// \return A context object
        context request(request_type req_type = request_type::PROGRAM);

        void exec(participant::callback_t && fun);

        /// \brief Starts the automaton
        void start();

        /// \brief Let's the automaton run for a single cycle
        void step();

        /// \brief Stops the automaton
        void stop();

        /// \brief Sets a grid cell as <tt>selected</tt> in the UI
        ///
        /// \param [in] hnd Handle of the cell
        ///
        /// All changes on the selected cell will be reported to the participant via callbacks
        void select(const cell_h & hnd);

        /// \brief Attempts to include a new part into the simulation
        ///
        /// \param [in] pt The part to include
        void include_part(const part & pt);

        /// \brief Loads a model from a string
        ///
        /// \param [in] ser A string that contains a serialized HAR model
        void load_model(string_t & ser);

        /// \brief Loads a model from a string
        ///
        /// \param [in] ser A string that contains a serialized HAR model
        void load_model(string_t && ser);

        /// \brief Loads a model from a string
        ///
        /// \param [in] is An input stream that supplies a serialized HAR model
        void load_model(istream & is);

        /// \brief Serializes the simulation's current model into a string
        ///
        /// \param [out] ser The target for the serialized model
        void store_model(string_t & ser);

        /// \brief Serializes the simulation's current model into an output stream
        ///
        /// \param [out] os The target for the serialized model
        void store_model(ostream & os);

        /// \brief Schedules a redraw of all cells in the current model
        void redraw_all();

        /// \brief Checks, if the participant is attached to a simulation
        [[nodiscard]]
        bool_t attached() const;

        /// \brief Detaches this participant from the simulation
        void detach();

        /// \brief Prompts the simulation process to exit gracefully
        void exit();

    public:

        /// \brief Called by the simulation to attach this participant to the simulation
        ///
        /// \param [in,out] iparti Reference to the corresponding inner participant
        /// \param [in] argc Number of command line arguments
        /// \param [in] argv Command line arguments
        /// \param [in] envp Environment variables
        void attach(inner_participant & iparti, int argc, char * argv[], char * envp[]);

        /// \brief Called by the simulation to detach this participant from the simulation
        ///
        /// \param [in,out] iparti Reference to the corresponding inner participant
        void detach(inner_participant & iparti);

        /// \brief Identifies a participant by name
        ///
        /// \return The name of the participant
        ///
        /// The name returned by this function is used in messages in a simulation to identify a participant
        [[nodiscard]]
        virtual std::string name() const = 0;

        /// \brief Retrieves a reference to the image of a participant displaying a cell
        ///
        /// \param [in] hnd Handle of the cell
        ///
        /// \return The participant's corresponding image
        [[nodiscard]]
        virtual image_t get_image_base(const cell_h & hnd) = 0;

        /// \brief Processes an image painted by a parts <tt>draw</tt> delegate for display in the participant
        ///
        /// \param [in] hnd Handle of the cell
        /// \param [in] img The cell's corresponding image
        ///
        /// \return The processed image
        virtual har::image_t process_image(har::cell_h hnd, har::image_t & img) = 0;

        /// \brief Retrieves the input stream of the participant
        ///
        /// \return Input stream of the participant
        virtual istream & input() = 0;

        /// \brief Retrieves the output stream of the participant
        ///
        /// \return Output stream of the participant
        virtual ostream & output() = 0;

        /// \brief Called once every cycle
        virtual void on_cycle(participant::context & ctx) = 0;

        /// \brief Called once, when the participant is added to the simulation
        /// This callback is guaranteed to be called before any other callback
        ///
        /// \param [in] argc Number of command line arguments
        /// \param [in] argv Array of command line arguments
        /// \param [in] envp Array of defined environment variables
        virtual void on_attach(int argc, char * const argv[], char * const envp[]) = 0;

        /// \brief Called every time a new part is added to the simulation
        ///
        /// \param [in] pt Added part
        /// \param [in] commit <tt>false</tt>, if more added parts will be reported subsequently, otherwise <tt>true</tt>
        virtual void on_part_included(const part & pt, bool_t commit) = 0;

        inline void on_part_included(const part & pt) {
            on_part_included(pt, true);
        }

        /// \brief Called every time a part is removed from the simulation
        ///
        /// \param [in] id ID of the removed part
        virtual void on_part_removed(part_h id) = 0;

        /// \brief Called every time a grid in the simulation is resized
        ///
        /// \param [in] to ID of the grid and its new size
        virtual void on_resize_grid(const gcoords_t & to) = 0;

        /// \brief Called every time a model is loaded
        virtual void on_model_loaded() = 0;

        /// \brief Called every time the information about the model is changed
        ///
        /// \param [in] info Updated model info
        virtual void on_info_updated(const model_info & info) = 0;

        /// \brief Called every time the automaton's state is set to <tt>RUN</tt>
        ///
        /// \param [in] responsible <tt>true</tt>, if the called participant is responsible for cycling the automaton
        virtual void on_run(bool_t responsible) = 0;

        inline void on_run() {
            on_run(false);
        }

        /// \brief Called every time the automaton's state is set to <tt>STEP</tt>
        virtual void on_step() = 0;

        /// \brief Called every time the automaton's state is set to <tt>STOP</tt>
        virtual void on_stop() = 0;

        /// \brief Called every time a message to the user is to be displayed
        ///
        /// \param [in] header Header of the message
        /// \param [in] content Content of the message
        virtual void on_message(const string_t & header, const string_t & content) = 0;

        /// \brief Called every time an exception occurs to report it to the user
        ///
        /// \param [in] e Reported exception
        virtual void on_exception(const exception::exception & e) = 0;

        /// \brief Called every time a property of the selected cell is updated
        ///
        /// \param [in] hnd Handle of the selected cell
        /// \param [in] id ID of the property
        /// \param [in] val New value
        virtual void on_selection_update(const cell_h & hnd, entry_h id, const value & val, bool_t commit) = 0;

        inline void on_selection_update(const cell_h & hnd, entry_h id, const value & val) {
            on_selection_update(hnd, id, val, true);
        }

        /// \brief Called every time a cell was redrawn
        ///
        /// \param [in] hnd Handle of the cell
        /// \param [in] img
        virtual void on_redraw(const cell_h & hnd, image_t && img, bool_t commit) = 0;

        inline void on_redraw(const cell_h & hnd, image_t && img) {
            on_redraw(hnd, std::forward<image_t>(img), true);
        }

        /// \brief Called when a connection is added to a grid cell
        ///
        /// \param [in] from Position of the observing end of the connection
        /// \param [in] to Position of the observed end of the connection
        /// \param [in] use Observer's ID for the connection
        virtual void on_connection_added(const gcoords_t & from, const gcoords_t & to, direction_t use) = 0;

        /// \brief Called when a connection is removed from a grid cell
        ///
        /// \param [in] from Position of the observing end of the connection
        /// \param [in] use Observer's ID for the connection
        virtual void on_connection_removed(const gcoords_t & from, direction_t use) = 0;

        /// \brief Called every time a new cargo is spawned
        ///
        /// \param [in] num ID of the cargo
        virtual void on_cargo_spawned(cargo_h num) = 0;

        /// \brief Called every time a new cargo is moved
        ///
        /// \param [in] num ID of the cargo
        /// \param [in] to New position of the cargo
        virtual void on_cargo_moved(cargo_h num, ccoords_t to) = 0;

        /// \brief Called every time a new cargo is destroyed
        ///
        /// \param [in] num ID of the cargo
        virtual void on_cargo_destroyed(cargo_h num) = 0;

        ///
        virtual void on_commit() = 0;

        /// \brief Called once, when the participant is detached from the simulation
        /// After this, no other callbacks will be called
        virtual void on_detach() = 0;

        /// \brief Default destructor
        virtual ~participant();
    };

    /// \brief Class for participants to operate on the simulation
    ///
    /// For the lifetime of an object of this class, all other threads of the associated simulation are halted
    class participant::context {
    private:
        std::reference_wrapper<inner_participant> _parti; ///<Reference to the creating participant's inner pendant
        request_type _type;
        bool_t _blocking; ///<Whether this context blocks the simulation

    public:

        /// \brief Constructor
        ///
        /// \param [in,out] parti Corresponding inner participant
        ///
        /// Waits for the automaton to yield
        explicit context(inner_participant & parti, request_type type, bool_t blocking = true);

        /// \brief Move constructor
        ///
        /// \param [in,out] fref Forwarding reference to original
        ///
        /// Takes over the referenced context
        context(context && fref) noexcept;

        /// \brief Retrieves a cell from the simulation's grid
        ///
        /// \param [in] pos Position of the cell
        ///
        /// \return The requested grid cell
        full_grid_cell at(const gcoords_t & pos);

        /// \brief Retrieves a cell of the simulation's cargo
        ///
        /// \param [in] num Number of the cargo
        ///
        /// \return The requested cargo cell
        full_cargo_cell at(cargo_h num);

        /// \brief Resizes one of the simulation's grids
        ///
        /// \param [in] to Grid number and new size
        void resize_grid(const gcoords_t & to);

        /// \brief Cycles the simulation for one step
        void cycle();

        /// \brief Commits all changes captured by this context before it's destruction
        ///
        /// This function has the same effects as the destruction of this object, without destroying it
        void commit();

        /// \brief Move assignment
        ///
        /// \param [in,out] fref Forwarding reference to original
        ///
        /// \return This context
        ///
        /// Takes over the referenced context
        context & operator=(context && fref) noexcept;

        /// \brief Default destructor
        ///
        /// Unlocks the automaton of the simulation
        ~context();
    };

}

#if defined(HAR_ENABLE_REQUEST_MACROS)
#define THISREQUEST(CTX) if (auto CTX = request(); true)
#define OBJREQUEST(CTX, OBJ) if (auto CTX = (OBJ).request(); true)
#define TYPEOBJREQUEST(CTX, OBJ, TYPE) if (auto CTX = (OBJ).request(TYPE); true)

#define GET_REQUEST_INNER(_1, _2, _3, NAME, ...) NAME
#define REQUEST(...) GET_REQUEST_INNER(__VA_ARGS__, TYPEOBJREQUEST, OBJREQUEST, THISREQUEST)(__VA_ARGS__)
#endif
#endif //HAR_PARTICIPANT_HPP
