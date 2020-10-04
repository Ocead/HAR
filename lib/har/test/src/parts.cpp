//
// Created by Johannes on 26.05.2020.
//

#include <har/cell.hpp>
#include <har/grid_cell.hpp>
#include <har/simulation.hpp>

#include "logic/context.hpp"
#include "world/grid_cell_base.hpp"

#include <catch2/catch.hpp>

using namespace har;

TEST_CASE("Parts") {
    part pt1{ PART[0], "gnampf" };
    part pt2{ PART[0], "fnampf" };
    part pt3{ PART[1], "gnampf" };

    SECTION("Parts are compared by id only") {
        REQUIRE(pt1 == pt2);
        REQUIRE(pt1 != pt3);
    }

    SECTION("Properties can be added to the model of a part") {
        entry e{ of::ANALOG_VOLTAGE,
                 "ANALOG_VOLTAGE",
                 "Analog voltage",
                 value(double_t()),
                 ui_access::INVISIBLE,
                 serialize::SERIALIZE,
                 std::array<double_t, 3>{ -5., 5., .1 }};

        REQUIRE_NOTHROW(pt1.add_entry(e));
        REQUIRE(pt1.model().at(of::ANALOG_VOLTAGE).id == e.id);

        grid_cell_base cell{ pt1, gcoords_t(INVALID_GRID, 0, 0) };
        context ctx{ };
        grid_cell gcl{ ctx, cell };

        pt1.init_standard(cell);
        cell.transit();
        REQUIRE(double_t(gcl[of::ANALOG_VOLTAGE]) == 0.);
    }

    SECTION("Cells of a part are filled with values for each entry in the property model") {
        entry e1{ of::ANALOG_VOLTAGE,
                  "ANALOG_VOLTAGE",
                  "Analog voltage",
                  value(double_t()),
                  ui_access::INVISIBLE,
                  serialize::SERIALIZE,
                  std::array<double_t, 3>{ -5., 5., .1 }};
        entry e2{ of::NEXT_FREE,
                  "POSITIVE_VOLTAGE",
                  "Positive voltage",
                  value(double_t(5.)),
                  ui_access::INVISIBLE,
                  serialize::SERIALIZE,
                  std::array<double_t, 3>{ 5., 10., .1 }};

        pt1.add_entry(e1);
        pt1.add_entry(e2);

        grid_cell_base cell{ pt1, gcoords_t(INVALID_GRID, 0, 0) };
        context ctx{ };
        grid_cell gcl{ ctx, cell };

        pt1.init_standard(cell);
        cell.transit();
        REQUIRE(double_t(gcl[of::ANALOG_VOLTAGE]) == 0.);
        REQUIRE(double_t(gcl[of::NEXT_FREE]) == 5.);
    }

    SECTION("Delegates can be called when defined") {
        grid_cell_base gclb{ pt1, gcoords_t() };
        context ctx{ };
        grid_cell gcl{ ctx, gclb };

        SECTION("init_static") {
            volatile bool_t init_static_ok = false;
            pt1.delegates.init_static = [&](cell &) {
                init_static_ok = true;
            };

            REQUIRE_NOTHROW(pt1.init_static(gcl));
            REQUIRE(init_static_ok);
        }

        SECTION("init_relative") {
            volatile bool_t init_relative_ok = false;
            pt1.delegates.init_relative = [&](cell &) {
                init_relative_ok = true;
            };

            REQUIRE_NOTHROW(pt1.init_relative(gcl));
            REQUIRE(init_relative_ok);
        }

        SECTION("clear") {
            volatile bool_t clear_ok = false;
            pt1.delegates.clear = [&](cell &) {
                clear_ok = true;
            };

            REQUIRE_NOTHROW(pt1.clear(gcl));
            REQUIRE(clear_ok);
        }

        SECTION("cycle") {
            volatile bool_t cycle_ok = false;
            pt1.delegates.cycle = [&](cell &) {
                cycle_ok = true;
            };

            REQUIRE_NOTHROW(pt1.cycle(gcl));
            REQUIRE(cycle_ok);
        }

        SECTION("move") {
            volatile bool_t move_ok = false;
            pt1.delegates.move = [&](cell &) {
                move_ok = true;
            };

            REQUIRE_NOTHROW(pt1.move(gcl));
            REQUIRE(move_ok);
        }

        SECTION("draw") {
            image_t im;
            volatile bool_t draw_ok = false;
            pt1.delegates.draw = [&](const cell &, image_t &) {
                draw_ok = true;
            };

            REQUIRE_NOTHROW(pt1.draw(gcl, im));
            REQUIRE(draw_ok);
        }

        SECTION("press") {
            volatile bool_t press_ok = false;
            pt1.delegates.press = [&](cell &, const ccoords_t &) {
                press_ok = true;
            };

            REQUIRE_NOTHROW(pt1.press(gcl, ccoords_t()));
            REQUIRE(press_ok);
        }

        SECTION("release") {
            volatile bool_t release_ok = false;
            pt1.delegates.release = [&](cell &, const ccoords_t &) {
                release_ok = true;
            };

            REQUIRE_NOTHROW(pt1.release(gcl, ccoords_t()));
            REQUIRE(release_ok);
        }
    }

    SECTION("Delegates can be called when not defined") {
        grid_cell_base cell{ pt2, gcoords_t() };
        context ctx{ };
        grid_cell gcl{ ctx, cell };

        SECTION("init_static") {
            REQUIRE_NOTHROW(pt2.init_static(gcl));
        }

        SECTION("init_relative") {
            REQUIRE_NOTHROW(pt2.init_relative(gcl));
        }

        SECTION("clear") {
            REQUIRE_NOTHROW(pt2.clear(gcl));
        }

        SECTION("cycle") {
            REQUIRE_NOTHROW(pt2.cycle(gcl));
        }

        SECTION("move") {
            REQUIRE_NOTHROW(pt2.move(gcl));
        }

        SECTION("draw") {
            image_t im;
            REQUIRE_NOTHROW(pt2.draw(gcl, im));
        }

        SECTION("press") {
            REQUIRE_NOTHROW(pt2.press(gcl, ccoords_t()));
        }

        SECTION("release") {
            REQUIRE_NOTHROW(pt2.release(gcl, ccoords_t()));
        }
    }

}
