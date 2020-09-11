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
             traits::BOARD_PART | traits::INPUT | traits::OUTPUT | traits::COLORED,
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
                if (gcl.connected().size() > 1) {
                    auto ngcl = gcl[direction::PIN[0]];
                    if (ngcl.has(PWM_VOLTAGE)) {
                        cl[PWM_VOLTAGE] = ngcl[PWM_VOLTAGE];
                        cl[PWM_DUTY] = ngcl[PWM_DUTY];
                        cl[POWERED_PIN] = ngcl[POWERING_PIN];
                    }
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

    return pt;
}
