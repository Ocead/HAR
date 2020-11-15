//
// Created by Johannes on 08.07.2020.
//

#include <gtkmm/buttonbox.h>
#include <gtkmm/box.h>
#include <gtkmm/viewport.h>

#include <har/full_cell.hpp>

#include "types.hpp"
#include "connection_list.hpp"

using namespace har::gui_;

//region connection_row

connection_row::connection_row(direction_t dir,
                               const string_t & name,
                               gcoords_t to,
                               const conn_cb_t & callback) : Gtk::ListBoxRow(),
                                                             _use(make_ustring(name)), _cell(), _remove(),
                                                             _dir(dir),
                                                             _to(to) {
    _use.set_alignment(0., .5);

    _cell.set_width_chars(10);
    _cell.set_editable(false);

    auto & bbox = *Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    _remove.set_hexpand(false);
    Gtk::Image & imgptr = *Gtk::manage(new Gtk::Image());
    imgptr.set_from_icon_name("edit-clear-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _remove.set_image(imgptr);
    _remove.set_tooltip_text("Remove connection");
    _remove.signal_clicked().connect([=]() {
        callback(dir);
    });

    set_adjacent(_to);

    bbox.get_style_context()->add_class("linked");
    bbox.pack_start(_cell, Gtk::PACK_EXPAND_WIDGET);
    bbox.pack_end(_remove, Gtk::PACK_SHRINK);

    auto & box = *Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    box.set_spacing(5);
    box.set_homogeneous(true);
    box.set_size_request(0, 40);
    box.set_margin_right(5);
    box.set_margin_left(5);
    box.pack_start(_use, Gtk::PACK_EXPAND_WIDGET);
    box.pack_end(bbox, Gtk::PACK_EXPAND_WIDGET);

    Gtk::ListBoxRow::add(box);
}

void connection_row::set_adjacent(const gcoords_t & pos) {
    _to = pos;
    if (_to.cat != grid_t::INVALID_GRID) {
        stringstream ss;
        ss << _to;
        _cell.set_text(make_ustring(ss.str()));
        _remove.set_sensitive(true);
    } else {
        _cell.set_text("");
        _remove.set_sensitive(false);
    }
}

connection_row::~connection_row() noexcept = default;

//endregion

//region connection_list

connection_list::connection_list() : Gtk::ScrolledWindow(),
                                     _list(),
                                     _rows() {
    auto & vprt = *new Gtk::Viewport(Gtk::ScrolledWindow::get_hadjustment(),
                                     Gtk::ScrolledWindow::get_vadjustment());
    vprt.add(_list);
    Gtk::ScrolledWindow::set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    Gtk::ScrolledWindow::add(*Gtk::manage(&vprt));

    _list.set_selection_mode(Gtk::SELECTION_NONE);

    show_all_children();
}

void connection_list::set_cell(full_grid_cell & fgcl, const conn_cb_t & callback) {
    unset_cell();

    for (auto & use : fgcl.logic().connection_uses()) {
        auto[it, ok] = _rows.try_emplace(use.first, use.first, use.second, fgcl[use.first].position(), callback);
        _list.add(it->second);
    }
}

void connection_list::unset_cell() {
    for (auto &[e, w] : _rows) {
        _list.remove(w);
    }
    _rows.clear();
}

connection_row & connection_list::get_row(direction_t use) {
    return _rows.at(use);
}

connection_list::~connection_list() noexcept = default;

//endregion
