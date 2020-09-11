//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::seven_segment(part_h offset) {
    part pt{ PART[standard_ids::SEVEN_SEGMENT + offset],
             text("har:seven_segment"),
             traits::COMPONENT_PART | traits::INPUT | traits::COLORED,
             text("Seven segment display") };

    add_properties_for_traits(pt, 5.);

    pt.delegates.cycle = [](cell & cl) {

    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto gcl = cl.as_grid_cell();
            auto color = color_t(gcl[of::COLOR]);

            cr->translate(-16., 0.);

            /*A*/ {
                cr->save();
                cr->translate(128., 48.);
                draw_segment(cr, color, color_t());
                cr->restore();
            }

            /*B*/ {
                cr->save();
                cr->rotate(M_PI_2);
                cr->translate(88., -192.);
                draw_segment(cr, color, color_t(), .60);
                cr->restore();
            }

            /*C*/ {
                cr->save();
                cr->rotate(M_PI_2);
                cr->translate(168., -192.);
                draw_segment(cr, color, color_t(), .60);
                cr->restore();
            }

            /*D*/ {
                cr->save();
                cr->translate(128., 208.);
                draw_segment(cr, color, color_t());
                cr->restore();
            }

            /*E*/ {
                cr->save();
                cr->rotate(M_PI_2);
                cr->translate(88., -64.);
                draw_segment(cr, color, color_t(), .60);
                cr->restore();
            }

            /*F*/ {
                cr->save();
                cr->rotate(M_PI_2);
                cr->translate(168., -64.);
                draw_segment(cr, color, color_t(), .60);
                cr->restore();
            }

            /*G*/ {
                cr->save();
                cr->translate(128., 128.);
                draw_segment(cr, color, color_t());
                cr->restore();
            }

            /*DP*/ {
                cr->save();
                cr->arc(224., 224., 20., 0, 2 * M_PI);
                cr->fill();
                cr->restore();
            }

            cr->set_line_width(8.);
            cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
            cr->fill_preserve();
            cr->set_source_rgba(0. / 256., 0. / 256., 0. / 256., 1.);
            cr->stroke();

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({ of::COLOR,
                     of::POWERING_PIN });

    pt.add_connection_uses({{ direction::PIN[0], text("A segment") },
                            { direction::PIN[1], text("B segment") },
                            { direction::PIN[2], text("C segment") },
                            { direction::PIN[3], text("D segment") },
                            { direction::PIN[4], text("E segment") },
                            { direction::PIN[5], text("F segment") },
                            { direction::PIN[6], text("G segment") },
                            { direction::PIN[7], text("DP segment") }});

    return pt;
}
