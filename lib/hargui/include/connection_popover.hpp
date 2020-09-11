//
// Created by Johannes on 26.07.2020.
//

#ifndef HAR_CONNECTION_POPOVER_HPP
#define HAR_CONNECTION_POPOVER_HPP

#include <gtkmm/image.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <gtkmm/popover.h>
#include <gtkmm/togglebutton.h>

#include <har/full_cell.hpp>
#include <har/participant.hpp>

namespace har::gui_ {
    class add_connection_row : public Gtk::ListBoxRow {
    private:
        const direction_t _dir;

    public:
        add_connection_row(direction_t dir);

        direction_t dir() const;

        ~add_connection_row() noexcept;
    };

    class connection_popover : public Gtk::Popover {
    private:
        Gtk::ToggleButton _btn_cl1;
        Gtk::Image _img_cl1;
        Gtk::ToggleButton _btn_cl2;
        Gtk::Image _img_cl2;
        Gtk::ListBox _list;

        std::map<direction_t, add_connection_row> _rows;

        std::unique_ptr<participant::context> _ctx;
        std::unique_ptr<full_grid_cell> _fgcl1;
        std::unique_ptr<full_grid_cell> _fgcl2;

        void select_target(full_grid_cell & fgcl);

        void select_connection_use(direction_t dir);

        void on_hide() override;

    public:
        connection_popover();

        void set_cell(std::unique_ptr<participant::context> && ctx,
                      std::unique_ptr<full_grid_cell> && fgcl1, Glib::RefPtr<const Gdk::Pixbuf> pixbuf1,
                      std::unique_ptr<full_grid_cell> && fgcl2, Glib::RefPtr<const Gdk::Pixbuf> pixbuf2);

        void unselect_target();

        ~connection_popover() noexcept override;
    };

}

#endif //HAR_CONNECTION_POPOVER_HPP
