//
// Created by Johannes on 28.06.2020.
//

#include <iostream>

#include <cell.hpp>

using namespace har::gui_;

cell::cell(const std::vector<GtkTargetEntry> & tentries) : Gtk::EventBox(), _image() {
    Gtk::EventBox::add(_image);

    {
        //In my defense: Gtk::Widget::drag_source_set accepts a `std::vector` that clashes
        // with the `-D_GLIBCXX_DEBUG` replacement `std::__debug::vector`
        gtk_drag_source_set(reinterpret_cast<GtkWidget *>(gobj()),
                            GdkModifierType(GDK_MODIFIER_MASK),
                            tentries.data(),
                            tentries.size(),
                            GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));

        gtk_drag_dest_set(reinterpret_cast<GtkWidget *>(gobj()),
                          GTK_DEST_DEFAULT_ALL,
                          tentries.data(),
                          tentries.size(),
                          GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));
    }

    Gtk::EventBox::set_can_focus(true);
    Gtk::EventBox::show_all_children();
}

void cell::set_name(std::string & name) {
    Gtk::EventBox::set_tooltip_text(name);
}

void cell::set_image(Glib::RefPtr<Gdk::Pixbuf> & pixbuf) {
    drag_source_set_icon(pixbuf);
    _image.set(pixbuf);
    show_all_children();
}

void cell::set_image(Glib::RefPtr<Gdk::Pixbuf> & imgpb, Glib::RefPtr<Gdk::Pixbuf> & dragpb) {
    drag_source_set_icon(dragpb);
    _image.set(imgpb);
    show_all_children();
}

Glib::RefPtr<const Gdk::Pixbuf> cell::get_image() const {
    return _image.get_pixbuf();
}

cell::~cell() noexcept = default;
