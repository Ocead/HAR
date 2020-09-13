//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::conveyor_belt(part_h offset) {
    part pt{ PART[standard_ids::CONVEYOR_BELT + offset],
             text("har:conveyor_belt"),
             traits::COMPONENT_PART |
             traits::MOVING |
             traits::ORIENTABLE |
             traits::COLORED,
             text("Conveyor belt") };

    add_properties_for_traits(pt, 5.);

    pt.remove_entry(of::FACING);

    pt.delegates.init_relative = [](cell & cl) {
        auto & gcl = cl.as_grid_cell();
        uint_t from_dist = std::numeric_limits<uint_t>::max();
        uint_t to_dist = std::numeric_limits<uint_t>::max();

        for (auto dir : direction::cardinal) {
            auto & ncl = gcl[dir];
            if (ncl.is_placed()) {
                if (ncl.has(of::MOVING_TO) && direction_t(ncl[of::MOVING_TO]) == !dir) {
                    if (ncl.has(of::MOTOR_DISTANCE)) {
                        auto nfrom_dist = uint_t(ncl[of::MOTOR_DISTANCE]);
                        if (nfrom_dist + 1 < from_dist) {
                            from_dist = nfrom_dist + 1;
                            cl[of::MOVING_FROM] = dir;
                        }
                    } else {
                        cl[of::MOVING_FROM] = dir;
                    }
                }
                if (ncl.has(of::MOVING_FROM) && direction_t(ncl[of::MOVING_FROM]) == !dir) {
                    if (ncl.has(of::MOTOR_DISTANCE)) {
                        auto nto_dist = uint_t(ncl[of::MOTOR_DISTANCE]);
                        if (nto_dist + 1 < to_dist) {
                            to_dist = nto_dist + 1;
                            cl[of::MOVING_TO] = dir;
                        }
                    } else {
                        cl[of::MOVING_TO] = dir;
                    }
                }
            }
        }
        if (from_dist == std::numeric_limits<uint_t>::max() && !to_dist) {
            cl[of::MOVING_FROM] = !direction_t(cl.get(of::MOVING_TO, true));
        }
        if (to_dist == std::numeric_limits<uint_t>::max() && !from_dist) {
            cl[of::MOVING_TO] = !direction_t(cl.get(of::MOVING_FROM, true));
        }
        if (!from_dist) {
            cl[of::MOTOR_DISTANCE] = (!to_dist) ? std::numeric_limits<uint_t>::max() : to_dist;
        } else if (!to_dist) {
            cl[of::MOTOR_DISTANCE] = from_dist;
        } else {
            cl[of::MOTOR_DISTANCE] = std::numeric_limits<uint_t>::max();
        }
    };

    pt.delegates.cycle = [](cell & cl) {
        auto & gcl = cl.as_grid_cell();
        uint_t distance = std::numeric_limits<uint_t>::max();
        direction_t motor_dir = direction::NONE;

        for (auto dir : direction::cardinal) {
            double_t speed;
            auto & ncl = gcl[dir];

            do {
                if (ncl.has(of::MOTOR_SPEED) && direction_t(ncl[of::FACING]) == !dir) {
                    speed = double_t(ncl[MOTOR_SPEED]);
                    distance = 0u;
                    motor_dir = dir;
                } else if (ncl.has(of::MOVING_TO)) {
                    double_t dir_fac;
                    if (ncl[of::MOVING_TO] == value(!dir)) {
                        dir_fac = 1;
                    } else if (ncl[of::MOVING_FROM] == value(!dir)) {
                        dir_fac = -1;
                    } else {
                        speed = 0.;
                        motor_dir = direction::NONE;
                        break;
                    }

                    auto ndistance = uint_t(ncl[of::MOTOR_DISTANCE]);
                    if (ndistance + 1 <= distance &&
                        ndistance != std::numeric_limits<uint_t>::max() &&
                        direction_t(ncl[of::MOTOR_DIRECTION]) != !dir) {
                        speed = double_t(ncl[value::moving(!dir)]) * dir_fac;
                        distance = ndistance + 1;
                        motor_dir = dir;
                    } else {
                        speed = double_t(ncl[value::moving(!dir)]) * dir_fac;
                        break;
                    }
                } else {
                    speed = 0.;
                    break;
                }
            } while (false);

            replace(cl[value::moved(dir)], speed);
            if (motor_dir == dir) {
                replace(cl[value::moving(direction_t(cl[of::MOVING_TO]))], speed);
                replace(cl[value::moving(direction_t(cl[of::MOVING_FROM]))], -speed);
            }
        }

        if (motor_dir == direction::NONE) {
            replace(cl[value::moving(direction_t(cl[of::MOVING_TO]))], 0.);
            replace(cl[value::moving(direction_t(cl[of::MOVING_FROM]))], -0.);
        }

        replace(cl[of::MOTOR_DISTANCE], (motor_dir != direction::NONE) ? distance : std::numeric_limits<uint_t>::max());
        replace(cl[of::MOTOR_DIRECTION], motor_dir);
    };

    pt.delegates.move = [](cell & cl) {
        for (auto &[num, cell] : cl.as_grid_cell().cargo()) {

        }
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto from = direction_t(cl[of::MOVING_FROM]);
            auto to = direction_t(cl[of::MOVING_TO]);

            rotate_cardinal(cr, from);

            auto color = color_t(cl[of::COLOR]);

            if (from == !to) {
                cr->set_source_rgb(.2, .2, .2);
                cr->rectangle(28., 16., 8., 256. - 32.);
                cr->rectangle(92., 16., 8., 256. - 32.);
                cr->rectangle(156., 16., 8., 256. - 32.);
                cr->rectangle(220., 16., 8., 256. - 32.);
                cr->fill();
                cr->stroke();

                cr->set_source_rgb(color.r / 255., color.g / 255., color.b / 255.);
                cr->rectangle(-16., 32., 256. + 32., 256. - 64.);
                cr->set_line_width(16.);
                cr->stroke();

                cr->set_source_rgb(.5, .5, .5);
                cr->rectangle(16., 48., 32., 256. - 96.);
                cr->rectangle(80., 48., 32., 256. - 96.);
                cr->rectangle(144., 48., 32., 256. - 96.);
                cr->rectangle(208., 48., 32., 256. - 96.);
                cr->fill();
                cr->stroke();
            } else if (from == cw(to)) {
                cr->set_source_rgb(color.r / 255., color.g / 255., color.b / 255.);
                cr->arc(256., 0., 256. - 32., 0., 2 * M_PI);
                cr->arc(256., 0., 32., 0., 2 * M_PI);
                cr->set_line_width(16.);
                cr->stroke();
            } else if (from == ccw(to)) {
                cr->set_source_rgb(color.r / 255., color.g / 255., color.b / 255.);
                cr->arc(256., 256., 256. - 32., 0., 2 * M_PI);
                cr->arc(256., 256., 32., 0., 2 * M_PI);
                cr->set_line_width(16.);
                cr->stroke();
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({ of::COLOR,
                     of::MOVING_UP,
                     of::MOVING_DOWN,
                     of::MOVING_RIGHT,
                     of::MOVING_LEFT,
                     of::MOVING_FROM,
                     of::MOVING_TO,
                     of::MOTOR_SPEED });

    return pt;
}
