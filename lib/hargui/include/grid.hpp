//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_GRID_HPP
#define HAR_GUI_GRID_HPP

#include <atomic>

#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/fixed.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/overlay.h>
#include <gtkmm/popover.h>
#include <gtkmm/spinbutton.h>

#include <har/coords.hpp>
#include <har/part.hpp>
#include <har/types.hpp>

#include "cell.hpp"
#include "part.hpp"
#include "types.hpp"

namespace har::gui_ {
    class inventory : public Gtk::Popover {
    private:
        std::vector<GtkTargetEntry> _tentries;
        har::map<part_h, part> _part_map;
        std::function<void(const Glib::RefPtr<Gdk::DragContext> &)> _drag_cb;

        Gtk::FlowBox _flowbox;

        std::function<void(const Glib::RefPtr<Gdk::DragContext> & context)> _drag_begin_fun;
        std::function<void(const Glib::RefPtr<Gdk::DragContext> & context,
                           Gtk::SelectionData & sel, guint info, guint time)> _drag_data_get_fun;

        static image_out_t get_image_base();

    public:
        explicit inventory();

        std::vector<GtkTargetEntry> & target_entries();

        void add_part(const har::part & pt);

        void remove_part(part_h id);

        decltype(_drag_begin_fun) & drag_begin_fun();

        decltype(_drag_data_get_fun) & drag_data_get_fun();

        ~inventory() noexcept override;
    };

    class grid_properties : public Gtk::Popover {
    private:
        Gtk::SpinButton _sizex;
        Gtk::SpinButton _sizey;
        Gtk::SpinButton _imgres;
        grid_t _cat;

        std::function<void(const gcoords_t &)> _resize_fun;
        std::function<void(uint_t)> _resolution_fun;
        bool_t _updating;

    public:
        grid_properties(uint_t res, grid_t cat);

        [[nodiscard]]
        dcoords_t size() const;

        [[nodiscard]]
        uint_t resolution() const;

        void set_size(const dcoords_t & to);

        std::function<void(const gcoords_t &)> & resize_fun();

        std::function<void(uint_t)> & resolution_fun();

        ~grid_properties() noexcept override;
    };

    class grid : public Gtk::Box {
    private:
        Gtk::Box _header;
        Gtk::Entry _name;
        Gtk::MenuButton _inv_btn;

        Gtk::Grid _grid;
        Gtk::Overlay _over;
        Gtk::Fixed _fixed;

        Gtk::Image _img_ref;
        Glib::RefPtr<Gdk::Pixbuf> _ref_pixbuf;

        std::atomic<bool_t> _updating;

        std::vector<GtkTargetEntry> _target_entries;
        drag_data_received_t _ddrf;

        inventory _inventory;
        grid_properties _properties;

        dcoords_t _size;
        uint_t _res;
        har::map<cargo_h, std::reference_wrapper<cell>> _cargo_map;

        grid_t _grid_hnd;
        button_press_t _button_press_fun;
        button_release_t _button_release_fun;
        drag_begin_t _drag_begin_fun;
        drag_data_get_t _drag_data_get_fun;
        std::function<void()> _drag_failed_fun;
        drag_data_received_t _drag_data_received_fun;

        cell & create_cell(const gcoords_t& gc);

        void insert_column();

        void insert_row();

        void remove_column();

        void remove_row();

    public:
        explicit grid(grid_t grid, uint_t res = 64u);

        dcoords_t size() const;

        void set_size(const dcoords_t & size);

        uint_t get_resolution() const;

        void set_resolution(uint_t res);

        cell & at(const dcoords_t & pos);

        cell & at(cargo_h num);

        void include_part(const har::part & pt);

        void remove_part(part_h id);

        void set_name(const string_t & name);

        Glib::SignalProxy<void> signal_name_set();

        std::function<void(const gcoords_t &)> & resize_fun();

        std::function<void(uint_t)> & resolution_fun();

        std::vector<GtkTargetEntry> & inventory_target_entries();

        std::vector<GtkTargetEntry> & cell_target_entries();

        decltype(_button_press_fun) & button_press_fun();

        decltype(_button_release_fun) & button_release_fun();

        decltype(_drag_begin_fun) & drag_begin_fun();

        decltype(_drag_data_get_fun) & drag_data_get_fun();

        decltype(_drag_failed_fun) & drag_failed_fun();

        decltype(_drag_data_received_fun) & drag_data_received_fun();

        void hide_overlay();

        void show_overlay();

        ~grid() noexcept override;
    };
}

#endif //HAR_GUI_GRID_HPP
