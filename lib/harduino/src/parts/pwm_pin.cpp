//
// Created by Johannes on 18.07.2020.
//

#include <numeric>

#include <cairomm/context.h>

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::pwm_pin(part_h offset) {
    part pt{ PART[standard_ids::PWM_PIN + offset],
             text("har:pwm_pin"),
             traits::BOARD_PART |
             traits::INPUT |
             traits::OUTPUT |
             traits::COLORED,
             text("PWM pin") };

    add_properties_for_traits(pt, 5.);

    pt.delegates.init_relative = [](cell & cl) {
        auto & gcl = cl.as_grid_cell();
        for (auto dir : direction::cardinal) {
            auto & ncl = gcl[dir];
            if (ncl.is_placed() && ncl.has(of::NEXT_FREE + 1)) {
                cl[of::NEXT_FREE + 1] = ncl[of::NEXT_FREE + 1];
                break;
            }
        }
    };

    pt.add_entry(entry{ of::PWM_VOLTAGE,
                        text("__PWM_VOLTAGE"),
                        text("PWM voltage"),
                        value(double_t()),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE,
                        std::array<double_t, 3>{ -5., 5., .1 }});

    pt.add_entry(entry{ of::PWM_DUTY,
                        text("__PWM_DUTY"),
                        text("PWM duty cycle"),
                        value(double_t()),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE,
                        std::array<double_t, 3>{ 0., 1., .01 }});

    pt.delegates.cycle = [](cell & cl) {
        switch (pin_mode(uint_t(cl[PIN_MODE]))) {
            case pin_mode::TRI_STATE: {
                auto out = double_t(cl[of::POWERING_PIN]);
                if (!std::isnan(out)) {
                    cl[of::POWERING_PIN] = std::nan("1");
                }

                auto in = double_t(cl[of::POWERED_PIN]);
                if (!std::isnan(in)) {
                    cl[of::POWERED_PIN] = std::nan("1");
                }
                break;
            }
            case pin_mode::OUTPUT: {
                cl[POWERING_PIN] = double_t(cl[PWM_VOLTAGE]) * double_t(cl[PWM_DUTY]);
                break;
            }
            case pin_mode::INPUT: {
                auto & gcl = cl.as_grid_cell();
                if (!gcl.connected().empty()) {
                    auto ngcl = gcl[direction::PIN];
                    if (ngcl.has(PWM_VOLTAGE)) {
                        auto nvolt = double_t(ngcl[PWM_VOLTAGE]);
                        auto nduty = double_t(ngcl[PWM_DUTY]);

                        cl[PWM_VOLTAGE] = nvolt;
                        cl[PWM_DUTY] = nduty;
                        cl[POWERED_PIN] = nvolt * nduty;
                    } else if (ngcl.has(POWERING_PIN)) {
                        auto npwrd = double_t(ngcl[POWERING_PIN]);

                        cl[PWM_VOLTAGE] = npwrd;
                        cl[PWM_DUTY] = (npwrd == 0. ? 0. : 1.);
                        cl[POWERED_PIN] = npwrd;
                    } else {
                        if (auto prop = cl[PWM_VOLTAGE]; double_t(prop) != 0.) {
                            prop = 0.;
                        }
                        if (auto prop = cl[PWM_DUTY]; double_t(prop) != 0.) {
                            prop = 0.;
                        }
                        if (auto prop = cl[POWERED_PIN]; double_t(prop) != 0.) {
                            prop = 0.;
                        }
                    }
                } else {
                    if (auto prop = cl[PWM_VOLTAGE]; double_t(prop) != 0.) {
                        prop = 0.;
                    }
                    if (auto prop = cl[PWM_DUTY]; double_t(prop) != 0.) {
                        prop = 0.;
                    }
                    if (auto prop = cl[POWERED_PIN]; double_t(prop) != 0.) {
                        prop = 0.;
                    }
                }
                if (auto prop = cl[POWERING_PIN]; double_t(prop) != 0.) {
                    prop = 0.;
                }
                break;
            }
        }
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto gcl = cl.as_grid_cell();
            auto color = color_t(gcl[of::COLOR]);

            if (uint_t(cl[DESIGN]) == 1) {
                draw_socket(cr, color);
            } else {
                draw_pin_base(cr, color);
                draw_pin_corners(cr, gcl, color);
                draw_pin_center(cr);
            }

            if (gcl.is_placed()) {
                draw_pwm_voltage(cr, cl, color);
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({ of::COLOR,
                     of::POWERING_PIN,
                     of::POWERED_PIN,
                     of::LOW_VOLTAGE,
                     of::HIGH_VOLTAGE,
                     of::PIN_MODE,
                     of::PWM_VOLTAGE,
                     of::PWM_DUTY,
                     of::NEXT_FREE });

    pt.add_waking(of::POWERING_PIN);

    pt.add_connection_use(direction::PIN, text("Input"));

    return pt;
}
