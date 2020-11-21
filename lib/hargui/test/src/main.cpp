//
// Created by Johannes on 28.06.2020.
//

#define EVER ;;

#include <chrono>

#include <har.hpp>

#include "har/gui.hpp"

using namespace std::chrono;
using namespace har;
using namespace har::gui_;

int main(int argc, char * argv[]) {
    simulation sim{ };
    gui gui;
    program p1{ };
    program p2{ };

    sim.attach(gui);
    sim.attach(p1);
    sim.attach(p2);

    auto pt = ::har::part{ PART[0] };

    sim.include_part(pt);

    for (EVER) {
        std::this_thread::sleep_for(1s);
    }
}
