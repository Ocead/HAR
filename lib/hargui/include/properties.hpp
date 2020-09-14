//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_PROPERTIES_HPP
#define HAR_GUI_PROPERTIES_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/notebook.h>

#include <har/full_cell.hpp>

#include "property_list.hpp"
#include "connection_list.hpp"
#include "cargo_list.hpp"

namespace har::gui_ {
    class properties : public Gtk::Box {
    private:
        Gtk::Label _name;
        Gtk::Button _export;
        Gtk::Notebook _notebook;

        property_list _proplist;
        connection_list _connlist;
        cargo_list _cargolist;

    public:
        explicit properties(std::function<void(of, value &&)> propcb);

        void set_cell(full_grid_cell & gcl, prop_cb_t && prop_cb, const conn_cb_t && conn_cb, bool lock = false);

        void set_cell(full_cargo_cell & ccl, prop_cb_t && callback, bool lock = false);

        void unset_cell();

        property_list & get_proplist();

        connection_list & get_connlist();

        cargo_list & get_cargolist();

        ~properties() override;
    };
}

#endif //HAR_GUI_PROPERTIES_HPP
