//
// Created by Johannes on 11.09.2020.
//

#include <har/duino.hpp>
#include <har/sketch_cell.hpp>

#include <catch2/catch.hpp>

using namespace har;

TEST_CASE("Push button") {
    part pt = duino::parts::push_button();
    sketch_grid_cell sgcl{ pt };

    SECTION(".press") {
        REQUIRE(sgcl[of::POWERING_PIN] == sgcl[of::LOW_VOLTAGE]);
        REQUIRE(bool_t(sgcl[of::FIRING]) == false);

        pt.press(sgcl, ccoords_t());
        sgcl.transit();

        REQUIRE(double_t(sgcl[of::POWERING_PIN]) == 5.);
        REQUIRE(bool_t(sgcl[of::FIRING]) == true);
    }

    SECTION(".release") {
        sgcl[of::POWERING_PIN] = 5.;
        sgcl[of::FIRING] = true;
        sgcl.transit();

        pt.release(sgcl, ccoords_t());
        sgcl.transit();

        REQUIRE(sgcl[of::POWERING_PIN] == sgcl[of::LOW_VOLTAGE]);
        REQUIRE(bool_t(sgcl[of::FIRING]) == false);
    }
}
