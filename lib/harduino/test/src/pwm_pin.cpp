//
// Created by Johannes on 11.09.2020.
//

#include <har/duino.hpp>
#include <har/sketch_cell.hpp>

#include "parts.hpp"

#include <catch2/catch.hpp>

using namespace har;

TEST_CASE("PWM pin") {
    part pt = duino::parts::pwm_pin();
    sketch_grid_cell sgcl{ pt };

    SECTION(".cycle") {
        SECTION("Output") {
            auto volt = 5.;
            sgcl[of::PIN_MODE] = uint_t(parts::pin_mode::OUTPUT);
            sgcl[of::PWM_VOLTAGE] = volt;

            for (auto & duty : { 0., .5, 1. }) {
                sgcl[of::PWM_DUTY] = duty;
                sgcl.transit();

                pt.cycle(sgcl);
                sgcl.transit();

                REQUIRE(double_t(sgcl[of::POWERING_PIN]) == volt * duty);
            }
        }

        SECTION("Input") {
            SECTION("from PWM enabled") {
                sketch_grid_cell ngcl{ pt };

                sgcl.add_connection(direction::PIN, ngcl);
                sgcl[of::PIN_MODE] = uint_t(parts::pin_mode::INPUT);
                ngcl[of::PIN_MODE] = uint_t(parts::pin_mode::OUTPUT);
                sgcl.transit();

                for (auto &[volt, duty] : std::map<double_t, double_t>{{ 1.,  0. },
                                                                       { 5,   .5 },
                                                                       { 3.3, 1. }}) {
                    ngcl[of::PWM_VOLTAGE] = volt;
                    ngcl[of::PWM_DUTY] = duty;
                    ngcl.transit();

                    pt.cycle(sgcl);
                    sgcl.transit();

                    REQUIRE(double_t(sgcl[of::PWM_VOLTAGE]) == volt);
                    REQUIRE(double_t(sgcl[of::PWM_DUTY]) == duty);
                    REQUIRE(double_t(sgcl[of::POWERED_PIN]) == volt * duty);
                }
            }

            SECTION("from other") {
                part npt = duino::parts::constant_pin();
                sketch_grid_cell ngcl{ npt };
                auto volt = 5.;

                sgcl.add_connection(direction::PIN, ngcl);
                sgcl[of::PIN_MODE] = uint_t(parts::pin_mode::INPUT);
                sgcl[of::PWM_VOLTAGE] = volt;

                for (auto & duty : { 0., .5, 1. }) {
                    ngcl[of::POWERING_PIN] = volt * duty;
                    ngcl.transit();
                    sgcl.transit();

                    pt.cycle(sgcl);
                    sgcl.transit();

                    REQUIRE(double_t(sgcl[of::PWM_DUTY]) == duty);
                    REQUIRE(double_t(sgcl[of::POWERED_PIN]) == volt * duty);
                }
            }
        }
    }
}
