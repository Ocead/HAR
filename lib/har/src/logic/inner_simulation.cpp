//
// Created by Johannes on 02.07.2020.
//

#include "logic/inner_simulation.hpp"

using namespace har;

//region duplicate_participant

exception::duplicate_participant::duplicate_participant(const std::string & source,
                                                        participant_h id,
                                                        const participant & original,
                                                        const participant & duplicate) : exception(source,
                                                                                                   "duplicate_participant in " +
                                                                                                   source +
                                                                                                   ":\n\tparticipant with id PARTICIPANT[" +
                                                                                                   std::to_string(id) +
                                                                                                   "] already registered as" +
                                                                                                   original.name()),
                                                                                         _id(id),
                                                                                         _original(original),
                                                                                         _duplicate(duplicate) {

}

participant_h exception::duplicate_participant::id() const {
    return _id;
}

const participant & exception::duplicate_participant::original() const {
    return _original;
}

const participant & exception::duplicate_participant::duplicate() const {
    return _duplicate;
}

exception::duplicate_participant::~duplicate_participant() = default;

//endregion

//region inner_simulation

inner_simulation::inner_simulation(int argc, char * argv[], char * envp[]) : _inventory(),
                                                                             _particnt(0),
                                                                             _ipartis(),
                                                                             _partis(),
                                                                             _automaton(*this),
                                                                             _model(*this),
                                                                             _argc(argc),
                                                                             _argv(argv),
                                                                             _envp(envp),
                                                                             _exit_fun([]() { std::exit(0); }) {
    _inventory.try_emplace(PART[0]);
}

automaton & inner_simulation::get_automaton() {
    return _automaton;
}

const class automaton & inner_simulation::get_automaton() const {
    return _automaton;
}

model & inner_simulation::get_model() {
    return _model;
}

const class model & inner_simulation::get_model() const {
    return _model;
}

const decltype(inner_simulation::_inventory) & inner_simulation::inventory() const {
    return _inventory;
}

void inner_simulation::include_part(const part & pt) {
    auto[it, ins] = _inventory.insert_or_assign(pt.id(), pt);

    if (!ins) {
        for (auto & p : _partis) {
            auto & ptr = std::get<1>(p);
            ptr->on_part_removed(pt.id());
        }
    }
    auto & ipt = std::get<1>(*it);
    for (auto & p : _partis) {
        std::get<1>(p)->on_part_included(ipt, true);
    }
}

void inner_simulation::remove_part(part_h id) {
    _model.purge_part(_inventory.at(id), _inventory.at(PART[0]));
    auto rem = _inventory.erase(id);
    if (rem) {
        for (auto & p : _partis) {
            std::get<1>(p)->on_part_removed(id);
        }
    }
}

participant_h inner_simulation::attach(participant & parti) {
    auto[it, ok] = _ipartis.try_emplace(_particnt, new inner_participant(_particnt, *this));

    if (ok) {
        parti.attach(*it->second, _argc, _argv, _envp);
        _partis.emplace(_particnt, &parti);

        switch (_automaton.state()) {
            case automaton::state::RUN: {
                parti.on_run();
                break;
            }
            case automaton::state::STEP: {
                parti.on_step();
                break;
            }
            case automaton::state::STOP: {
                parti.on_stop();
                break;
            }
            default: {
                break;
            }
        }
        for (auto & pt : _inventory) {
            parti.on_part_included(std::get<1>(pt), true);
        }
        parti.on_resize_grid(gcoords_t{ grid_t::MODEL_GRID, _model.get_model().dim() });
        parti.on_resize_grid(gcoords_t{ grid_t::BANK_GRID, _model.get_bank().dim() });
        parti.on_model_loaded();
        parti.on_info_updated(_model.info());

    } else {
        raise(*new exception::duplicate_participant("simulation::attach",
                                                    _particnt,
                                                    *_partis.at(_particnt),
                                                    parti));
    }
    return _particnt++;
}

void inner_simulation::detach(participant_h id) {
    auto node = _partis.extract(id);
    auto inode = _ipartis.extract(id);
    node.mapped()->detach(*inode.mapped());
}

decltype(inner_simulation::_partis) & inner_simulation::participants() {
    return _partis;
}

const decltype(inner_simulation::_partis) & inner_simulation::participants() const {
    return _partis;
}

decltype(inner_simulation::_ipartis) & inner_simulation::inner_participants() {
    return _ipartis;
}

const decltype(inner_simulation::_ipartis) & inner_simulation::inner_participants() const {
    return _ipartis;
}

void inner_simulation::call_on_exit(const std::function<void()> & fun) {
    _exit_fun = fun;
}

void inner_simulation::call_on_exit(std::function<void()> && fun) {
    _exit_fun = std::move(fun);
}

const part & inner_simulation::part_of(part_h id) const {
    return _inventory.at(id);
}

void inner_simulation::load_model(context & ctx, istream & is) {
    model _new_model{ *this };
    bool_t ok;
    is >> std::tie(_new_model, ok);
    if (ok) {
        _model = std::move(_new_model);

        for (auto & p : _partis) {
            p.second->on_resize_grid(gcoords_t{ grid_t::MODEL_GRID, _model.get_model().dim() });
            p.second->on_resize_grid(gcoords_t{ grid_t::BANK_GRID, _model.get_bank().dim() });
            p.second->on_model_loaded();
            p.second->on_info_updated(_model.info());
        }
    } else {
        send_message("Error", "Read in model couldn't be loaded. Source contains errors");
    }
}

void inner_simulation::store_model(ostream & os) {
    os << _model;
}

void inner_simulation::send_message(const string_t & header, const string_t & content) {
    for (auto &[id, parti] : _partis) {
        parti->on_message(header, content);
    }
}

void inner_simulation::commence() {
    _automaton.set_state(automaton::state::STOP);
}

void inner_simulation::exit() {
    _automaton.begin(true);
    while (!_ipartis.empty()) {
        auto ptr = _ipartis.begin()->second;
        ptr->detach();
    }
    _automaton.end(true);
    debug_log("Calling on exit function");
    _exit_fun();
}

inner_simulation::~inner_simulation() {
    while (!_ipartis.empty()) {
        auto ptr = _ipartis.begin()->second;
        ptr->detach();
    };
}

//endregion
