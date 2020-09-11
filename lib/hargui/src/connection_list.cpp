//
// Created by Johannes on 08.07.2020.
//

#include <gtkmm/box.h>
#include <gtkmm/viewport.h>

#include <har/full_cell.hpp>

#include "types.hpp"
#include "connection_list.hpp"

using namespace har::gui_;

//region connection_row

connection_row::connection_row(direction_t dir,
                               const string_t & name,
                               gcoords_t to) : Gtk::ListBoxRow(),
                                               _use(make_ustring(name)), _cell(),
                                               _dir(dir),
                                               _to(to) {
    _use.set_alignment(0., .5);

    if (to.cat != grid_t::INVALID_GRID) {
        stringstream ss;
        ss << _to;
        _cell.set_text(make_ustring(ss.str()));
    }

    auto * box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    box->set_spacing(5);
    box->set_homogeneous(true);
    box->set_size_request(0, 40);
    box->set_margin_right(5);
    box->set_margin_left(5);
    box->pack_start(_use, Gtk::PACK_EXPAND_WIDGET);
    box->pack_end(_cell, Gtk::PACK_EXPAND_WIDGET);

    Gtk::ListBoxRow::add(*box);
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

    show_all_children();
}

void connection_list::set_cell(full_grid_cell & fgcl) {
    unset_cell();

    for (auto & use : fgcl.logic().connection_uses()) {
        auto[it, ok] = _rows.try_emplace(use.first, use.first, use.second, fgcl[use.first].position());
        _list.add(it->second);
    }
}

void connection_list::unset_cell() {
    for (auto &[e, w] : _rows) {
        _list.remove(w);
    }
    _rows.clear();
}

connection_list::~connection_list() noexcept = default;

//endregion