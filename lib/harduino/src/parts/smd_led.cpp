//
// Created by Johannes on 16.08.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::smd_led(part_h offset) {
    part pt{ PART[standard_ids::SMD_LED + offset],
             text("har:smd_led"),
             traits::BOARD_PART |
             traits::INPUT |
             traits::ORIENTABLE |
             traits::COLORED,
             text("SMD LED") };

    add_properties_for_traits(pt, 5.);

    pt.add_entry(entry(of::DESIGN,
                       text("__DESIGN"),
                       text("Design"),
                       value(uint_t()),
                       ui_access::CHANGEABLE,
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

            auto gcl = cl.as_grid_cell();
            auto color = color_t(gcl[of::COLOR]);

            cr->save();

            if (uint_t(cl[NEXT_FREE + 1]) == 1) {
                cr->translate(128., 128.);
                cr->rotate(M_PI_2);
                cr->translate(-128., -128.);
            }

            if (uint_t(cl[DESIGN]) == 0) {
                cr->set_source_rgb(.5, .5, .5);
                cr->rectangle(32., 0., 32., 256.);
                cr->fill();
                cr->rectangle(112., 0., 32., 256.);
                cr->fill();
                cr->rectangle(192., 0., 32., 256.);
                cr->fill();
                cr->stroke();

                cr->set_source_rgb(.9, .9, .9);
                cr->rectangle(0., 16., 256., 256. - 32.);
                cr->fill();
                cr->stroke();

                cr->set_source_rgba(color.r / 256.,
                                    color.g / 256.,
                                    color.b / 256.,
                                    color.a / 256.);
                cr->arc(128., 128., 96., 0., 2 * M_PI);
                cr->fill();
                cr->stroke();
                if (double_t(cl[HIGH_VOLTAGE]) - double_t(cl[POWERED_PIN]) > .1) {
                    cr->set_operator(Cairo::OPERATOR_OVER);
                    cr->set_source_rgba(0., 0., 0., .75);
                    cr->arc(128., 128., 96., 0., 2 * M_PI);
                    cr->fill();
                    cr->stroke();
                }
            } else {
                cr->set_source_rgb(.5, .5, .5);
                cr->rectangle(96., 0., 64., 256.);
                cr->fill();

                cr->set_source_rgb(.9, .9, .9);
                cr->rectangle(64., 16., 256. - 128., 256. - 32.);
                cr->fill();
                cr->stroke();

                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(80., 48., 256. - 160., 256. - 96.);
                cr->fill();
                cr->stroke();

                if (double_t(cl[HIGH_VOLTAGE]) - double_t(cl[POWERED_PIN]) > .1) {
                    cr->set_operator(Cairo::OPERATOR_OVER);
                    cr->set_source_rgba(0., 0., 0., .75);
                    cr->rectangle(80., 48., 256. - 160., 256. - 96.);
                    cr->fill();
                    cr->stroke();
                }
            }

            cr->restore();

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({
                           of::COLOR,
                           of::POWERING_PIN,
                           of::POWERED_PIN,
                           of::LOW_VOLTAGE,
                           of::HIGH_VOLTAGE,
                           of::DESIGN,
                           of::NEXT_FREE + 1
                   });

    return pt;
}
