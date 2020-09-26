//
// Created by Johannes on 19.08.2020.
//

#include <cairomm/context.h>

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::constant_pin(part_h offset) {
    part pt{ PART[standard_ids::CONSTANT_PIN + offset],
             text("har:constant_pin"),
             traits::BOARD_PART |
             traits::OUTPUT |
             traits::COLORED,
             text("Constant pin") };

    add_properties_for_traits(pt, 5.);

    pt.add_entry(entry{ of::POWERING_PIN,
                        text("__POWERING_PIN"),
                        text("Powering (pin)"),
                        value(double_t()),
                        ui_access::CHANGEABLE,
                        SERIALIZE,
                        std::array<double_t, 3>{ 0., 5., .1 }});

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

    return pt;
}
