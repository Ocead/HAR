//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_AUTOMATON_HPP
#define HAR_AUTOMATON_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <vector>

#include <har/co_queue.hpp>
#include <har/participant.hpp>
#include <har/types.hpp>

#include "logic/context.hpp"
#include "logic/process_tab.hpp"
#include "world/grid_cell_base.hpp"

namespace har {

    class automaton {
    public:
        enum class state;

    private:
        class dispatcher;

        class worker;

        enum class substep;

        inner_simulation & _sim; ///<Associated simulation

        std::atomic<state> _state; ///<State of the automaton
        std::atomic<substep> _substep; ///<Current substep

        const uint_t _threads; ///<Number of threads
        std::vector<std::unique_ptr<worker>> _workers; ///<Contains worker threads and their data

        std::atomic<uint_t> _waiting; ///<Barrier counter for worker threads
        std::mutex _cyclex;
        std::mutex _autoex;

        process_tab _tab;

        co_queue<std::pair<participant_h, participant::callback_t>> _queue;

        std::atomic<ushort_t> _workdone; ///<Counter for how many workers have finished their current task
        std::atomic_flag _first; ///<Flag for the first worker to have completed it's task

        std::chrono::microseconds _time_delta; ///<The minimal time interval between cycles
        std::chrono::time_point<clock> _last_cycle_begin; ///<The point in time the last cycle started

        /// \brief Called by workers when their assigned task is finished
        void i_am_done();

        /// \brief Waits for all worker threads to finish their assigned tasks
        void wait_for_all();

        void inner_exec(std::pair<participant_h, participant::callback_t> & fun);

    public:
        /// \brief Constructor
        ///
        /// \param [in] sim Associated simulation
        explicit automaton(inner_simulation & sim);

        /// \brief Constructor
        ///
        /// \param [in] sim Associated simulation
        /// \param [in] workers Number of worker threads
        automaton(inner_simulation & sim, ushort_t workers);

        /// \brief Returns the current state of the automaton
        /// \return The automaton's current state
        state state();

        /// \brief Returns the current substep the automaton is in
        /// \return The automaton's current substep
        substep substep();

        /// \brief Sets the automatons state
        ///
        /// \param [in] to New state
        ///
        /// \return Old state
        enum state set_state(enum state to);

        process_tab & get_tab();

        std::mutex & get_autoex();

        void resize_tab(const gcoords_t & from, const gcoords_t & to);

        void request(participant_h id);

        void exec(participant_h id, participant::callback_t && fun);

        void process(inner_participant & iparti);

        /// \brief Blocks the automaton from exclusive operation
        /// \param [in] and_block <tt>TRUE</tt>, if the calling th
        /// \return <tt>TRUE</tt>, if the
        bool_t begin(bool_t and_block);

        /// \brief Unblocks the automaton for exclusive operation
        /// \param [in] and_unblock
        /// \return
        bool_t end(bool_t and_unblock);

        void cycle();

        /// \brief Standard destructor
        ~automaton();
    };

    class automaton::dispatcher {
    private:
        automaton & _auto;
        grid & _model;
        grid & _bank;
        const uint_t _interval;

    public:
        dispatcher(automaton & automaton, uint_t interval);

        std::tuple<cell_h, std::reference_wrapper<cell_base>> get_my_nth(uint_t num, uint_t offset);

        ~dispatcher();
    };

   class automaton::worker {
    private:
        automaton & _auto; ///<Underlying automaton
        std::mutex _workex; ///<Lock the worker thread waits on
        context _ctx; ///<The workers context

        /// \brief Entry function for the worker threads
        void work();

        /// \brief Begins the task assigned by the automaton
        void begin();

        /// \brief Reports finishing the assigned task to the automaton
        void done();

        /// \brief Cycles all grid cells in the grid without committing
        void process_grid(grid & grid);

        /// \brief Moves all cargo cells (if applicable) without committing
        void process_cargo(world & world);

        /// \brief Commits all changes to cells in the context and emit draw callbacks (if applicable)
        void context_commit_and_draw(context & ctx);

        void wait_for_next_step();

    public:
        const ushort_t offset; ///<The serial number of the worker thread and it's offset for working on cells

        std::thread _thread{ }; ///<The thread that an object of this class acts as closure for
        std::atomic_flag _valid{ true }; ///<<tt>TRUE</tt>,if the thread should continue working

        /// \brief Constructor
        /// \param automaton The underlying automaton
        /// \param id The serial number for this worker
        explicit worker(automaton & automaton, ushort_t id = 0u);

        /// Starts a thread for this worker
        void start();

        void process_single_request(inner_participant & iparti);

        void process_exec(std::pair<participant_h, participant::callback_t> & pack);

        bool_t process_requests();

        void cycle_and_move();

        void cycle_commit_and_draw();

        /// \brief Resets the worker's context
        void clean();

        /// \brief Unblocks the worker thread to continue work
        void unblock();

        /// \brief Standard destructor
        ~worker();
    };

    enum class automaton::state {
        INIT, ///<Simulation hasn't commenced yet
        RUN,  ///<Automaton is running cyclically
        STEP, ///<Automaton is on last cycle before stopping
        STOP  ///<Automaton is stopped
    };

    enum class automaton::substep {
        INIT,            ///<Simulation hasn't commenced yet
        WAITING,         ///<Automaton is waiting
        PROCESS_REQUEST, ///<Automaton processes a request from a participant
        CYCLE_AND_MOVE,  ///<Automaton calculates the cycle
        COMMIT_AND_DRAW, ///<Automaton commits changes from the cycle
        CLEAN            ///<Automaton is cleaning up the cycle
    };

}

#endif //HAR_AUTOMATON_HPP
