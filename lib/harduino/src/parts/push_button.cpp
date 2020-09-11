//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::push_button(part_h offset) {
    part pt{ PART[standard_ids::PUSH_BUTTON + offset],
             text("har:push_button"),
             traits::COMPONENT_PART |
             traits::OUTPUT |
             traits::SENSOR |
             traits::COLORED,
             text("Push button") };

    add_properties_for_traits(pt, 5.);

    pt.delegates.cycle = [](cell & cl) {

    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto color = cl.as_grid_cell().is_placed() ? color_t(cl[of::COLOR])
                                                       : color_t(114u, 158u, 206u, 255u);

            cr->set_source_rgb(.2, .2, .2);
            cr->rectangle(24., 24., 256. - 48., 256. - 48.);
            cr->fill_preserve();
            cr->set_source_rgb(.5, .5, .5);
            cr->set_line_width(16.);
            cr->stroke();

            cr->set_source_rgb(color.r / 256., color.g / 256., color.b / 256.);
            cr->arc(128., 128., 80., 0, 2 * M_PI);
            cr->fill();
            cr->stroke();

            if (bool_t(cl[FIRING])) {
                cr->set_operator(Cairo::OPERATOR_OVER);
                cr->set_source_rgba(.5, .5, .5, .5);
                cr->arc(128., 128., 80., 0, 2 * M_PI);
                cr->fill();
                cr->stroke();
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.delegates.press = [](cell & cl, const ccoords_t & pos) {
        cl[of::FIRING] = true;
        cl[of::POWERING_PIN] = cl[of::HIGH_VOLTAGE];
    };

    pt.delegates.release = [](cell & cl, const ccoords_t & pos) {
        cl[of::FIRING] = false;
        cl[of::POWERING_PIN] = cl[of::LOW_VOLTAGE];
    };

    pt.add_visuals({
                           of::COLOR,
                           of::FIRING
                   });

    return pt;
}
