//
// Created by Johannes on 17.07.2020.
//

#include "part.hpp"

using namespace har::gui_;

part::part() : Gtk::Button(), _box(Gtk::ORIENTATION_VERTICAL), _image(), _label() {
    _box.pack_start(_image, Gtk::PACK_EXPAND_WIDGET);
    _box.pack_end(_label, Gtk::PACK_EXPAND_WIDGET);
    _image.set_hexpand(true);
    _label.set_hexpand(true);

    Gtk::Button::set_relief(Gtk::RELIEF_NONE);
    Gtk::Button::add(_box);

    show_all_children();
}

void part::set_image(Glib::RefPtr<Gdk::Pixbuf> & pixbuf) {
    _image.set(pixbuf);
}

void part::set_label(const std::string & label) {
    _label.set_text(label);
}

part::~part() noexcept = default;
