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

//region connection_row

add_connection_row::add_connection_row(direction_t dir) : Gtk::ListBoxRow(), _dir(dir) {

}

har::direction_t add_connection_row::dir() const {
    return _dir;
}

add_connection_row::~add_connection_row() noexcept = default;

//endregion

//region connection_popover

connection_popover::connection_popover() : Gtk::Popover(),
                                           _btn_cl1(),
                                           _img_cl1(),
                                           _btn_cl2(),
                                           _img_cl2(),
                                           _list() {
    auto & box = *Gtk::manage(new Gtk::VBox());

    _btn_cl1.set_image(_img_cl1);
    _btn_cl1.signal_toggled().connect_notify([&]() {
        if (_btn_cl1.get_active()) {
            if (_btn_cl2.get_active()) {
                select_target(*_fgcl1);
            }
            _btn_cl2.set_active(false);
        }
    });
    _btn_cl2.set_image(_img_cl2);
    _btn_cl2.signal_toggled().connect_notify([&]() {
        if (_btn_cl2.get_active()) {
            if (_btn_cl1.get_active()) {
                select_target(*_fgcl2);
            }
            _btn_cl1.set_active(false);
        }
    });

    auto & btnbox = *Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_HORIZONTAL));
    btnbox.pack_start(_btn_cl1, Gtk::PACK_EXPAND_WIDGET);
    btnbox.pack_end(_btn_cl2, Gtk::PACK_EXPAND_WIDGET);
    btnbox.set_layout(Gtk::BUTTONBOX_EXPAND);

    box.pack_start(btnbox, Gtk::PACK_EXPAND_PADDING);

    auto & scrl = *Gtk::manage(new Gtk::ScrolledWindow());
    auto & vprt = *Gtk::manage(new Gtk::Viewport(scrl.get_hadjustment(), scrl.get_vadjustment()));

    _list.signal_button_press_event().connect([&](const GdkEventButton * evb) -> bool {
        if (evb->button == 1 && evb->type == GDK_2BUTTON_PRESS) {
            auto use = dynamic_cast<add_connection_row *>(_list.get_selected_row())->dir();
            select_connection_use(use);
        }
        return true;
    });
    _list.set_selection_mode(Gtk::SELECTION_BROWSE);
    _list.set_placeholder(*Gtk::manage(new Gtk::Label("None")));

    vprt.add(_list);

    scrl.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    scrl.set_max_content_height(300);
    scrl.set_propagate_natural_height(true);
    scrl.set_shadow_type(Gtk::SHADOW_IN);
    scrl.add(vprt);

    box.pack_end(scrl, Gtk::PACK_EXPAND_WIDGET);

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

void connection_popover::select_target(full_grid_cell & fgcl) {
    unselect_target();

    for (auto &[use, name] : fgcl.logic().connection_uses()) {
        if (!fgcl.has_connection(use)) {
            auto it = std::get<0>(_rows.try_emplace(use, use));
            auto & row = it->second;
            row.add_label(make_ustring(name) + " (" + std::to_string(int_t(use)) + ")");
            _list.append(row);
        }
    }

    _list.show_all_children(true);
}

void connection_popover::select_connection_use(direction_t dir) {
    auto & from = _btn_cl2.get_active() ? *_fgcl2 : *_fgcl1;
    auto & to = _btn_cl2.get_active() ? *_fgcl1 : *_fgcl2;

    from.add_connection(dir, to);
    Gtk::Popover::popdown();
}

void connection_popover::on_hide() {
    _fgcl2 = nullptr;
    _fgcl1 = nullptr;
    _ctx = nullptr;
    Gtk::Popover::on_hide();
}

void connection_popover::set_cell(std::unique_ptr<participant::context> && ctx,
                                  std::unique_ptr<full_grid_cell> && fgcl1, Glib::RefPtr<const Gdk::Pixbuf> pixbuf1,
                                  std::unique_ptr<full_grid_cell> && fgcl2, Glib::RefPtr<const Gdk::Pixbuf> pixbuf2) {
    _ctx = std::move(ctx);
    _fgcl1 = std::move(fgcl1);
    _fgcl2 = std::move(fgcl2);

    _img_cl1.set(pixbuf1->scale_simple(16, 16, Gdk::INTERP_BILINEAR));
    _img_cl2.set(pixbuf2->scale_simple(16, 16, Gdk::INTERP_BILINEAR));

    select_target(*_fgcl1);

    _btn_cl1.set_active(true);
}

void connection_popover::unselect_target() {
    for (auto &[dir, r] : _rows) {
        _list.remove(r);
    }

    _rows.clear();
}

connection_popover::~connection_popover() noexcept = default;

//endregion
