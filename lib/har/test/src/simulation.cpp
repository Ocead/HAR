//
// Created by Johannes on 26.05.2020.
//

#include <har/program.hpp>
#include <har/simulation.hpp>

#include "logic/inner_simulation.hpp"

#include <catch2/catch.hpp>

using namespace har;

TEST_CASE("Custom parts", "[simulation][part]") {
    simulation sim{ };

    SECTION("New parts can be added to a simulation") {
        part pt{ PART[5] };

        REQUIRE_NOTHROW(sim.include_part(pt));
        REQUIRE(sim.part_of(PART[5]) == pt);
    }

    SECTION("Included parts can be replaced") {
        part pt1{ PART[5], "gnampf" };
        part pt2{ PART[5], "fnampf" };

        sim.include_part(pt1);
        REQUIRE_NOTHROW(sim.include_part(pt2));
        REQUIRE(sim.part_of(PART[5]).unique_name() == pt2.unique_name());
    }

    SECTION("Standard parts can be added to a simulation") {
        FAIL("Not implemented");
    }

    SECTION("Parts can be removed from a simulation") {
        FAIL("Not implemented");
    }
}

TEST_CASE("Models", "[!mayfail][world][cargo_cell_base][grid_cell_base][parser]") {
    SECTION("The simulation can deserialize models") {
        FAIL("Not implemented");
    }

    SECTION("The simulation can load models") {
        FAIL("Not implemented");
    }

    SECTION("The simulation checks the for the parts required by a model") {
        FAIL("Not implemented");
    }

    SECTION("When moved in memory, models remain intact") {
        FAIL("Not implemented");
    }
}

TEST_CASE("Participants", "[simulation][participant]") {
    inner_simulation & isim = *new inner_simulation{ 0, nullptr, nullptr };
    simulation sim{ isim };
    program prog{ };

    SECTION("Participants can be added to the simulation") {
        participant_h hnd;
        REQUIRE_NOTHROW(hnd = sim.attach(prog));
        REQUIRE(isim.participants().size() == 1);
        REQUIRE(isim.inner_participants().size() == 1);
        REQUIRE_NOTHROW(isim.inner_participants().at(hnd));

        REQUIRE(isim.participants().at(hnd) == &prog);
    }

    SECTION("Participants can halt the simulation") {
        sim.attach(prog);
        prog.stop();
        REQUIRE(isim.get_automaton().state() == automaton::state::STOP);
    }

    SECTION("Participants can resume the simulation") {
        sim.attach(prog);
        prog.start();
        REQUIRE(isim.get_automaton().state() == automaton::state::RUN);
    }

    SECTION("Participants can be removed from the simulation") {
        sim.attach(prog);
        REQUIRE_NOTHROW(prog.detach());
        REQUIRE(isim.participants().empty());
        REQUIRE(isim.inner_participants().empty());
    }

    SECTION("Participants exit the simulation") {
        bool_t called = false;
        isim.call_on_exit([&]() {
            called = true;
        });
        sim.attach(prog);
        REQUIRE_NOTHROW(prog.exit());
        REQUIRE(called);
    }
}
