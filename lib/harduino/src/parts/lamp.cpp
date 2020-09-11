//
// Created by Johannes on 18.07.2020.
//

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::lamp(part_h offset) {
    part pt{ PART[standard_ids::LAMP + offset],
             text("har:lamp"),
             traits::COMPONENT_PART |
             traits::INPUT |
             traits::COLORED,
             text("Lamp") };

    add_properties_for_traits(pt, 5.);

    pt.delegates.cycle = [](cell & cl) {
        auto & gcl = cl.as_grid_cell();
        auto connected = gcl.connected();
        double_t voltage = 0;

        for (auto & [use, ncl] : connected) {
            voltage += double_t(ncl[of::POWERING_PIN]);
        }
        voltage /= connected.size();

        cl[of::POWERED_PIN] = voltage;
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto color = cl.as_grid_cell().is_placed() ? color_t(cl[of::COLOR])
                                                       : color_t(252u, 233u, 79u, 255u);

            cr->set_source_rgb(.2, .2, .2);
            cr->rectangle(24., 24., 256. - 48., 256. - 48.);
            cr->fill_preserve();
            cr->set_source_rgb(.5, .5, .5);
            cr->set_line_width(16.);
            cr->stroke();

            if (!cl.as_grid_cell().is_placed() ||
                (double_t(cl[of::POWERED_PIN]) - double_t(cl[of::HIGH_VOLTAGE])) >= 0.) {
                cr->set_source_rgb(color.r / 256., color.g / 256., color.b / 256.);
            } else {
                cr->set_source_rgb(0., 0., 0.);
            }

            cr->rectangle(48., 48., 256. - 96., 256. - 96.);
            cr->fill();
            cr->stroke();

            static Cairo::RefPtr<Cairo::RadialGradient> pattern;
            if (!pattern) {
                pattern = Cairo::RadialGradient::create(128., 128., 64., 128., 128., 160.);
                pattern->add_color_stop_rgba(1., .2, .2, .2, 1.);
                pattern->add_color_stop_rgba(0., .2, .2, .2, 0.);
            }

            cr->set_operator(Cairo::OPERATOR_OVER);
            cr->set_source(pattern);
            cr->rectangle(48., 48., 256. - 96., 256. - 96.);
            cr->fill();
            cr->stroke();

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({
                           of::COLOR,
                           of::POWERED_PIN,
                           of::POWERING_PIN
                   });

    pt.add_connection_use(direction::PIN, text("Input"));

    return pt;
}
