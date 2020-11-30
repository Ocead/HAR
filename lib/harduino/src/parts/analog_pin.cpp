//
// Created by Johannes on 18.07.2020.
//

#include <cairomm/context.h>

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::analog_pin(part_h offset) {
    part pt{ PART[standard_ids::ANALOG_PIN + offset],
             text("har:analog_pin"),
             traits::BOARD_PART |
             traits::INPUT |
             traits::OUTPUT |
             traits::COLORED,
             text("Analog pin") };

    add_properties_for_traits(pt, 5.);

    pt.remove_entry(of::INT_HANDLER);
    pt.remove_entry(of::INT_CONDITION);

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

    pt.delegates.cycle = [](cell & cl) {
        auto & gcl = cl.as_grid_cell();
        auto mode = pin_mode(uint_t(cl[of::PIN_MODE]));

        switch (mode) {
            case pin_mode::TRI_STATE: {
                auto out = double_t(gcl[of::POWERING_PIN]);
                if (!std::isnan(out)) {
                    gcl[of::POWERING_PIN] = std::nan("1");
                }

                auto in = double_t(gcl[of::POWERED_PIN]);
                if (!std::isnan(in)) {
                    gcl[of::POWERED_PIN] = std::nan("1");
                }
                break;
            }
            case pin_mode::OUTPUT: {
                break;
            }
            case pin_mode::INPUT: {
                auto before = double_t(gcl[of::POWERING_PIN]);

                double_t after = 0.;
                auto connected = gcl.connected();
                for (auto & ccl : connected) {
                    after += double_t(ccl.cell[of::POWERING_PIN]);
                }
                after /= connected.size();

                if (before != after) {
                    gcl[of::POWERED_PIN] = after;
                }
                break;
            }
        }
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto & gcl = cl.as_grid_cell();
            auto color = color_t(gcl[of::COLOR]);

            if (uint_t(cl[DESIGN]) == 1) {
                draw_socket(cr, color);
            } else {
                draw_pin_base(cr, color);
                draw_pin_corners(cr, gcl, color);
                draw_pin_center(cr);
            }

            if (gcl.is_placed()) {
                draw_voltage(cr, cl, color);
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({
                           of::COLOR,
                           of::POWERING_PIN,
                           of::POWERED_PIN,
                           of::LOW_VOLTAGE,
                           of::HIGH_VOLTAGE,
                           of::PIN_MODE,
                           of::DESIGN
                   });

    pt.add_waking(of::POWERING_PIN);

    pt.add_connection_use(direction::PIN, text("Input"));

    return pt;
}
