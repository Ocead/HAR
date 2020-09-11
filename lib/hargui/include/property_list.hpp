//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_PROPERTY_LIST_HPP
#define HAR_GUI_PROPERTY_LIST_HPP

#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <gtkmm/scrolledwindow.h>

#include <har/part.hpp>
#include <har/value.hpp>

namespace har::gui_ {

    using prop_cb_t = std::function<void(of, value &&)>;

    class property_row : public Gtk::ListBoxRow {
    private:
        Gtk::Label _label;
        Gtk::Widget * _child;
        const entry & _entry;

    public:
        property_row(const entry & ent, const prop_cb_t & callback, har::cell & cl);

        void set_value(const value & val);

        void set_value(value && val);

        void set_locked(bool_t lock);

        ~property_row() noexcept override;
    };

    class property_list : public Gtk::ScrolledWindow {
    private:
        Gtk::ListBox _list;

        std::function<void(of, value)> _callback;
        std::map<entry_h, property_row> _rows;

    public:
        explicit property_list(prop_cb_t callback);

        void set_cell(har::cell & cl, const prop_cb_t & callback, bool lock = false);

        void unset_cell();

        property_row & get_row(entry_h id);

        ~property_list() override;
    };
}

#endif //HAR_GUI_PROPERTY_LIST_HPP
