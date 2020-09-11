//
// Created by Johannes on 17.07.2020.
//

#include "action_bar.hpp"

using namespace har::gui_;

action_bar::action_bar() : Gtk::ActionBar(),
                           _run(),
                           _step(),
                           _stop(),
                           _updating(false) {
    _run.set_image_from_icon_name("media-playback-start-symbolic", Gtk::ICON_SIZE_BUTTON);
    _step.set_image_from_icon_name("zoom-original-symbolic", Gtk::ICON_SIZE_BUTTON);
    _stop.set_image_from_icon_name("media-playback-stop-symbolic", Gtk::ICON_SIZE_BUTTON);

    _run.set_tooltip_text("Run simulation");
    _step.set_tooltip_text("Make single step");
    _stop.set_tooltip_text("Stop simulation");

    auto & box = *new Gtk::ButtonBox(Gtk::ORIENTATION_HORIZONTAL);
    box.pack_start(_run, Gtk::PACK_SHRINK);
    box.pack_start(_step, Gtk::PACK_SHRINK);
    box.pack_start(_stop, Gtk::PACK_SHRINK);
    box.set_layout(Gtk::BUTTONBOX_EXPAND);

    _run.signal_toggled().connect([&]() {
        if (_run.get_active()) {
            _updating = true;
            _stop.set_active(false);
            _updating = false;
            if (_run_fun) {
                _run_fun();
            }
        } else if (!_updating) {
            _run.set_active(true);
        }
    });

    _step.signal_clicked().connect([&]() {
        if (_step_fun)
            _step_fun();
    });

    _stop.signal_toggled().connect([&]() {
        if (_stop.get_active()) {
            _updating = true;
            _run.set_active(false);
            _updating = false;
            if (_stop_fun) {
                _stop_fun();
            }
        } else if (!_updating) {
            _stop.set_active(true);
        }
    });

    Gtk::ActionBar::set_center_widget(*Gtk::manage(&box));

    show_all_children();
}

void action_bar::set_run() {
    _run.set_active(true);
}

void action_bar::set_step() {
    _step.clicked();
}

void action_bar::set_stop() {
    _stop.set_active(true);
}

std::function<void()> & action_bar::run_fun() {
    return _run_fun;
}

std::function<void()> & action_bar::step_fun() {
    return _step_fun;
}

std::function<void()> & action_bar::stop_fun() {
    return _stop_fun;
}

action_bar::~action_bar() noexcept = default;
