//
// Created by Johannes on 26.07.2020.
//

#ifndef HAR_CONNECTION_POPOVER_HPP
#define HAR_CONNECTION_POPOVER_HPP

#include <gtkmm/image.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <gtkmm/popover.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stack.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/viewport.h>

#include <har/full_cell.hpp>
#include <har/participant.hpp>

#include "types.hpp"

namespace har::gui_ {
    class scroll_list_box : public Gtk::ScrolledWindow {
    private:
        Gtk::Viewport _vprt;
        Gtk::ListBox _list;
        std::vector<Gtk::ListBoxRow *> _rows;

    public:
        scroll_list_box();

        void insert(Gtk::ListBoxRow & row, int_t position);

        Gtk::ListBoxRow * get_selected_row();

        void clear();

        const Gtk::ListBox & list_box() const;

        ~scroll_list_box() noexcept override;
    };

    class add_connection_row : public Gtk::ListBoxRow {
    private:
        const direction_t _use;
        const string_t _name;

    public:
        add_connection_row(direction_t use, const string_t & name);

        direction_t use() const;

        ~add_connection_row() noexcept override;
    };

    class connection_popover : public Gtk::Popover {
    private:
        Gtk::ToggleButton _btn_cl1;
        Gtk::Image _img_cl1;
        Gtk::ToggleButton _btn_cl2;
        Gtk::Image _img_cl2;
        Gtk::Stack _stack;
        scroll_list_box _list_from;
        scroll_list_box _list_to;

        conn_add_t _conn_add_fun;

        gcoords_t _from;
        gcoords_t _to;

        static uint_t populate_list(full_grid_cell & fgcl, scroll_list_box & list);

        void select_connection_use(direction_t use);

    public:
        connection_popover();

        void set_context(participant::context & ctx,
                      full_grid_cell & fgcl1, Glib::RefPtr<const Gdk::Pixbuf> && pixbuf1,
                      full_grid_cell & fgcl2, Glib::RefPtr<const Gdk::Pixbuf> && pixbuf2);

        conn_add_t & conn_add_fun();

        ~connection_popover() noexcept override;
    };

}

#endif //HAR_CONNECTION_POPOVER_HPP
