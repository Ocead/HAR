//
// Created by Johannes on 18.07.2020.
//

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <har/grid_cell.hpp>

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::empty(part_h offset) {
    part pt{ PART[standard_ids::EMPTY + offset],
             text("har:empty"),
             traits::EMPTY_PART | traits::COMPONENT_PART | traits::BOARD_PART,
             text("Empty") };

    add_properties_for_traits(pt, 5.);

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);

            auto & gcl = cl.as_grid_cell();
            if (!gcl.is_placed()) {
                auto cr = Cairo::Context::create(sf);

                cr->save();
                cr->translate(128., 128.);
                cr->rotate(45. * M_PI / 180.);
                cr->scale(1.15, 1.15);
                cr->translate(-128., -128.);
                cr->set_source_rgba(0., 0., 0., .25);
                cr->rectangle(128. - 16., 0., 32., 256.);
                cr->rectangle(0., 128. - 16., 256., 32.);
                cr->fill();
                cr->stroke();
                cr->restore();
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    return pt;
}
