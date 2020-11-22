//
// Created by Johannes on 25.06.2020.
//

#pragma once

#ifndef HAR_INNER_PARTICIPANT_HPP
#define HAR_INNER_PARTICIPANT_HPP

#include <deque>
#include <mutex>


#include <har/participant.hpp>
#include <har/full_cell.hpp>

#include "logic/automaton.hpp"
#include "logic/context.hpp"
#include "world/cargo_cell_base.hpp"
#include "world/grid_cell_base.hpp"
#include "world/model.hpp"

namespace har {
    class asymmetric_lock {
    private:
        std::mutex _mutex;
        std::atomic<bool_t> _waiting;
        std::reference_wrapper<std::mutex> _inverse;

    public:
        explicit asymmetric_lock(std::mutex & inverse);

        void request();

        void grant();

        void hand_back();

        [[nodiscard]]
        bool_t waiting() const;

        ~asymmetric_lock();
    };

    class inner_participant {
    private:
        participant_h _id;

        context _ctx;

        std::reference_wrapper<inner_simulation> _simulation;
        std::reference_wrapper<automaton> _automaton;
        std::reference_wrapper<model> _model;

        std::atomic<bool_t> _do_cycle;
        std::atomic<bool_t> _do_draw;

        cell_h _selected;

        asymmetric_lock _alock;

    public:
        explicit inner_participant(participant_h id, inner_simulation & simulation);

        [[nodiscard]]
        participant_h id() const;

        [[nodiscard]]
        context & get_context();

        [[nodiscard]]
        const class context & get_context() const;

        [[nodiscard]]
        const cell_h & get_selected() const;

        [[nodiscard]]
        bool_t do_cycle() const;

        bool_t do_cycle(bool_t cycle);

        [[nodiscard]]
        bool_t do_draw() const;

        bool_t do_draw(bool_t draw);

        [[nodiscard]]
        bool_t has_request() const;

        void cycle();

        void commit();

        void unlock_automaton_and_commit(request_type type);

        void wait_for_automaton(request_type type);

        void unlock_and_wait_for_request();

        participant::context request(request_type req_type = request_type::PROGRAM);

        void exec(std::function<void(participant::context &)> && fun);

        full_grid_cell at(const gcoords_t & pos);

        full_cargo_cell at(cargo_h num);

        void select(const cell_h & hnd);

        void include_part(const part & pt);

        void load_model(string_t & ser);

        void load_model(string_t && ser);

        void load_model(istream & is);

        void store_model(string_t & ser);

        void store_model(ostream & os);

        void resize_grid(const gcoords_t & to);

        void redraw_all();

        void start();

        void step();

        void stop();

        void detach();

        void exit();

        ~inner_participant();
    };
}

#endif //HAR_INNER_PARTICIPANT_HPP
