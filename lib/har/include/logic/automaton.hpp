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
#include "world/grid_cell_base.hpp"

namespace har {

    enum process : ushort_t {
        CYCLE = 1u << 0u,
        MOVE = 1u << 1u
    };

    struct process_tab {
    private:
        std::map<cell_h, std::tuple<std::reference_wrapper<cell_base>, process>> _active;
        std::map<cell_h, std::reference_wrapper<cell_base>> _waking;
        std::map<cell_h, std::reference_wrapper<cell_base>> _tiring;
        std::map<cell_h, std::reference_wrapper<cell_base>> _starting;
        std::map<cell_h, std::reference_wrapper<cell_base>> _halting;
        std::map<cell_h, std::reference_wrapper<cell_base>> _inactive;

    public:
        process_tab();

        void wake(const cell_h & hnd, cell_base & clb);

        void tire(const cell_h & hnd, cell_base & clb);

        void start(const cell_h & hnd, cell_base & clb);

        void halt(const cell_h & hnd, cell_base & clb);

        void apply();

        void remove(const cell_h & hnd);

        const decltype(_active) & get_active() const;

        const decltype(_waking) & get_waking() const;

        const decltype(_tiring) & get_tiring() const;

        const decltype(_starting) & get_starting() const;

        const decltype(_halting) & get_halting() const;

        const decltype(_inactive) & get_inactive() const;

        ~process_tab();
    };

    class automaton {
    public:
        enum class state;

    private:
        class dispatcher;

        class worker;

        enum class substep;

        inner_simulation & _sim;

        std::atomic<state> _state; ///<State of the automaton
        std::atomic<substep> _substep; ///<Current substep

        const uint_t _threads; ///<Number of threads
        std::vector<std::unique_ptr<worker>> _workers; ///<Contains worker threads and their data

        std::atomic<uint_t> _waiting;
        std::mutex _cyclex;
        std::mutex _autoex;

        co_queue<std::pair<participant_h, participant::callback_t>> _queue;

        std::atomic<ushort_t> _workdone; ///<Counter for how many workers have finished their current task
        std::atomic_flag _first; ///<Flag for the first worker to have completed it's task

        std::chrono::microseconds _time_delta;
        std::chrono::time_point<clock> _last_cycle_begin;

        void i_am_done();

        void wait_for_all();

        void inner_exec(std::pair<participant_h, participant::callback_t> & fun);

    public:
        explicit automaton(inner_simulation & sim);

        automaton(inner_simulation & sim, ushort_t workers);

        state state();

        substep substep();

        enum state set_state(enum state to);

        std::mutex & get_autoex();

        void request(participant_h id);

        void exec(participant_h id, participant::callback_t && fun);

        void process(inner_participant & iparti);

        bool_t begin(bool_t and_block);

        bool_t end(bool_t and_unblock);

        void cycle();

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
        automaton & _auto;
        std::mutex _workex;
        context _ctx;

        void work();

        void begin();

        void done();

        void process_grid(grid & grid);

        void process_cargo(world & world);

        void context_commit_and_draw(context & ctx);

        void wait_for_next_step();

    public:
        const ushort_t offset;

        std::thread _thread{ };
        std::atomic_flag _valid{ true };

        explicit worker(automaton & automaton, ushort_t id = 0u);

        void start();

        void process_single_request(inner_participant & iparti);

        void process_exec(std::pair<participant_h, participant::callback_t> & pack);

        bool_t process_requests();

        void cycle_and_move();

        void cycle_commit_and_draw();

        void clean();

        void unblock();

        ~worker();
    };

    enum class automaton::state {
        INIT,
        RUN,
        STEP,
        STOP
    };

    enum class automaton::substep {
        INIT,
        WAITING,
        PROCESS_REQUEST,
        CYCLE_AND_MOVE,
        COMMIT_AND_DRAW,
        CLEAN
    };

}

#endif //HAR_AUTOMATON_HPP
