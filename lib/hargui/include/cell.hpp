//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_CELL_HPP
#define HAR_GUI_CELL_HPP

#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>

namespace har::gui_ {
    class cell : public Gtk::EventBox {
    private:
        Gtk::Image _image;

    public:
        explicit cell(const std::vector<GtkTargetEntry> & tentries);

        void set_name(std::string & name);

        void set_image(Glib::RefPtr<Gdk::Pixbuf> & pixbuf);

        void set_image(Glib::RefPtr<Gdk::Pixbuf> & imgpb, Glib::RefPtr<Gdk::Pixbuf> & dragpb);

        Glib::RefPtr<const Gdk::Pixbuf> get_image() const;

        ~cell() noexcept override;
    };
}

#endif //HAR_GUI_CELL_HPP
