//
// Created by Johannes on 26.07.2020.
//

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/viewport.h>

#include "types.hpp"
#include "connection_popover.hpp"

using namespace har::gui_;

//region scroll_list_box

scroll_list_box::scroll_list_box() :
        Gtk::ScrolledWindow(),
        _vprt(get_hadjustment(), get_vadjustment()),
        _list() {
    Gtk::ScrolledWindow::set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    Gtk::ScrolledWindow::set_max_content_height(300);
    Gtk::ScrolledWindow::set_propagate_natural_height(true);
    Gtk::ScrolledWindow::set_shadow_type(Gtk::SHADOW_IN);

    _list.set_selection_mode(Gtk::SELECTION_BROWSE);
    _list.set_placeholder(*Gtk::manage(new Gtk::Label("(None)")));

    _vprt.add(_list);
    Gtk::ScrolledWindow::add(_vprt);
}

void scroll_list_box::insert(Gtk::ListBoxRow & row, int_t position) {
    _rows.emplace_back(&row);
    _list.insert(row, position);
}

Gtk::ListBoxRow * scroll_list_box::get_selected_row() {
    return _list.get_selected_row();
}

void scroll_list_box::clear() {
    for (auto r : _rows) {
        _list.remove(*r);
    }
    _rows.clear();
}

const Gtk::ListBox & scroll_list_box::list_box() const {
    return _list;
}

scroll_list_box::~scroll_list_box() noexcept = default;

//endregion

//region connection_row

add_connection_row::add_connection_row(direction_t use, const string_t & name) : Gtk::ListBoxRow(),
                                                                                 _use(use),
                                                                                 _name(name) {
    auto & box = *Gtk::manage(new Gtk::HBox());
    auto & lbl = *Gtk::manage(new Gtk::Label(name));
    auto & idx = *Gtk::manage(new Gtk::Label(std::to_string(int_t(use))));

    lbl.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
    idx.set_alignment(Gtk::ALIGN_END, Gtk::ALIGN_CENTER);

    box.set_spacing(10);
    box.pack_start(lbl, Gtk::PACK_EXPAND_WIDGET);
    box.pack_end(idx, Gtk::PACK_EXPAND_WIDGET);

    Gtk::ListBoxRow::add(box);
}

har::direction_t add_connection_row::use() const {
    return _use;
}

add_connection_row::~add_connection_row() noexcept = default;

//endregion

//region connection_popover

connection_popover::connection_popover() : Gtk::Popover(),
                                           _btn_cl1(),
                                           _img_cl1(),
                                           _btn_cl2(),
                                           _img_cl2(),
                                           _stack(),
                                           _list_from(),
                                           _list_to() {
    auto & box = *Gtk::manage(new Gtk::VBox());

    _btn_cl1.set_image(_img_cl1);
    _btn_cl1.signal_toggled().connect_notify([&]() {
        if (_btn_cl1.get_active()) {
            if (_btn_cl2.get_active()) {
                _stack.set_visible_child(_list_from);
            }
            _btn_cl2.set_active(false);
        }
    });
    _btn_cl2.set_image(_img_cl2);
    _btn_cl2.signal_toggled().connect_notify([&]() {
        if (_btn_cl2.get_active()) {
            if (_btn_cl1.get_active()) {
                _stack.set_visible_child(_list_to);
            }
            _btn_cl1.set_active(false);
        }
    });

    auto & btnbox = *Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_HORIZONTAL));
    btnbox.pack_start(_btn_cl1, Gtk::PACK_EXPAND_WIDGET);
    btnbox.pack_end(_btn_cl2, Gtk::PACK_EXPAND_WIDGET);
    btnbox.set_layout(Gtk::BUTTONBOX_EXPAND);

    box.pack_start(btnbox, Gtk::PACK_EXPAND_PADDING);

    _list_from.signal_button_press_event().connect([&](const GdkEventButton * evb) -> bool {
        if (evb->button == 1 && evb->type == GDK_2BUTTON_PRESS) {
            auto use = dynamic_cast<add_connection_row *>(_list_from.get_selected_row())->use();
            select_connection_use(use);
        }
        return true;
    });

    _list_to.signal_button_press_event().connect([&](const GdkEventButton * evb) -> bool {
        if (evb->button == 1 && evb->type == GDK_2BUTTON_PRESS) {
            auto use = dynamic_cast<add_connection_row *>(_list_to.get_selected_row())->use();
            select_connection_use(use);
        }
        return true;
    });

    _stack.add(_list_from);
    _stack.add(_list_to);

    box.pack_end(_stack, Gtk::PACK_EXPAND_WIDGET);

    box.set_spacing(5);
    box.set_margin_start(5);
    box.set_margin_top(5);
    box.set_margin_end(5);
    box.set_margin_bottom(5);

    Gtk::Popover::add(box);
    Gtk::Popover::set_modal(true);
    Gtk::Popover::set_position(Gtk::POS_BOTTOM);
    Gtk::Popover::show_all_children();
}

har::uint_t connection_popover::populate_list(full_grid_cell & fgcl, scroll_list_box & list) {
    list.clear();

    uint_t count{ 0u };
    for (auto &[use, name] : fgcl.logic().connection_uses()) {
        if (!fgcl.has_connection(use)) {
            add_connection_row & row = *Gtk::manage(new add_connection_row(use, name));
            list.insert(row, int_t(use));
            count++;
        }
    }

    list.show_all_children(true);
    return count;
}

void connection_popover::select_connection_use(direction_t use) {
    auto & from = _btn_cl2.get_active() ? _to : _from;
    auto & to = _btn_cl2.get_active() ? _from : _to;

    _conn_add_fun(from, to, use);
    Gtk::Popover::popdown();
}

void connection_popover::set_context(participant::context & ctx,
                                     full_grid_cell & from, Glib::RefPtr<const Gdk::Pixbuf> && pixbuf1,
                                     full_grid_cell & to, Glib::RefPtr<const Gdk::Pixbuf> && pixbuf2) {
    _from = from.position();
    _to = to.position();

    _img_cl1.set(pixbuf1->scale_simple(16, 16, Gdk::INTERP_BILINEAR));
    _img_cl2.set(pixbuf2->scale_simple(16, 16, Gdk::INTERP_BILINEAR));

    populate_list(from, _list_from);
    populate_list(to, _list_to);

    _btn_cl1.set_active(true);
}

conn_add_t & connection_popover::conn_add_fun() {
    return _conn_add_fun;
}

connection_popover::~connection_popover() noexcept = default;

//endregion
