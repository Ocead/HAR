//
// Created by Johannes on 16.08.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::smd_button(part_h offset) {
    part pt{ PART[standard_ids::SMD_BUTTON + offset],
             text("har:smd_button"),
             traits::BOARD_PART |
             traits::INPUT |
             traits::SENSOR |
             traits::COLORED |
             traits::ORIENTABLE,
             text("SMD Button") };

    add_properties_for_traits(pt, 5.);

    pt.add_entry(entry(of::DESIGN,
                       text("__DESIGN"),
                       text("Design"),
                       value(uint_t()),
                       ui_access::VISIBLE,
                       serialize::SERIALIZE,
                       {
                               { 0u, text("Square") },
                               { 1u, text("Slim") }
                       }));

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

            cr->save();

            if (uint_t(gcl[NEXT_FREE + 1]) == 1) {
                cr->translate(128., 128.);
                cr->rotate(M_PI_2);
                cr->translate(-128., -128.);
            }

            if (uint_t(gcl[DESIGN]) == 0) {
                cr->set_source_rgb(.5, .5, .5);
                cr->rectangle(0., 0., 256., 256.);
                cr->fill();
                cr->stroke();

                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->arc(128., 128., 64., 0., 2 * M_PI);
                cr->fill();
                cr->arc(48., 48., 24., 0., 2. * M_PI);
                cr->fill();
                cr->arc(48., 256. - 48., 24., 0., 2. * M_PI);
                cr->fill();
                cr->arc(256. - 48., 48., 24., 0., 2. * M_PI);
                cr->fill();
                cr->arc(256. - 48., 256. - 48., 24., 0., 2. * M_PI);
                cr->fill();
                cr->stroke();

                if (bool_t(cl[FIRING])) {
                    cr->set_operator(Cairo::OPERATOR_OVER);
                    cr->set_source_rgba(.5, .5, .5, .5);
                    cr->arc(128., 128., 64., 0., 2. * M_PI);
                    cr->fill();
                    cr->stroke();
                }
            } else {
                cr->set_source_rgb(.5, .5, .5);
                cr->rectangle(48., 0., 256. - 96., 256.);
                cr->fill();
                cr->stroke();

                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(96., 64., 256. - 192., 256. - 128.);
                cr->fill();
                cr->rectangle(96., 16., 256. - 192., 24.);
                cr->fill();
                cr->rectangle(96., 256. - 16 - 24., 256. - 192., 24.);
                cr->fill();
                cr->stroke();

                if (bool_t(cl[FIRING])) {
                    cr->set_operator(Cairo::OPERATOR_OVER);
                    cr->set_source_rgba(.5, .5, .5, .5);
                    cr->rectangle(96., 64., 256. - 192., 256. - 128.);
                    cr->fill();
                    cr->stroke();
                }
            }

            cr->restore();

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.delegates.press = [](cell & cl, const ccoords_t & pos) {
        cl[FIRING] = true;
    };

    pt.delegates.release = [](cell & cl, const ccoords_t & pos) {
        cl[FIRING] = false;
    };

    pt.add_visuals({
                           of::COLOR,
                           of::POWERING_PIN,
                           of::FIRING,
                           of::DESIGN,
                           of::NEXT_FREE + 1
                   });

    return pt;
}
