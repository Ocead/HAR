//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::switch_button(part_h offset) {
    part pt{ PART[standard_ids::SWITCH_BUTTON + offset],
             text("har:switch_button"),
             traits::COMPONENT_PART |
             traits::OUTPUT |
             traits::SENSOR |
             traits::ORIENTABLE |
             traits::COLORED,
             text("Switch button") };

    add_properties_for_traits(pt, 5.);

    pt.remove_entry(of::FACING);

    pt.add_entry(entry(of::NEXT_FREE + 1,
                       text("__ORIENTATION"),
                       text("Orientation"),
                       value(uint_t()),
                       ui_access::CHANGEABLE,
                       serialize::SERIALIZE,
                       {
                               { 0u, text("Horizontal") },
                               { 1u, text("Vertical") }
                       }));

    pt.delegates.cycle = [](cell & cl) {

    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto color = cl.as_grid_cell().is_placed() ? color_t(cl[of::COLOR])
                                                       : color_t(115u, 210u, 22u, 255u);
            auto matrix = Cairo::identity_matrix();

            if (uint_t(cl[of::NEXT_FREE + 1])) {
                cr->translate(128., 128.);
                cr->rotate(M_PI_2);
                cr->translate(-128., -128.);
            }

            cr->set_source_rgb(.2, .2, .2);
            cr->rectangle(24., 24., 256. - 48., 256. - 48.);
            cr->fill_preserve();
            cr->set_source_rgb(.5, .5, .5);
            cr->set_line_width(16.);
            cr->stroke();

            cr->set_source_rgb(color.r / 256., color.g / 256., color.b / 256.);
            cr->rectangle(48., 48., 256. - 96., 256. - 96.);
            cr->fill();
            cr->stroke();

            static Cairo::RefPtr<Cairo::LinearGradient> pattern;
            if (!pattern) {
                pattern = Cairo::LinearGradient::create(0., 0., 128., 0.);
                pattern->add_color_stop_rgba(1., .2, .2, .2, .3);
                pattern->add_color_stop_rgba(0., .2, .2, .2, .8);
            }

            cr->set_operator(Cairo::OPERATOR_OVER);
            cr->set_source(pattern);
            if (bool_t(cl[of::FIRING])) {
                cr->rotate(M_PI);
                cr->translate(-256., -256.);
                matrix.rotate(M_PI);
                matrix.translate(-256., -256.);
            }
            pattern->set_matrix(matrix);
            cr->rectangle(48., 48., 256. - 96. - 64., 256. - 96.);
            cr->fill();
            cr->stroke();

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.delegates.press = [](cell & cl, const ccoords_t & pos) {
        if (bool_t(cl[of::FIRING])) {
            cl[of::FIRING] = false;
            cl[of::POWERING_PIN] = cl[of::LOW_VOLTAGE];
        } else {
            cl[of::FIRING] = true;
            cl[of::POWERING_PIN] = cl[of::HIGH_VOLTAGE];
        }
    };

    pt.delegates.release = [](cell & cl, const ccoords_t & pos) {

    };

    pt.add_visuals({
                           of::COLOR,
                           of::FIRING,
                           of::NEXT_FREE + 1
                   });

    return pt;
}
