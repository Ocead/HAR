//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_CARGO_LIST_HPP
#define HAR_GUI_CARGO_LIST_HPP

#include <gtkmm/listbox.h>

#include <har/grid_cell.hpp>

namespace har::gui_ {
    class cargo_list : public Gtk::ListBox {
    public:
        cargo_list();

        void set_cell(const grid_cell & ctx);

        void unset_cell();

        ~cargo_list() override;
    };
}

#endif //HAR_GUI_CARGO_LIST_HPP
