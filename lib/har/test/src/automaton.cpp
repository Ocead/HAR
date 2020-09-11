//
// Created by Johannes on 10.06.2020.
//

#include <har/program.hpp>

#include "logic/automaton.hpp"
#include "logic/inner_simulation.hpp"

#include <catch2/catch.hpp>

using namespace har;

TEST_CASE("Automaton", "[!mayfail][automaton]") {
    inner_simulation isim{ 0, nullptr, nullptr };
    automaton & automaton = isim.get_automaton();
    program prog{ };

    isim.attach(prog);

    isim.commence();

    SECTION("The automaton can be started") {
        REQUIRE_NOTHROW(prog.start());
        REQUIRE(automaton.state() == automaton::state::RUN);
    }

    SECTION("The automaton can be stopped") {
        REQUIRE_NOTHROW(prog.stop());
        REQUIRE(automaton.state() == automaton::state::STOP);
    }

    SECTION("The automaton can execute single steps") {
        REQUIRE_NOTHROW(prog.step());
        REQUIRE(automaton.state() == automaton::state::STEP);
        //TODO: Implement automaton::cycle
        //automaton.cycle();
        REQUIRE(automaton.state() == automaton::state::STOP);
    }

    SECTION("The automaton can keep tabs on how to process single cells") {
        part pt{ PART[0] };
        cell_h hnd{ gcoords_t() };
        cell_base clb{ pt };

        process_tab tab{ };

        SECTION("Tab on cell can be added") {
            REQUIRE_NOTHROW(tab.tire(hnd, clb));
            REQUIRE_NOTHROW(tab.get_tiring().at(hnd));
            tab.apply();

            REQUIRE(tab.get_active().empty());
            REQUIRE(tab.get_inactive().size() == 1);
            REQUIRE_THROWS(tab.get_active().at(hnd));
            REQUIRE(&tab.get_inactive().at(hnd).get() == &clb);
        }

        SECTION("Tabs can be applied") {
            tab.wake(CARGO[0], clb);
            tab.tire(CARGO[1], clb);
            tab.start(CARGO[2], clb);
            tab.halt(CARGO[3], clb);
            REQUIRE_NOTHROW(tab.apply());

            REQUIRE(tab.get_active().size() == 2);
            REQUIRE(tab.get_inactive().size() == 2);
            REQUIRE(tab.get_waking().empty());
            REQUIRE(tab.get_tiring().empty());
            REQUIRE(tab.get_starting().empty());
            REQUIRE(tab.get_halting().empty());
        }

        SECTION("Tab on cell can be set to \"cycling\"") {
            tab.tire(CARGO[0], clb);
            tab.start(CARGO[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.wake(CARGO[0], clb));
            REQUIRE_NOTHROW(tab.wake(CARGO[1], clb));
            tab.apply();

            REQUIRE(std::get<1>(tab.get_active().at(CARGO[0])) == process::CYCLE);
            REQUIRE(std::get<1>(tab.get_active().at(CARGO[1])) == static_cast<process>(process::CYCLE | process::MOVE));
        }

        SECTION("Tab on cell can be set to \"sleeping\"") {
            tab.wake(CARGO[0], clb);
            tab.wake(CARGO[1], clb);
            tab.start(CARGO[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.tire(CARGO[0], clb));
            REQUIRE_NOTHROW(tab.tire(CARGO[1], clb));
            tab.apply();

            REQUIRE_THROWS(std::get<1>(tab.get_active().at(CARGO[0])));
            REQUIRE(std::get<1>(tab.get_active().at(CARGO[1])) == process::MOVE);
        }

        SECTION("Tab on cell can be set to \"moving\"") {
            tab.halt(CARGO[0], clb);
            tab.wake(CARGO[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.start(CARGO[0], clb));
            REQUIRE_NOTHROW(tab.start(CARGO[1], clb));
            tab.apply();

            REQUIRE(std::get<1>(tab.get_active().at(CARGO[0])) == process::MOVE);
            REQUIRE(std::get<1>(tab.get_active().at(CARGO[1])) == static_cast<process>(process::CYCLE | process::MOVE));
        }

        SECTION("Tab on cell can be set to \"halting\"") {
            tab.start(CARGO[0], clb);
            tab.start(CARGO[1], clb);
            tab.wake(CARGO[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.halt(CARGO[0], clb));
            REQUIRE_NOTHROW(tab.halt(CARGO[1], clb));
            tab.apply();

            REQUIRE_THROWS(std::get<1>(tab.get_active().at(CARGO[0])));
            REQUIRE(std::get<1>(tab.get_active().at(CARGO[1])) == process::CYCLE);
        }
    }

    SECTION("A cycle updates every cell") {
        FAIL("Not implemented");
    }

    SECTION("The automaton can be interrupted by requests from programs") {
        FAIL("Not implemented");
    }

    SECTION("The automaton can be interrupted by requests from user interfaces") {
        FAIL("Not implemented");
    }

    SECTION("The time period for cycles can be changed") {
        FAIL("Not implemented");
    }

    SECTION("The ratio of programs requests per cycle can be set") {
        FAIL("Not implemented");
    }
}
