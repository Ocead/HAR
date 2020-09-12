//
// Created by Johannes on 18.07.2020.
//

#include "parts.hpp"

using namespace har;

void parts::add_properties_for_traits(part & pt, double_t voltage_level, ui_access minimal_vis) {
    auto invisible = std::clamp(ui_access::INVISIBLE, minimal_vis, ui_access::CHANGEABLE);
    auto visible = std::clamp(ui_access::VISIBLE, minimal_vis, ui_access::CHANGEABLE);
    auto changeable = std::clamp(ui_access::CHANGEABLE, minimal_vis, ui_access::CHANGEABLE);

    entry::Specifics voltage_spec{ std::array<double_t, 3>{ -voltage_level, voltage_level, .1 }};
    entry::Specifics moving_spec{ std::array<double_t, 3>{ -1., 1., .01 }};

    auto part_traits = pt.traits();

    pt.add_entry(entry{ of::TYPE,
                        text("__TYPE"),
                        text("Type"),
                        value(part_h()),
                        visible,
                        serialize::NO_SERIALIZE });
    pt.add_entry(entry{ of::NAME,
                        text("__NAME"),
                        text("Name"),
                        value(string_t()),
                        changeable,
                        serialize::SERIALIZE });

    if (!(part_traits & traits::SOLID) && part_traits & traits::COMPONENT_PART) {
        pt.add_entry(entry{ of::HAS_CARGO,
                            text("__HAS_CARGO"),
                            text("Has cargo"),
                            value(bool_t()),
                            invisible,
                            serialize::NO_SERIALIZE });
        pt.add_entry(entry{ of::HAS_ARTIFACTS,
                            text("__HAS_ARTIFACTS"),
                            text("Has artifacts"),
                            value(bool_t()),
                            invisible,
                            serialize::NO_SERIALIZE });
    }

    if (part_traits & traits::EMPTY_PART) {
        return;
    }

    if (part_traits & traits::CARGO_PART) {
        pt.add_entry(entry{ of::WEIGHT,
                            text("__WEIGHT"),
                            text("Weight"),
                            value(double_t()),
                            changeable,
                            serialize::SERIALIZE });
    }

    if (part_traits & traits::INPUT) {
        if (part_traits & traits::COMPONENT_PART) {
            pt.add_entry(entry{ of::POWERED_UP,
                                text("__POWERED_UP"),
                                text("Powered (up)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
            pt.add_entry(entry{ of::POWERED_DOWN,
                                text("__POWERED_DOWN"),
                                text("Powered (down)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
            pt.add_entry(entry{ of::POWERED_RIGHT,
                                text("__POWERED_RIGHT"),
                                text("Powered (right)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
            pt.add_entry(entry{ of::POWERED_LEFT,
                                text("__POWERED_LEFT"),
                                text("Powered (left)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
        }
        pt.add_entry(entry{ of::POWERED_PIN,
                            text("__POWERED_PIN"),
                            text("Powered (pin)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            voltage_spec });

        if (part_traits & traits::BOARD_PART) {
            pt.add_entry(entry{ of::INT_HANDLER,
                                text("__INT_HANDLER"),
                                text("Interrupt handler"),
                                value(callback_t()),
                                visible,
                                serialize::SERIALIZE });

            pt.add_entry(entry{ of::INT_CONDITION,
                                text("__INT_CONDITION"),
                                text("Interrupt condition"),
                                value(uint_t()),
                                changeable,
                                serialize::SERIALIZE,
                                {
                                        { 0u, text("None") },
                                        { 1u, text("Rising") },
                                        { 2u, text("Falling") },
                                        { 3u, text("Change") },
                                        { 4u, text("Low") },
                                        { 5u, text("High") },
                                }});
        }
    }

    if (part_traits & traits::OUTPUT) {
        if (part_traits & traits::COMPONENT_PART) {
            pt.add_entry(entry{ of::POWERING_UP,
                                text("__POWERING_UP"),
                                text("Powering (up)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
            pt.add_entry(entry{ of::POWERING_DOWN,
                                text("__POWERING_DOWN"),
                                text("Powering (down)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
            pt.add_entry(entry{ of::POWERING_RIGHT,
                                text("__POWERING_RIGHT"),
                                text("Powering (right)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
            pt.add_entry(entry{ of::POWERING_LEFT,
                                text("__POWERING_LEFT"),
                                text("Powering (left)"),
                                value(double_t()),
                                visible,
                                serialize::NO_SERIALIZE,
                                voltage_spec });
        }
        pt.add_entry(entry{ of::POWERING_PIN,
                            text("__POWERING_PIN"),
                            text("Powering (pin)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            voltage_spec });
    }

    if (part_traits & (traits::INPUT | traits::OUTPUT)) {
        pt.add_entry(entry{ of::HIGH_VOLTAGE,
                            text("__HIGH_VOLTAGE"),
                            text("High voltage"),
                            value(double_t(voltage_level)),
                            changeable,
                            serialize::SERIALIZE,
                            voltage_spec });
        pt.add_entry(entry{ of::LOW_VOLTAGE,
                            text("__LOW_VOLTAGE"),
                            text("Low voltage"),
                            value(double_t()),
                            changeable,
                            serialize::SERIALIZE,
                            voltage_spec });
    }

    if (part_traits & traits::BOARD_PART) {
        if (part_traits & traits::INPUT && part_traits & traits::OUTPUT) {
            pt.add_entry(entry{ of::PIN_MODE,
                                text("__PIN_MODE"),
                                text("Pin mode"),
                                value(uint_t(pin_mode::TRI_STATE)),
                                changeable,
                                serialize::SERIALIZE,
                                {
                                        { uint_t(pin_mode::TRI_STATE), text("Tri-state") },
                                        { uint_t(pin_mode::OUTPUT), text("Output") },
                                        { uint_t(pin_mode::INPUT), text("Input") }
                                }});
        }
        pt.add_entry(entry(of::DESIGN,
                           text("__DESIGN"),
                           text("Design"),
                           value(uint_t()),
                           changeable,
                           serialize::SERIALIZE,
                           {
                                   { 0u, text("Pin") },
                                   { 1u, text("Socket") }
                           }));
        pt.add_entry(entry(of::NEXT_FREE + 1,
                           text("__ORIENTATION"),
                           text("Orientation"),
                           value(uint_t()),
                           changeable,
                           serialize::SERIALIZE,
                           {
                                   { 0u, text("Vertical") },
                                   { 1u, text("Horizontal") }
                           }));
    }

    if (part_traits & traits::MOVING) {
        pt.add_entry(entry{ of::MOVED_UP,
                            text("__MOVED_UP"),
                            text("Moved (up)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOVED_DOWN,
                            text("MOVED_DOWN"),
                            text("Moved (down)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOVED_RIGHT,
                            text("__MOVED_RIGHT"),
                            text("Moved (right)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOVED_LEFT,
                            text("__MOVED_LEFT"),
                            text("Moved (left)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOVING_UP,
                            text("__MOVING_UP"),
                            text("Moving (up)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOVING_DOWN,
                            text("__MOVING_DOWN"),
                            text("Moving (down)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOVING_RIGHT,
                            text("__MOVING_RIGHT"),
                            text("Moving (right)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOVING_LEFT,
                            text("__MOVING_LEFT"),
                            text("Moving (left)"),
                            value(double_t()),
                            visible,
                            serialize::NO_SERIALIZE,
                            moving_spec });
        pt.add_entry(entry{ of::MOTOR_DISTANCE,
                            text("__MOTOR_DISTANCE"),
                            text("Distance to motor"),
                            value(uint_t(std::numeric_limits<uint_t>::max())),
                            visible,
                            serialize::NO_SERIALIZE,
                            std::array<uint_t, 3>{ 0u, std::numeric_limits<uint_t>::max(), 1u }});
        pt.add_entry(entry{ of::MOTOR_DIRECTION,
                            text("__MOTOR_DIRECTION"),
                            text("Direction to motor"),
                            value(direction_t(direction::NONE)),
                            invisible,
                            serialize::NO_SERIALIZE,
                            dir_cat::ALL_DIRECTIONS });
    }

    if (part_traits & traits::ORIENTABLE) {
        if (part_traits & traits::MOVING) {
            pt.add_entry(entry{ of::MOVING_FROM,
                                text("__MOVING_FROM"),
                                text("Moving from"),
                                value(direction_t(direction::RIGHT)),
                                changeable,
                                serialize::ANYWAY,
                                dir_cat::CARDINAL_DIRECTIONS });
            pt.add_entry(entry{ of::MOVING_TO,
                                text("__MOVING_TO"),
                                text("Moving to"),
                                value(direction_t(direction::LEFT)),
                                changeable,
                                serialize::ANYWAY,
                                dir_cat::CARDINAL_DIRECTIONS });
        }
        if (part_traits & traits::COMPONENT_PART) {
            pt.add_entry(entry{ of::FACING,
                                text("__FACING"),
                                text("Facing"),
                                value(direction_t(direction::RIGHT)),
                                changeable,
                                serialize::SERIALIZE,
                                dir_cat::CARDINAL_DIRECTIONS });
        }
    }

    if (part_traits & traits::SENSOR) {
        pt.add_entry(entry{ of::FIRING,
                            text("__FIRING"),
                            text("Firing"),
                            value(bool_t()),
                            visible,
                            serialize::NO_SERIALIZE });
        pt.add_entry(entry{ of::LOGIC_POLARITY,
                            text("__LOGIC_POLARITY"),
                            text("Logic polarity"),
                            value(uint_t()),
                            changeable,
                            serialize::SERIALIZE,
                            {
                                    { 0u, text("Positive") },
                                    { 1u, text("Negative") }
                            }});
    }

    if (part_traits & traits::COLORED) {
        pt.add_entry(entry{ of::COLOR,
                            text("__COLOR"),
                            text("Color"),
                            value(color_t()),
                            changeable,
                            serialize::SERIALIZE });
    }
}

double_t parts::relative(double_t val, double_t min, double_t max) {
    return double_t(val - min) / double_t(max - min);
}

void parts::draw_pin_base(Cairo::RefPtr<Cairo::Context> & cr, const color & color) {
    cr->save();
    cr->translate(128., 128.);
    cr->rotate(45. * M_PI / 180.);
    cr->scale(1.15, 1.15);
    cr->translate(-128., -128.);
    cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
    cr->rectangle(0., 0., 256., 256.);
    cr->fill();
    cr->stroke();
    cr->restore();
}

void parts::draw_pin_corners(Cairo::RefPtr<Cairo::Context> & cr, grid_cell & gcl, const color & color) {
    if (gcl.is_placed()) {
        if (uint_t(gcl[of::NEXT_FREE + 1]) == 0u) {
            if (!(gcl[direction::LEFT].traits() & traits::EMPTY_PART)) {
                cr->save();
                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(0., 0., 96., 256.);
                cr->fill();
                cr->stroke();
                cr->restore();
            }
            if (!(gcl[direction::RIGHT].traits() & traits::EMPTY_PART)) {
                cr->save();
                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(160., 0., 96., 256.);
                cr->fill();
                cr->stroke();
                cr->restore();
            }
        } else {
            if (!(gcl[direction::UP].traits() & traits::EMPTY_PART)) {
                cr->save();
                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(0., 0., 256., 96.);
                cr->fill();
                cr->stroke();
                cr->restore();
            }
            if (!(gcl[direction::DOWN].traits() & traits::EMPTY_PART)) {
                cr->save();
                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(0., 160., 256., 96.);
                cr->fill();
                cr->stroke();
                cr->restore();
            }
        }
    }
}

void parts::draw_pin_center(Cairo::RefPtr<Cairo::Context> & cr) {
    cr->save();
    cr->set_source_rgb(0xf3 / 256., 0xe7 / 256., 0xa5 / 256.);
    cr->rectangle(128. - 16., 128. - 16., 32, 32);
    cr->fill();
    cr->stroke();

    cr->set_source_rgb(0xf5 / 256., 0xf0 / 256., 0xd5 / 256.);
    cr->move_to(128. - 32., 128. - 32.);
    cr->line_to(128. + 32., 128. - 32);
    cr->line_to(128. + 16., 128. - 16.);
    cr->line_to(128. - 16., 128. - 16.);
    cr->fill();
    cr->stroke();

    cr->set_source_rgb(0xe2 / 256., 0xb0 / 256., 0x07 / 256.);
    cr->move_to(128. + 32., 128. + 32.);
    cr->line_to(128. - 32., 128. + 32);
    cr->line_to(128. - 16., 128. + 16.);
    cr->line_to(128. + 16., 128. + 16.);
    cr->fill();
    cr->stroke();

    cr->set_source_rgb(0xfe / 256., 0xf6 / 256., 0xcc / 256.);
    cr->move_to(128. - 32., 128. - 32.);
    cr->line_to(128. - 32., 128. + 32);
    cr->line_to(128. - 16., 128. + 16.);
    cr->line_to(128. - 16., 128. - 16.);
    cr->fill();
    cr->stroke();

    cr->set_source_rgb(0xee / 256., 0xde / 256., 0x80 / 256.);
    cr->move_to(128. + 32., 128. + 32.);
    cr->line_to(128. + 32., 128. - 32);
    cr->line_to(128. + 16., 128. - 16.);
    cr->line_to(128. + 16., 128. + 16.);
    cr->fill();
    cr->stroke();
    cr->restore();
}

void parts::draw_socket(Cairo::RefPtr<Cairo::Context> & cr, const color & color) {
    cr->save();

    cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
    cr->rectangle(0., 0., 256., 256.);
    cr->fill();
    cr->stroke();

    cr->set_operator(Cairo::OPERATOR_OVER);
    cr->set_source_rgba(0., 0., 0., .50);
    cr->rectangle(128. - 48., 128. - 48., 96., 96.);
    cr->fill();
    cr->stroke();

    cr->set_source_rgba(.75, .75, .75, .35);
    cr->move_to(0., 0.);
    cr->line_to(256., 0.);
    cr->line_to(128. + 48., 128. - 48.);
    cr->line_to(128. - 48., 128. - 48.);
    cr->fill();
    cr->stroke();

    cr->set_source_rgba(.75, .75, .75, .20);
    cr->move_to(0., 0.);
    cr->line_to(0., 256.);
    cr->line_to(128. - 48., 128. + 48.);
    cr->line_to(128. - 48., 128. - 48.);
    cr->fill();
    cr->stroke();

    cr->set_source_rgba(.25, .25, .25, .35);
    cr->move_to(0., 256.);
    cr->line_to(256., 256.);
    cr->line_to(128. + 48., 128. + 48.);
    cr->line_to(128. - 48., 128. + 48.);
    cr->fill();
    cr->stroke();

    cr->set_source_rgba(.25, .25, .25, .20);
    cr->move_to(256., 0.);
    cr->line_to(256., 256.);
    cr->line_to(128. + 48., 128. + 48.);
    cr->line_to(128. + 48., 128. - 48.);
    cr->fill();
    cr->stroke();

    cr->restore();
}

void parts::draw_segment(const Cairo::RefPtr<Cairo::Context> & cr, const color & seg_color, const color & line_color,
                         double_t length) {
    cr->move_to(-48. * length, 0.);
    cr->line_to(-32. * length, -16.);
    cr->line_to(32. * length, -16.);
    cr->line_to(48. * length, 0.);
    cr->line_to(32. * length, 16.);
    cr->line_to(-32. * length, 16.);
    cr->close_path();
}

void parts::draw_voltage(const Cairo::RefPtr<Cairo::Context> & cr, cell & cl, const color & color) {
    double_t high = double_t(cl[HIGH_VOLTAGE]);
    double_t low = double_t(cl[LOW_VOLTAGE]);
    double_t vol = 0.;
    uint_t mode;
    if (cl.has(PIN_MODE)) {
        mode = uint_t(cl[PIN_MODE]);
        switch (mode) {
            case uint_t(pin_mode::OUTPUT):
                vol = double_t(cl[POWERING_PIN]);
                break;
            case uint_t(pin_mode::INPUT):
                vol = double_t(cl[POWERED_PIN]);
                break;
            default:
                break;
        }
    } else {
        vol = double_t(cl[POWERING_PIN]);
        mode = 1u;
    }

    if (high != 0. && mode != uint_t(pin_mode::TRI_STATE)) {
        cr->save();
        if (int_t(color.r) + int_t(color.g) + int_t(color.b) > 384) {
            cr->set_source_rgba(0., 0., 0., .85);
        } else {
            cr->set_source_rgba(1., 1., 1., .85);
        }
        cr->set_line_width(24.);
        cr->set_line_cap(Cairo::LINE_CAP_ROUND);
        if (low - vol < .1 && vol - high < .1) {
            double_t pos = .9 - relative(vol, low, high) * .8;
            cr->move_to(256. * .15, 256. * pos);
            cr->line_to(256. * .85, 256. * pos);
        } else {
            cr->move_to(128., 256. * .1);
            cr->line_to(64., 128. + 16.);
            cr->line_to(256. - 64., 128. - 16.);
            cr->line_to(128., 256. * .9);
        }
        cr->stroke();
        cr->restore();
    }
}

void parts::draw_pwm_voltage(const Cairo::RefPtr<Cairo::Context> & cr, cell & cl, const color & color) {
    double_t high = double_t(cl[HIGH_VOLTAGE]);
    double_t low = double_t(cl[LOW_VOLTAGE]);
    double_t vol = double_t(cl[PWM_VOLTAGE]);
    double_t duty = double_t(cl[PWM_DUTY]);
    uint_t mode = uint_t(cl[PIN_MODE]);

    if (high != 0. && mode != uint_t(pin_mode::TRI_STATE)) {
        cr->save();
        if (int_t(color.r) + int_t(color.g) + int_t(color.b) > 384) {
            cr->set_source_rgba(0., 0., 0., .85);
        } else {
            cr->set_source_rgba(1., 1., 1., .85);
        }
        cr->set_line_width(24.);
        cr->set_line_cap(Cairo::LINE_CAP_ROUND);
        if (low - vol < .1 && vol - high < .1 && low < high) {
            double_t pos = .9 - relative(vol, low, high) * .8;
            double_t n_pos = .9 - relative(std::max(0., low), low, high) * .8;
            if (duty < .01) {
                cr->move_to(256. * .15, 256. * n_pos);
                cr->line_to(256. * .85, 256. * n_pos);
            } else if (duty >= 1.) {
                cr->move_to(256. * .15, 256. * pos);
                cr->line_to(256. * .85, 256. * pos);
            } else {
                double_t d_pos = duty * .7;
                cr->move_to(256. * .15, 256. * n_pos);
                cr->line_to(128. - 128. * d_pos, 256. * n_pos);
                cr->line_to(128. - 128. * d_pos, 256. * pos);
                cr->line_to(128. + 128. * d_pos, 256. * pos);
                cr->line_to(128. + 128. * d_pos, 256. * n_pos);
                cr->line_to(256. * .85, 256. * n_pos);
            }
        } else {
            cr->move_to(128., 256. * .1);
            cr->line_to(64., 128. + 16.);
            cr->line_to(256. - 64., 128. - 16.);
            cr->line_to(128., 256. * .9);
        }
        cr->stroke();
        cr->restore();
    }
}

void parts::rotate_cardinal(const Cairo::RefPtr<Cairo::Context> & cr, direction_t dir) {
    cr->translate(128., 128.);
    switch (dir) {
        case raw_direction::UP: {
            cr->rotate(-M_PI_2);
            break;
        }
        case raw_direction::DOWN: {
            cr->rotate(M_PI_2);
            break;
        }
        case raw_direction::RIGHT: {
            break;
        }
        case raw_direction::LEFT: {
            cr->rotate(M_PI);
            break;
        }
        default: {
            break;
        }
    }
    cr->translate(-128., -128.);
}