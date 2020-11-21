//
// Created by Johannes on 10.06.2020.
//

#define HAR_ENABLE_REQUEST_MACROS

#include <har/program.hpp>

#include "logic/automaton.hpp"
#include "logic/inner_simulation.hpp"

#include "world/grid.hpp"

#include <catch2/catch.hpp>

using namespace har;

TEST_CASE("Automaton", "[!mayfail][automaton]") {
    inner_simulation isim{ 0, nullptr, nullptr };
    automaton & automaton = isim.get_automaton();
    program prog{ };

    isim.attach(prog);

    SECTION("The automaton can be started") {
        isim.commence();

        REQUIRE_NOTHROW(prog.start());
        REQUIRE(automaton.state() == automaton::state::RUN);
    }

    SECTION("The automaton can be stopped") {
        isim.commence();

        REQUIRE_NOTHROW(prog.stop());
        REQUIRE(automaton.state() == automaton::state::STOP);
    }

    SECTION("The automaton can execute single steps") {
        isim.commence();

        REQUIRE_NOTHROW(prog.step());
        REQUIRE(automaton.state() == automaton::state::STEP);
        automaton.cycle();
        REQUIRE(automaton.state() == automaton::state::STOP);
    }

    SECTION("The automaton can keep tabs on how to process single cells") {
        part pt{ PART[0] };
        gcoords_t pos{ };
        cell_base clb{ pt };
        gcoords_t posa[4]{
                gcoords_t(grid_t::INVALID_GRID, 0, 0),
                gcoords_t(grid_t::INVALID_GRID, 0, 1),
                gcoords_t(grid_t::INVALID_GRID, 0, 2),
                gcoords_t(grid_t::INVALID_GRID, 0, 3)
        };

        auto & tab = automaton.get_tab();

        SECTION("Tab on cell can be added") {
            REQUIRE_NOTHROW(tab.tire(pos, clb));
            REQUIRE_NOTHROW(tab.get_tiring().at(pos));
            tab.apply();

            REQUIRE(tab.get_active().empty());
            REQUIRE(tab.get_inactive().size() == 1);
            REQUIRE_THROWS(tab.get_active().at(pos));
            REQUIRE(&tab.get_inactive().at(pos).get() == &clb);
        }

        SECTION("Tabs can be applied") {
            tab.wake(posa[0], clb);
            tab.tire(posa[1], clb);
            tab.start(posa[2], clb);
            tab.halt(posa[3], clb);
            REQUIRE_NOTHROW(tab.apply());

            REQUIRE(tab.get_active().size() == 2);
            REQUIRE(tab.get_inactive().size() == 2);
            REQUIRE(tab.get_waking().empty());
            REQUIRE(tab.get_tiring().empty());
            REQUIRE(tab.get_starting().empty());
            REQUIRE(tab.get_halting().empty());
        }

        SECTION("Tab on cell can be set to \"cycling\"") {
            tab.tire(posa[0], clb);
            tab.start(posa[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.wake(posa[0], clb));
            REQUIRE_NOTHROW(tab.wake(posa[1], clb));
            tab.apply();

            REQUIRE(tab.get_active().at(posa[0]).status == process::CYCLE);
            REQUIRE(tab.get_active().at(posa[1]).status == (process::CYCLE | process::MOVE));
        }

        SECTION("Tab on cell can be set to \"sleeping\"") {
            tab.wake(posa[0], clb);
            tab.wake(posa[1], clb);
            tab.start(posa[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.tire(posa[0], clb));
            REQUIRE_NOTHROW(tab.tire(posa[1], clb));
            tab.apply();

            REQUIRE_THROWS(tab.get_active().at(posa[0]));
            REQUIRE(tab.get_active().at(posa[1]).status == process::MOVE);
        }

        SECTION("Tab on cell can be set to \"moving\"") {
            tab.halt(posa[0], clb);
            tab.wake(posa[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.start(posa[0], clb));
            REQUIRE_NOTHROW(tab.start(posa[1], clb));
            tab.apply();

            REQUIRE(tab.get_active().at(posa[0]).status == process::MOVE);
            REQUIRE(tab.get_active().at(posa[1]).status == (process::CYCLE | process::MOVE));
        }

        SECTION("Tab on cell can be set to \"halting\"") {
            tab.start(posa[0], clb);
            tab.start(posa[1], clb);
            tab.wake(posa[1], clb);
            tab.apply();

            REQUIRE_NOTHROW(tab.halt(posa[0], clb));
            REQUIRE_NOTHROW(tab.halt(posa[1], clb));
            tab.apply();

            REQUIRE_THROWS(tab.get_active().at(posa[0]));
            REQUIRE(tab.get_active().at(posa[1]).status == process::CYCLE);
        }

        SECTION("Tab keeps up with model expansion") {
            gcoords_t to{ grid_t::MODEL_GRID, 2, 2 };
            REQUEST(ctx, prog) {
                ctx.resize_grid(to);
            }
            auto & active = tab.get_active();
            for (dcoords_t xy{ }; xy.in(to.pos); xy.rectangle(to.pos)) {
                REQUIRE_NOTHROW(active.at(gcoords_t(grid_t::MODEL_GRID, xy)));
            }
            REQUIRE(tab.size() == to.pos.x * to.pos.y);
        }

        SECTION("Tab keeps up with model diminution") {
            gcoords_t from{ grid_t::MODEL_GRID, 5, 5 };
            gcoords_t to{ grid_t::MODEL_GRID, 3, 3 };
            auto & model = isim.get_model();
            model.resize(from.cat, isim.part_of(PART[0]), from.pos);

            for (dcoords_t xy{ }; xy.in(from.pos); xy.rectangle(from.pos)) {
                gcoords_t ipos{ grid_t::MODEL_GRID, xy };
                tab.tire(ipos, model.at(ipos));
            }

            REQUEST(ctx, prog) {
                ctx.resize_grid(to);
            }

            auto & active = tab.get_active();
            auto & inactive = tab.get_inactive();
            for (dcoords_t xy{ }; xy.in(to.pos - 1); xy.rectangle(to.pos - 1)) {
                REQUIRE_NOTHROW(inactive.at(gcoords_t(grid_t::MODEL_GRID, xy)));
            }
            for (dcoord_t y = 0; y < to.pos.y; ++y) {
                REQUIRE_NOTHROW(active.at(gcoords_t(grid_t::MODEL_GRID, to.pos.x - 1, y)));
            }
            for (dcoord_t x = 0; x < to.pos.x; ++x) {
                REQUIRE_NOTHROW(active.at(gcoords_t(grid_t::MODEL_GRID, x, to.pos.y - 1)));
            }
            REQUIRE(tab.size() == to.pos.x * to.pos.y);
        }
    }

    SECTION("A cycle updates every cell") {
        isim.commence();

        part counter{ PART[1] };
        counter.add_entry(entry{ of::VALUE,
                                 text("__VALUE"),
                                 text("Counter value"),
                                 value(uint_t()),
                                 ui_access::VISIBLE,
                                 serialize::NO_SERIALIZE,
                                 std::array<uint_t, 3>{ 0, std::numeric_limits<uint_t>::max(), 1 }});

        counter.delegates.cycle = [](cell & cl) {
            auto prop = cl[of::VALUE];
            prop = uint_t(1u);
        };

        isim.include_part(counter);
        auto & model = isim.get_model();
        model.resize(grid_t::MODEL_GRID, isim.part_of(PART[1]), dcoords_t(5, 5));

        for (auto &[pos, clb] : model.get_model()) {
            REQUIRE(get<uint_t>(clb.get(of::VALUE)) == 0u);
        }

        automaton.set_state(PARTICIPANT.no_one(), automaton::state::RUN);
        REQUIRE_NOTHROW(automaton.cycle());

        for (auto &[pos, clb] : model.get_model()) {
            REQUIRE(get<uint_t>(clb.get(of::VALUE)) == 1u);
        }
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
