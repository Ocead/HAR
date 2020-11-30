//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

enum channels {
    RED = of::VALUE1,
    GREEN = of::VALUE2,
    BLUE = of::VALUE3
};

part duino::parts::rgb_led(part_h offset) {
    part pt{ PART[standard_ids::RGB_LED + offset],
             text("har:rgb_led"),
             traits::COMPONENT_PART | traits::INPUT,
             text("RGB LED") };

    add_properties_for_traits(pt, 5.);

    pt.add_entry(entry{ of(RED),
                        text("__RED_CHANNEL"),
                        text("Red channel"),
                        value(uint_t()),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE,
                        std::array<uint_t, 3>{ 0, 255, 1 }});

    pt.add_entry(entry{ of(GREEN),
                        text("__GREEN_CHANNEL"),
                        text("Green channel"),
                        value(uint_t()),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE,
                        std::array<uint_t, 3>{ 0, 255, 1 }});

    pt.add_entry(entry{ of(BLUE),
                        text("__BLUE_CHANNEL"),
                        text("Green channel"),
                        value(uint_t()),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE,
                        std::array<uint_t, 3>{ 0, 255, 1 }});

    pt.delegates.cycle = [](cell & cl) {
        double_t high = double_t(cl[of::HIGH_VOLTAGE]);
        for (auto &[use, ncl] : cl.as_grid_cell().connected()) {
            switch (use) {
                case direction::PIN[0]: {
                    if (ncl.has(of::POWERING_PIN)) {
                        cl[of::POWERED_PIN] = ncl[of::POWERING_PIN];
                    }
                    break;
                }
                case direction::PIN[1]: {
                    if (ncl.has(of::POWERING_PIN)) {
                        cl[of(RED)] = uint_t(double_t(ncl[of::POWERING_PIN]) / high * 255);
                    }
                    break;
                }
                case direction::PIN[2]: {
                    if (ncl.has(of::POWERING_PIN)) {
                        cl[of(GREEN)] = uint_t(double_t(ncl[of::POWERING_PIN]) / high * 255);
                    }
                    break;
                }
                case direction::PIN[3]: {
                    if (ncl.has(of::POWERING_PIN)) {
                        cl[of(BLUE)] = uint_t(double_t(ncl[of::POWERING_PIN]) / high * 255);
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(ImageType)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);
            auto cr = Cairo::Context::create(sf);

            auto & gcl = cl.as_grid_cell();

            color_t color{ 0u, 0u, 0u, 255u };

            if (gcl.is_placed()) {
                color.r = uint_t(gcl[of(RED)]);
                color.g = uint_t(gcl[of(GREEN)]);
                color.b = uint_t(gcl[of(BLUE)]);
            }

            cr->set_source_rgb(.2, .2, .2);
            cr->rectangle(24., 24., 256. - 48., 256. - 48.);
            cr->fill_preserve();
            cr->set_source_rgb(.5, .5, .5);
            cr->set_line_width(16.);
            cr->stroke();

            if (gcl.is_placed()) {
                cr->set_source_rgb(color.r / 256., color.g / 256., color.b / 256.);
                cr->rectangle(48., 48., 256. - 96., 256. - 96.);
                cr->fill();
                cr->stroke();
            } else {
                static Cairo::RefPtr<Cairo::LinearGradient> pattern;
                if (!pattern) {
                    pattern = Cairo::LinearGradient::create(0., 0., 256., 0.);
                    pattern->add_color_stop_rgb(.8, 0., 0., 1.);
                    pattern->add_color_stop_rgb(.65, 0., 1., 1.);
                    pattern->add_color_stop_rgb(.5, 0., 1., 0.);
                    pattern->add_color_stop_rgb(.35, 1., 1., 0.);
                    pattern->add_color_stop_rgb(.2, 1., 0., 0.);
                }

                cr->set_source(pattern);
                cr->rectangle(48., 48., 256. - 96., 256. - 96.);
                cr->fill();
                cr->stroke();
            }

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
                           of::POWERED_PIN,
                           of::POWERING_PIN,
                           of(RED),
                           of(GREEN),
                           of(BLUE)
                   });

    pt.add_connection_uses({
                                   { direction::PIN[1], text("Red channel") },
                                   { direction::PIN[2], text("Green channel") },
                                   { direction::PIN[3], text("Blue channel") }
                           });

    return pt;
}
