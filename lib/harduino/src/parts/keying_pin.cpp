//
// Created by Johannes on 18.07.2020.
//

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::keying_pin(part_h offset) {
    part pt{ PART[standard_ids::KEYING_PIN + offset],
             text("har:keying_pin"),
             traits::BOARD_PART | traits::COLORED,
             text("Keying pin") };

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

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto & gcl = cl.as_grid_cell();
            auto color = color_t(gcl[of::COLOR]);

            if (uint_t(cl[DESIGN]) == 1) {
                cr->save();
                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(0., 0., 256., 256.);
                cr->fill();
                cr->stroke();
                cr->restore();
            } else {
                draw_pin_base(cr, color);
                draw_pin_corners(cr, gcl, color);
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({
                           of::COLOR,
                           of::NEXT_FREE
                   });

    return pt;
}
