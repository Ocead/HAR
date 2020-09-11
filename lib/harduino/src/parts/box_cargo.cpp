//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::box_cargo(part_h offset) {
    part pt{ PART[standard_ids::BOX_CARGO + offset],
             text("har:box_cargo"),
             traits::CARGO_PART |
             traits::COLORED,
             text("Box cargo") };

    add_properties_for_traits(pt, 5.);

    pt.remove_entry(of::COLOR);

    pt.add_entry(entry{ of::COLOR,
                        text("__COLOR"),
                        text("Color"),
                        value(color_t(193u, 125u, 17u, 255u)),
                        ui_access::CHANGEABLE,
                        serialize::SERIALIZE });

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto gcl = cl.as_grid_cell();
            auto color = color_t(gcl[of::COLOR]);

            cr->set_source_rgb(color.r / 255., color.g / 255.,  color.b / 255.);
            cr->rectangle(0., 0., 256., 256.);
            cr->fill();
            cr->stroke();

            cr->set_operator(Cairo::OPERATOR_OVER);
            cr->set_source_rgba(0., 0., 0., .2);
            cr->move_to(0., 64.);
            cr->line_to(256., 64.);
            cr->move_to(128., 0.);
            cr->line_to(128., 64.);
            cr->set_line_width(16.);
            cr->stroke();

            cr->set_source_rgb(0., 0., 0.);
            cr->move_to(96., 112.);
            cr->line_to(256. - 96., 112.);
            cr->set_line_width(40.);
            cr->set_line_cap(Cairo::LINE_CAP_ROUND);
            cr->stroke();

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visual(of::COLOR);

    return pt;
}
