//
// Created by Johannes on 17.07.2020.
//

#ifndef HAR_PARTS_HPP
#define HAR_PARTS_HPP

#include <cairomm/context.h>
#include <gdkmm/pixbuf.h>

#include <har/grid_cell.hpp>
#include <har/part.hpp>

namespace har::parts {
    using ImageType = Glib::RefPtr<Gdk::Pixbuf>;

    enum standard_ids : std::size_t {
        EMPTY = 0u,

        PUSH_BUTTON = 1u,
        SWITCH_BUTTON = 2u,

        LAMP = 3u,
        RGB_LED = 4u,
        SEVEN_SEGMENT = 5u,

        PROXIMITY_SENSOR = 6u,
        COLOR_SENSOR = 7u,
        MOVEMENT_SENSOR = 8u,

        MOTOR = 9u,
        CONVEYOR_BELT = 10u,
        THREAD_ROD = 11u,

        PRODUCER = 12u,
        DESTRUCTOR = 13u,

        BOX_CARGO = 14u,

        DIGITAL_PIN = 15u,
        ANALOG_PIN = 16u,
        CONSTANT_PIN = 17u,
        PWM_PIN = 18u,
        SERIAL_PIN = 19u,

        SMD_BUTTON = 20u,
        SMD_LED = 21u,
        TIMER = 22u,

        DUMMY_PIN = 23u,
        KEYING_PIN = 24u
    };

    enum class pin_mode : uint_t {
        TRI_STATE = 2u,
        OUTPUT = 1u,
        INPUT = 0u
    };

    double_t relative(double_t val, double_t min, double_t max);

    void
    add_properties_for_traits(part & pt, double_t voltage_level, ui_access minimal_vis = ui_access::INVISIBLE);

    void
    draw_pin_base(Cairo::RefPtr<Cairo::Context> & cr, const color & color);

    void
    draw_pin_corners(Cairo::RefPtr<Cairo::Context> & cr, grid_cell & gcl, const color & color);

    void
    draw_pin_center(Cairo::RefPtr<Cairo::Context> & cr);

    void
    draw_socket(Cairo::RefPtr<Cairo::Context> & cr, const color & color);

    void
    draw_segment(const Cairo::RefPtr<Cairo::Context> & cr,
                 const color & seg_color, const color & line_color, double_t length = 1.);

    void
    draw_voltage(const Cairo::RefPtr<Cairo::Context> & cr, cell & cl, const color & color);

    void
    draw_pwm_voltage(const Cairo::RefPtr<Cairo::Context> & cr, cell & cl, const color & color);

    void
    rotate_cardinal(const Cairo::RefPtr<Cairo::Context> & cr, direction_t dir);
}

#endif //HAR_PARTS_HPP
