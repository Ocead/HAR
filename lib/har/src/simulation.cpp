//
// Created by Johannes on 02.07.2020.
//

#include <har/simulation.hpp>

#include "logic/inner_simulation.hpp"

using namespace har;

simulation::simulation(int argc, char * argv[], char * envp[]) : _isim(std::make_unique<inner_simulation>(argc, argv, envp)) {

}

simulation::simulation(inner_simulation & isim) : _isim(&isim) {

}

simulation::simulation(simulation && fref) noexcept = default;

void simulation::include_part(const part & pt) {
    _isim->include_part(pt);
}

const part & simulation::part_of(part_h id) const {
    return _isim->part_of(id);
}

void simulation::call_on_exit(const std::function<void()> & fun) {
    _isim->call_on_exit(fun);
}

void simulation::call_on_exit(std::function<void()> && fun) {
    _isim->call_on_exit(std::forward<std::function<void()>>(fun));
}

participant_h simulation::attach(participant & par) {
    return _isim->attach(par);
}

void simulation::datach(participant_h id) {
    _isim->detach(id);
}

void simulation::commence() {
    _isim->commence();
}

simulation & simulation::operator=(simulation && fref) noexcept = default;

simulation::~simulation() = default;
