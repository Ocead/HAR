//
// Created by Johannes on 29.06.2020.
//

#pragma once

#ifndef HAR_INNER_SIMULATION_HPP
#define HAR_INNER_SIMULATION_HPP

#include <har/exception.hpp>
#include <har/participant.hpp>

#include "logic/automaton.hpp"
#include "logic/inner_participant.hpp"
#include "world/model.hpp"

namespace har {

    namespace exception {
        class duplicate_participant : public exception {
        private:
            const participant_h _id;
            const participant & _original;
            const participant & _duplicate;

        public:
            duplicate_participant(const std::string & source,
                                  participant_h id,
                                  const participant & original,
                                  const participant & duplicate);

            [[nodiscard]]
            participant_h id() const;

            [[nodiscard]]
            const participant & original() const;

            [[nodiscard]]
            const participant & duplicate() const;

            ~duplicate_participant() override;
        };
    }

    class inner_simulation {
    private:
        std::map<part_h, part> _inventory;
        participant_h _particnt;
        map<participant_h, inner_participant *> _ipartis;
        map<participant_h, participant *> _partis;

        automaton _automaton;
        model _model;

        int _argc;
        char ** _argv;
        char ** _envp;

        std::function<void()> _exit_fun;

    public:
        inner_simulation(int argc, char * argv[], char * envp[]);

        automaton & get_automaton();

        const class automaton & get_automaton() const;

        model & get_model();

        const class model & get_model() const;

        const decltype(_inventory) & inventory() const;

        void include_part(const part & pt);

        void remove_part(part_h id);

        participant_h attach(participant & parti);

        void detach(participant_h id);

        decltype(_partis) & participants();

        const decltype(_partis) & participants() const;

        decltype(_ipartis) & inner_participants();

        const decltype(_ipartis) & inner_participants() const;

        void call_on_exit(const std::function<void()> & fun);

        void call_on_exit(std::function<void()> && fun);

        const part & part_of(part_h id) const;

        void load_model(context & ctx, istream & is);

        void store_model(ostream & os);

        void send_message(const string_t & header, const string_t & content);

        void commence();

        void exit();

        ~inner_simulation();
    };
}

#endif //HAR_INNER_SIMULATION_HPP
