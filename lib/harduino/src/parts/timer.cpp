//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

typedef enum : uint_t {
    NEVER = 0u,
    RISING = 1u,
    FALLING = 2u,
    CHANGE = 3u,
    LOW = 4u,
    HIGH = 5u,
    ALWAYS = 6u
} inc_condition;

part duino::parts::timer(part_h offset) {
    part pt{ PART[standard_ids::TIMER + offset],
             text("har:timer"),
             traits::BOARD_PART | traits::INPUT | traits::COLORED,
             text("Timer") };

    add_properties_for_traits(pt, 5.);

    pt.remove_entry(of::DESIGN);
    pt.remove_entry(of::NEXT_FREE + 1);
    pt.remove_entry(of::INT_HANDLER);
    pt.remove_entry(of::INT_CONDITION);

    pt.add_entry(entry{ of::COLOR,
                        text("__COLOR"),
                        text("Color"),
                        value(color_t(238u, 238u, 236u, 255u)),
                        ui_access::CHANGEABLE,
                        serialize::SERIALIZE });

    pt.add_entry(entry{ of::NEXT_FREE + 2,
                        text("__INC_CONDITION"),
                        text("Increment condition"),
                        value(uint_t()),
                        ui_access::CHANGEABLE,
                        serialize::SERIALIZE,
                        {
                                { NEVER, text("Never") },
                                { RISING, text("Rising") },
                                { FALLING, text("Falling") },
                                { CHANGE, text("Change") },
                                { LOW, text("Low") },
                                { HIGH, text("High") },
                                { ALWAYS, text("Always") }
                        }});

    pt.add_entry(entry{ of::VALUE,
                        text("__VALUE"),
                        text("Timer value"),
                        value(uint_t()),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE,
                        std::array<uint_t, 3>{ 0, std::numeric_limits<uint_t>::max(), 1 }});

    pt.add_entry(entry{ of::MAX_VALUE,
                        text("__MAX_VALUE"),
                        text("Max. timer value"),
                        value(uint_t(1)),
                        ui_access::CHANGEABLE,
                        serialize::SERIALIZE,
                        std::array<uint_t, 3>{ 1, std::numeric_limits<uint_t>::max(), 1 }});

    pt.delegates.cycle = [](cell & cl) {
        auto connected = cl.as_grid_cell().connected();
        auto powered = double_t(cl[of::POWERED_PIN]);
        auto high = double_t(cl[of::HIGH_VOLTAGE]);
        auto value = uint_t(cl[of::VALUE]);
        auto max_value = uint_t(cl[of::MAX_VALUE]) + 1;
        auto condition = inc_condition(uint_t(cl[of::NEXT_FREE + 2]));

        if (connected.empty()) {
            switch (condition) {
                case inc_condition::NEVER:
                case inc_condition::RISING:
                case inc_condition::HIGH:{
                    break;
                }
                case inc_condition::FALLING:
                case inc_condition::CHANGE: {
                    if (powered >= high) {
                        value = (value + 1) % max_value;
                        cl[of::VALUE] = value;
                    }
                    break;
                }
                case inc_condition::LOW:
                case inc_condition::ALWAYS: {
                    value = (value + 1) % max_value;
                    cl[of::VALUE] = value;
                }
                default: {
                    break;
                }
            }
            if (powered != 0.) {
                cl[of::POWERED_PIN] = 0.;
            }
        } else {
            for (auto &[use, ncl] : connected) {
                auto powering = double_t(ncl[of::POWERING_PIN]);
                switch (condition) {
                    case inc_condition::NEVER: {
                        break;
                    }
                    case inc_condition::RISING: {
                        if (powered < high && powering >= high) {
                            value = (value + 1) % max_value;
                            cl[of::VALUE] = value;
                        }
                        break;
                    }
                    case inc_condition::FALLING: {
                        if (powered >= high && powering < high) {
                            value = (value + 1) % max_value;
                            cl[of::VALUE] = value;
                        }
                        break;
                    }
                    case inc_condition::CHANGE: {
                        if ((powered < high && powering >= high) || (powered >= high && powering < high)) {
                            value = (value + 1) % max_value;
                            cl[of::VALUE] = value;
                        }
                        break;
                    }
                    case inc_condition::LOW: {
                        if (powering < high) {
                            value = (value + 1) % max_value;
                            cl[of::VALUE] = value;
                        }
                        break;
                    }
                    case inc_condition::HIGH: {
                        if (powering >= high) {
                            value = (value + 1) % max_value;
                            cl[of::VALUE] = value;
                        }
                        break;
                    }
                    case inc_condition::ALWAYS: {
                        value = (value + 1) % max_value;
                        cl[of::VALUE] = value;
                    }
                    default: {
                        break;
                    }
                }
                cl[of::POWERED_PIN] = powering;
            }
        }
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto color = cl.as_grid_cell().is_placed() ? color_t(cl[of::COLOR])
                                                       : color_t(238u, 238u, 236u, 255u);
            auto bright = (int_t(color.r) + int_t(color.g) + int_t(color.b) > 384);

            cr->set_source_rgb(color.r / 256., color.g / 256., color.b / 256.);
            cr->arc(128., 128., 112., 0., 2 * M_PI);
            cr->fill_preserve();
            if (bright) {
                cr->set_source_rgb(0., 0., 0.);
            } else {
                cr->set_source_rgb(1., 1., 1.);
            }
            cr->set_line_width(16.);
            cr->stroke();

            auto max_time = uint_t(cl[of::MAX_VALUE]);
            auto time = uint_t(cl[of::VALUE]);

            double diff = std::clamp(double(time) / double(max_time), 0., 1.);

            cr->set_line_width(80.);
            cr->arc(128., 128., 40., -M_PI_2, 2 * M_PI * diff - M_PI_2);
            if (bright) {
                cr->set_source_rgb(0., 0., 0.);
            } else {
                cr->set_source_rgb(1., 1., 1.);
            }
            cr->stroke();

            if (bright) {
                cr->set_source_rgb(0., 0., 0.);
            } else {
                cr->set_source_rgb(1., 1., 1.);
            }
            cr->set_line_width(8.);
            cr->move_to(128., 32.);
            cr->line_to(128., 64.);
            cr->stroke();

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({
                           of::COLOR,
                           of::VALUE,
                           of::MAX_VALUE
                   });

    pt.add_connection_use(direction::PIN, text("Clock"));

    return pt;
}
