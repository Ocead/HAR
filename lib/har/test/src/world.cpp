//
// Created by Johannes on 26.05.2020.
//

#include "world/world.hpp"

#include <catch2/catch.hpp>

using namespace har;

inline void mark_grid(grid & g) {
    for (auto & c : g) {
        c.second.set(of::NEXT_FREE, value(c.first));
        c.second.transit();
    }
}

inline void check_marked_grid(grid & g, const dcoords_t & ins, int_t by) {
    for (auto & c : g) {
        auto[posx, posy] = c.first;

        if (!(posx == ins.x || posy == ins.y)) {
            if (ins.x >= 0 && posx >= ins.x) posx += by;
            if (ins.y >= 0 && posy >= ins.y) posy += by;

            REQUIRE((posx, posy) == get<dcoords_t>(c.second.get(of::NEXT_FREE)));
        }
    }
}

inline void check_neighbors(const grid & g, const dcoords_t & c) {
    for (auto d : direction::cardinal) {
        auto * p = g.at(c).get_neighbor(d);
        if (p) {
            REQUIRE(p == &g.at(c + dcoords_t(d)));
        }
    }
}

inline void check_all_cells(const grid & g) {
    for (dcoords_t ip{ 0, 0 }; ip != g.dim(); ip.rectangle(dcoords_t(), g.dim())) {
        check_neighbors(g, ip);
    }
}

inline void connect_grid(grid & g) {
    for (auto & c : g) {
        auto & gclb = c.second;
        auto & nclb = g.at((gclb.position().pos + dcoords_t(1, 0)) % g.dim());
        gclb.add_connection(direction::PIN[0], nclb);
    }
}

inline void check_connected_grid(grid & g) {
    for (auto & c : g) {
        auto & gclb = c.second;
        auto nclb = gclb.get_connected(direction::PIN[0]);
        REQUIRE(nclb);
        REQUIRE(nclb->position() == gcoords_t(gclb.position().cat, (gclb.position().pos + dcoords_t(1, 0)) % g.dim()));
    }
}

TEST_CASE("Grid", "[grid]") {
    const part pt{ };
    grid gd1{ gcoords_t(INVALID_GRID, 5, 5), pt };

    const auto before = gd1.dim();

    SECTION("Rows can be added") {
        REQUIRE_NOTHROW(gd1.insert_row(pt));
        REQUIRE(gd1.dim() == before + dcoords_t(direction::DOWN));
        check_all_cells(gd1);

        mark_grid(gd1);
        REQUIRE_NOTHROW(gd1.insert_row(pt, 2));
        check_all_cells(gd1);
        check_marked_grid(gd1, dcoords_t(-1, 2), -1);
    }

    SECTION("Columns can be added") {
        REQUIRE_NOTHROW(gd1.insert_column(pt));
        REQUIRE(gd1.dim() == before + dcoords_t(direction::RIGHT));
        check_all_cells(gd1);

        mark_grid(gd1);
        REQUIRE_NOTHROW(gd1.insert_column(pt, 2));
        check_all_cells(gd1);
        check_marked_grid(gd1, dcoords_t(2, -1), -1);
    }

    SECTION("Rows can be removed") {
        REQUIRE_NOTHROW(gd1.remove_row());
        REQUIRE(gd1.dim() == before + dcoords_t(direction::UP));
        check_all_cells(gd1);

        mark_grid(gd1);
        REQUIRE_NOTHROW(gd1.remove_row(2));
        check_all_cells(gd1);
        check_marked_grid(gd1, dcoords_t(-1, 2), 1);
    }

    SECTION("Columns can be removed") {
        REQUIRE_NOTHROW(gd1.remove_column());
        REQUIRE(gd1.dim() == before + dcoords_t(direction::LEFT));
        check_all_cells(gd1);

        mark_grid(gd1);
        REQUIRE_NOTHROW(gd1.remove_column(2));
        check_all_cells(gd1);
        check_marked_grid(gd1, dcoords_t(2, -1), 1);
    }

    SECTION("Can be resized") {
        dcoords_t to{ 4, 4 };
        REQUIRE_NOTHROW(gd1.resize_to(pt, to));
        auto after = gd1.dim();
        REQUIRE(after == to);
        check_all_cells(gd1);

        REQUIRE_NOTHROW(gd1.resize_by(pt, before - after));
        after = gd1.dim();
        REQUIRE(after == before);
        check_all_cells(gd1);
    }

    SECTION("When moved in memory, grids remain intact") {
        grid gd2;

        mark_grid(gd1);
        connect_grid(gd1);

        REQUIRE_NOTHROW(gd2 = std::move(gd1));

        check_marked_grid(gd2, dcoords_t(), 0);
        check_connected_grid(gd2);
    }
}

TEST_CASE("World", "[!mayfail][world]") {
    part pt{ };
    world w1{ };

    for (auto i : { true, false }) {
        auto & grid = i ? w1.get_model() : w1.get_bank();
        auto str = i ? "model" : "bank";

        DYNAMIC_SECTION("for " << str << " grid") {
            SECTION("Cells can be inserted") {
                FAIL("Not implemented");
            }

            SECTION("Cells can be swapped") {
                FAIL("Not implemented");
            }

            SECTION("Cells can be removed") {
                FAIL("Not implemented");
            }

            SECTION("Grid can be minimized") {
                FAIL("Not implemented");
            }
        }
    }

    SECTION("When moved in memory, worlds remain intact") {
        world w2;

        mark_grid(w1.get_model());
        connect_grid(w1.get_model());
        mark_grid(w1.get_bank());
        connect_grid(w1.get_bank());

        REQUIRE_NOTHROW(w2 = std::move(w1));

        check_marked_grid(w2.get_model(), dcoords_t(), 0);
        check_connected_grid(w2.get_model());
        check_marked_grid(w2.get_bank(), dcoords_t(), 0);
        check_connected_grid(w2.get_bank());
    }
}
