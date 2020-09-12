//
// Created by Johannes on 12.09.2020.
//

#include <har/duino.hpp>
#include <har/sketch_cell.hpp>

#include "parts.hpp"

#include <catch2/catch.hpp>

using namespace har;

TEST_CASE("Digital pin") {
    part pt = duino::parts::digital_pin();
    sketch_grid_cell sgcl{ pt };

    SECTION(".cycle") {
        SECTION("Output") {
            sgcl[of::POWERING_PIN] = 5.;
            sgcl[of::PIN_MODE] = uint_t(parts::pin_mode::OUTPUT);
            sgcl.transit();

            pt.cycle(sgcl);
            sgcl.transit();

            REQUIRE(uint_t(sgcl[of::PIN_MODE]) == uint_t(parts::pin_mode::OUTPUT));
        }

        SECTION("Input") {
            part npt = duino::parts::constant_pin();
            sketch_grid_cell ngcl{ npt };

            sgcl.add_connection(direction::PIN, ngcl);
            sgcl[of::PIN_MODE] = uint_t(parts::pin_mode::INPUT);
            sgcl.transit();

            for (auto & volt : { 0., .5, 1. }) {
                ngcl[of::POWERING_PIN] = volt;
                ngcl.transit();

                pt.cycle(sgcl);
                sgcl.transit();

                REQUIRE(double_t(sgcl[of::POWERED_PIN]) == volt);
            }
        }
    }
}
