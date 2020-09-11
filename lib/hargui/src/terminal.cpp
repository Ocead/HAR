//
// Created by Johannes on 04.08.2020.
//

#include "terminal.hpp"

using namespace har::gui_;

terminal::terminal() : Gtk::TextView() {
    Gtk::TextView::set_monospace(true);
}

terminal::~terminal() noexcept = default;
