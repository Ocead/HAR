//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::motor(part_h offset) {
    part pt{ PART[standard_ids::MOTOR + offset],
             text("har:motor"),
             traits::COMPONENT_PART |
             traits::INPUT |
             traits::MOVING |
             traits::ORIENTABLE |
             traits::SOLID,
             text("Motor") };

    add_properties_for_traits(pt, 5.);

    pt.remove_entry(of::MOVING_FROM);
    pt.remove_entry(of::MOVING_TO);
    pt.remove_entry(of::MOTOR_DISTANCE);
    pt.remove_entry(of::MOTOR_DIRECTION);

    pt.add_entry(entry{ of::MOTOR_SPEED,
                        text("__MOTOR_SPEED"),
                        text("Motor speed"),
                        value(double_t(0.)),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE,
                        std::array<double_t, 3>{ -1., 1., .001 }});

    pt.add_entry(entry{ of::SPEED_FACTOR,
                        text("__SPEED_FACTOR"),
                        text("Speed factor"),
                        value(double_t(0.)),
                        ui_access::CHANGEABLE,
                        serialize::NO_SERIALIZE,
                        std::array<double_t, 3>{ 0., 1., .001 }});

    pt.delegates.cycle = [](cell & cl) {
        double_t speed = 0.;
        double_t dir = 1.;

        for (auto &[use, ncl] : cl.as_grid_cell().connected()) {
            if (use == direction::PIN[0]) {
                speed = double_t(cl[of::SPEED_FACTOR]) *
                        (double_t(ncl[of::POWERING_PIN]) / double_t(cl[of::HIGH_VOLTAGE]));
            } else if (use == direction::PIN[1]) {
                if (double_t(ncl[of::POWERING_PIN]) > double_t(cl[of::HIGH_VOLTAGE])) {
                    dir = -1;
                }
            }
        }

        speed *= dir;
        replace(cl[of::MOTOR_SPEED], speed);
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            rotate_cardinal(cr, direction_t(cl[of::FACING]));

            cr->set_source_rgb(.75, .75, .75);
            cr->move_to(200., 128.);
            cr->line_to(256., 128.);
            cr->set_line_width(24.);
            cr->stroke();

            cr->set_source_rgb(.5, .5, .5);
            cr->move_to(16., 72.);
            cr->line_to(32., 56.);
            cr->line_to(192., 56.);
            cr->line_to(208., 72.);
            cr->line_to(208., 256. - 72.);
            cr->line_to(192., 256. - 56.);
            cr->line_to(32., 256. - 56.);
            cr->line_to(16., 256. - 72.);
            cr->fill();
            cr->stroke();

            cr->set_source_rgb(.25, .25, .25);
            cr->move_to(64., 56.);
            cr->line_to(64., 256. - 56.);
            cr->move_to(160., 56.);
            cr->line_to(160., 256. - 56.);
            cr->move_to(64., 64.);
            cr->line_to(160., 64.);
            cr->move_to(64., 80.);
            cr->line_to(160., 80.);
            cr->move_to(64., 102.);
            cr->line_to(160., 102.);
            cr->move_to(64., 128.);
            cr->line_to(160., 128.);
            cr->move_to(64., 256. - 102.);
            cr->line_to(160., 256. - 102.);
            cr->move_to(64., 256. - 80.);
            cr->line_to(160., 256. - 80.);
            cr->move_to(64., 256. - 64.);
            cr->line_to(160., 256. - 64.);
            cr->set_line_width(8.);
            cr->stroke();

            rotate_cardinal(cr, direction::UP);
            cr->translate(0., -16.);
            auto speed = double_t(cl[of::MOTOR_SPEED]);
            if (speed < 0) {
                rotate_cardinal(cr, direction::LEFT);
            }
            if (speed != 0) {
                cr->set_source_rgb(252. / 255., 233. / 255., 79. / 255.);
                cr->move_to(92., 108.);
                cr->line_to(256. - 92. - 32., 108.);
                cr->line_to(256. - 92. - 32.,  92.);
                cr->line_to(256. - 92.,  128.);
                cr->line_to(256. - 92. - 32.,  256. - 92.);
                cr->line_to(256. - 92. - 32., 256. - 108.);
                cr->line_to(92., 256. - 108.);
                cr->close_path();
                cr->fill();
                cr->stroke();
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({ of::FACING,
                     of::MOTOR_SPEED });

    pt.add_connection_uses({{ direction::PIN[0], text("Enable") },
                            { direction::PIN[1], text("Direction") }});

    return pt;
}
