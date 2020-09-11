//
// Created by Johannes on 10.09.2020.
//

#include "about.hpp"

using namespace har::gui_;

about::about() : Gtk::AboutDialog() {
    set_program_name("HAR");
    set_version("2.0 beta");
    set_license_type(Gtk::LICENSE_BSD);
}

about::~about() noexcept = default;