//
// Created by Johannes on 21.11.2020.
//

#include <iomanip>

#include <har/types.hpp>

#include "timing_control.hpp"

using namespace std::chrono_literals;
using namespace har;
using namespace har::gui_;

timing_control::timing_control(std::chrono::microseconds us) : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
                                                               _entry(), _units(), _lock(),
                                                               _timing() {
    _entry.set_width_chars(8);
    _entry.set_alignment(Gtk::ALIGN_END);
    _entry.signal_activate().connect([this]() {
        update_timing(true);
    });

    _units.insert(0, "s", "s");
    _units.insert(1, "ms", "ms");
    _units.insert(2, "us", "µs");
    _units.insert(3, "Hz", "Hz");
    _units.set_active_id("ms");

    _units.signal_changed().connect([this]() {
        update_timing(false);
    });

    _lock.set_image_from_icon_name("changes-prevent-symbolic", Gtk::ICON_SIZE_BUTTON);

    set_timing(us);

    pack_start(_entry, Gtk::PACK_EXPAND_WIDGET);
    pack_end(_lock, Gtk::PACK_SHRINK);
    pack_end(_units, Gtk::PACK_SHRINK);
    get_style_context()->add_class("linked");
    show_all_children();
}

void timing_control::update_timing(bool_t updating) {
    double_t nom = updating ? std::stod(_entry.get_text()) : _timing.count();
    switch (_units.get_active_row_number()) {
        case 0: {
            nom *= 1.e6;
            break;
        }
        case 1: {
            nom *= 1.e3;
            break;
        }
        case 2: {
            nom *= 1.;
            break;
        }
        case 3: {
            nom = (1. / nom) * 1.e6;
            break;
        }
    }

    if (updating) {
        _timing = std::chrono::microseconds(std::lround(nom));
        if (_on_change_fun) {
            _on_change_fun(_timing);
        }
    }

    double_t rep = updating ? nom : _timing.count();
    int_t precision;
    switch (_units.get_active_row_number()) {
        case 0: {
            rep = _timing.count() / 1.e6;
            precision = 4;
            break;
        }
        case 1: {
            rep = _timing.count() / 1.e3;
            precision = 2;
            break;
        }
        case 2: {
            rep = _timing.count();
            precision = 0;
            break;
        }
        case 3: {
            rep = 1. / (_timing.count() / 1.e6);
            precision = 2;
            break;
        }
    }

    stringstream ss;
    ss << std::fixed << std::setprecision(precision) << rep;
    _entry.set_text(ss.str());
}

decltype(timing_control::_on_change_fun) & timing_control::on_change_fun() {
    return _on_change_fun;
}

std::chrono::microseconds timing_control::get_timing() const {
    return _timing;
}

void timing_control::set_timing(std::chrono::microseconds timing) {
    _timing = timing;
    update_timing(false);
}

timing_control::~timing_control() noexcept = default;
