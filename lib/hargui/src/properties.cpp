//
// Created by Johannes on 08.07.2020.
//

#include <gtkmm/box.h>
#include <gtkmm/separator.h>

#include "types.hpp"
#include "properties.hpp"

using namespace har::gui_;

properties::properties(std::function<void(of, value && )> propcb) : Gtk::Box(Gtk::ORIENTATION_VERTICAL),
                                                                    _name(), _export(), _notebook(),
                                                                    _proplist(std::move(propcb)),
                                                                    _connlist(),
                                                                    _cargolist() {
    auto & btn_export = *new Gtk::Button();
    btn_export.set_image_from_icon_name("document-send-symbolic", Gtk::ICON_SIZE_BUTTON);
    btn_export.set_relief(Gtk::RELIEF_NONE);

    _name.set_alignment(0.5f);

    auto & header = *new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);
    header.set_margin_start(2);
    header.set_margin_end(2);
    header.set_margin_top(5);
    header.set_margin_bottom(5);
    header.set_center_widget(_name);
    header.pack_end(*Gtk::manage(&btn_export), Gtk::PACK_SHRINK);

    _notebook.set_show_border(false);
    _notebook.set_tab_pos(Gtk::POS_BOTTOM);
    _notebook.insert_page(_proplist, "Properties", 1);

    Gtk::Box::pack_start(header, Gtk::PACK_SHRINK);
    Gtk::Box::pack_start(*Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL)), Gtk::PACK_SHRINK);
    Gtk::Box::pack_end(_notebook, Gtk::PACK_EXPAND_WIDGET);
    show_all_children();
}

void properties::set_cell(full_grid_cell & fgcl, prop_cb_t && callback, bool lock) {
    gcoords_t gc = fgcl.position();
    gc.pos += 1;
    stringstream ss{ };
    ss << gc << " " << string_t(fgcl[of::NAME]);
    _name.set_text(make_ustring(ss.str()));

    _proplist.set_cell(fgcl, callback, lock);
    _connlist.set_cell(fgcl);
    _cargolist.set_cell(fgcl);

    if (_notebook.get_n_pages() == 1) {
        _notebook.insert_page(_connlist, "Connections", 1);
    }
    if (fgcl.position().cat == grid_t::MODEL_GRID) {
        if (_notebook.get_n_pages() == 2) {
            _notebook.insert_page(_cargolist, "Cargo", 2);
        }
    } else if (_notebook.get_n_pages() == 3) {
        _notebook.remove_page(_cargolist);
    }

    show_all_children();
}

void properties::set_cell(full_cargo_cell & fccl, prop_cb_t && callback, bool lock) {
    stringstream ss{ };
    ss << fccl.position() << text(" ") << string_t(fccl[of::NAME]);
    _name.set_text(make_ustring(ss.str()));

    _proplist.set_cell(fccl, callback, lock);

    if (_notebook.get_n_pages() == 3) {
        _notebook.remove_page(_connlist);
    }
    if (_notebook.get_n_pages() == 2) {
        _notebook.remove_page(_cargolist);
    }

    show_all_children();
}

void properties::unset_cell() {
    _name.set_text("");
    _proplist.unset_cell();
    _connlist.unset_cell();
    _cargolist.unset_cell();

    if (_notebook.get_n_pages() == 3) {
        _notebook.remove_page(_connlist);
    }
    if (_notebook.get_n_pages() == 2) {
        _notebook.remove_page(_cargolist);
    }
}

property_list & properties::get_proplist() {
    return _proplist;
}

connection_list & properties::get_connlist() {
    return _connlist;
}

cargo_list & properties::get_cargolist() {
    return _cargolist;
}

properties::~properties() = default;
