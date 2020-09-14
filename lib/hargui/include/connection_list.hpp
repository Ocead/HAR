//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_CONNECTION_LIST_HPP
#define HAR_GUI_CONNECTION_LIST_HPP

#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>
#include <gtkmm/listbox.h>
#include <gtkmm/scrolledwindow.h>

#include <har/grid_cell.hpp>

namespace har::gui_ {

    using conn_cb_t = std::function<void(direction_t)>;

    class connection_row : public Gtk::ListBoxRow {
    private:
        Gtk::Label _use;
        Gtk::Entry _cell;
        Gtk::Button _remove;

        direction_t _dir;
        gcoords_t _to;

    public:
        connection_row(direction_t dir, const string_t & name, gcoords_t to, const conn_cb_t & callback);

        void set_adjacent(const gcoords_t & pos);

        ~connection_row() noexcept override;
    };

    class connection_list : public Gtk::ScrolledWindow {
    private:
        Gtk::ListBox _list;

        std::map<direction_t, connection_row> _rows;

    public:
        connection_list();

        void set_cell(full_grid_cell & fgcl, const conn_cb_t & callback);

        void unset_cell();

        connection_row & get_row(direction_t use);

        ~connection_list() noexcept override;
    };
}

#endif //HAR_GUI_CONNECTION_LIST_HPP
