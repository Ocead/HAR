//
// Created by Johannes on 17.07.2020.
//

#ifndef HAR_GUI_PART_HPP
#define HAR_GUI_PART_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

namespace har::gui_ {
    class part : public Gtk::Button {
    private:
        Gtk::Box _box;
        Gtk::Image _image;
        Gtk::Label _label;

        Cairo::RefPtr<Cairo::Surface> _surface;

    public:
        part();

        void set_image(Glib::RefPtr<Gdk::Pixbuf> & pixbuf);

        void set_label(const std::string & label);

        ~part() noexcept override;
    };
}

#endif //HAR_GUI_PART_HPP
