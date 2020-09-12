//
// Created by Johannes on 28.06.2020.
//

#include <utility>

#include <gtkmm.h>

#include <har/cell.hpp>
#include <har/grid_cell.hpp>

#include "types.hpp"
#include "property_list.hpp"

using namespace har::gui_;

//region property_row

property_row::property_row(const entry & ent,
                           const prop_cb_t & callback,
                           cell & cl) : Gtk::ListBoxRow(),
                                        _label(),
                                        _child(nullptr),
                                        _entry(ent) {
    auto prop = cl[ent.id];
    _label.set_alignment(0., .5);
    switch ((ent.id != of::TYPE) ? ent.access : ui_access::CHANGEABLE) {
        case ui_access::INVISIBLE:
            _label.set_markup(make_ustring(text("<span foreground=\"maroon\"><i>") + ent.friendly_name + text("</i></span>")));
            break;
        case ui_access::VISIBLE:
            _label.set_markup(make_ustring(text("<span foreground=\"maroon\">") + ent.friendly_name + text("</span>")));
            break;
        case ui_access::CHANGEABLE:
            _label.set_text(make_ustring(ent.friendly_name));
            break;
    }
    switch (ent.type_and_default.type()) {
        case datatype::VOID: {
            break;
        }
        case datatype::BOOLEAN: {
            auto & child = *new Gtk::CheckButton();
            child.set_active(bool_t(prop));
            child.signal_toggled().connect([id = ent.id, callback, &child]() {
                callback(id, value(bool_t(child.get_active())));
            });
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::INTEGER: {
            auto & child = *new Gtk::SpinButton();
            auto & spec = std::get<1>(ent.specifics);
            auto adj = Gtk::Adjustment::create(int_t(prop),
                                               std::get<0>(spec),
                                               std::get<1>(spec),
                                               std::get<2>(spec));
            child.set_adjustment(adj);
            child.signal_value_changed().connect([id = ent.id, callback, &child]() {
                callback(id, value(int_t(child.get_value_as_int())));
            });
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::UNSIGNED: {
            if (ent.specifics.index() == 2) {
                auto & child = *new Gtk::SpinButton();
                auto & spec = std::get<2>(ent.specifics);
                auto adj = Gtk::Adjustment::create(uint_t(prop),
                                                   std::get<0>(spec),
                                                   std::get<1>(spec),
                                                   std::get<2>(spec));
                child.set_adjustment(adj);
                child.signal_value_changed().connect([id = ent.id, callback, &child]() {
                    callback(id, value(uint_t(child.get_value_as_int())));
                });
                _child = Gtk::manage(&child);
            } else if (ent.specifics.index() == 4) {
                auto & child = *new Gtk::ComboBoxText();
                auto & spec = std::get<4>(ent.specifics);
                for (auto & e : *spec) {
                    child.insert(int_t(std::get<0>(e)), std::to_string(std::get<0>(e)), make_ustring(std::get<1>(e)));
                }
                child.set_active_id(std::to_string(uint_t(prop)));
                child.signal_changed().connect([id = ent.id, callback, &child]() {
                    callback(id, value(uint_t(std::stoul(child.get_active_id()))));
                });
                _child = Gtk::manage(&child);
            }
            break;
        }
        case datatype::DOUBLE: {
            auto & child = *new Gtk::SpinButton();
            auto & spec = std::get<3>(ent.specifics);
            int prec = std::ceil(std::log10(1. / std::fmod(std::get<2>(spec), 1.)));
            auto val = double_t(prop);
            if (!std::isnan(val)) {
                child.set_adjustment(Gtk::Adjustment::create(val,
                                                             std::get<0>(spec),
                                                             std::get<1>(spec),
                                                             std::get<2>(spec)));
            } else {
                child.set_adjustment(Gtk::Adjustment::create(0,
                                                             std::get<0>(spec),
                                                             std::get<1>(spec),
                                                             std::get<2>(spec)));
                child.set_text("NaN");
            }

            child.set_digits(prec);
            child.signal_value_changed().connect([id = ent.id, callback, &child]() {
                callback(id, value(double_t(child.get_value())));
            });
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::STRING: {
            auto & child = *new Gtk::Entry();
            child.set_text(make_ustring(prop.as<string_t>()));
            child.signal_activate().connect([id = ent.id, callback, &child]() {
                auto ustr = child.get_text();
                string_t str{ ustr.begin(), ustr.end() };
                callback(id, value(str));
            });
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::C_COORDINATES: {
            auto & child = *new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);
            auto & childx = *new Gtk::Entry();
            auto & childy = *new Gtk::Entry();
            auto & cc = prop.as<ccoords_t>();
            childx.set_text(std::to_string(cc.x));
            childy.set_text(std::to_string(cc.y));
            childx.signal_activate().connect([id = ent.id, callback, &childx, &childy]() {
                callback(id, value(ccoords_t(std::stod(childx.get_text()), std::stod(childy.get_text()))));
            });
            childy.signal_activate().connect([id = ent.id, callback, &childx, &childy]() {
                callback(id, value(ccoords_t(std::stod(childx.get_text()), std::stod(childy.get_text()))));
            });
            child.pack_start(*Gtk::manage(&childx), Gtk::PACK_EXPAND_WIDGET);
            child.pack_end(*Gtk::manage(&childy), Gtk::PACK_EXPAND_WIDGET);
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::D_COORDINATES: {
            auto & child = *new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);
            auto & childx = *new Gtk::Entry();
            auto & childy = *new Gtk::Entry();
            auto & dc = prop.as<dcoords_t>();
            childx.set_text(std::to_string(dc.x));
            childy.set_text(std::to_string(dc.y));
            childx.signal_activate().connect([id = ent.id, callback, &childx, &childy]() {
                callback(id, value(dcoords_t(std::stol(childx.get_text()), std::stol(childy.get_text()))));
            });
            childy.signal_activate().connect([id = ent.id, callback, &childx, &childy]() {
                callback(id, value(dcoords_t(std::stol(childx.get_text()), std::stol(childy.get_text()))));
            });
            child.pack_start(*Gtk::manage(&childx), Gtk::PACK_EXPAND_WIDGET);
            child.pack_end(*Gtk::manage(&childy), Gtk::PACK_EXPAND_WIDGET);
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::DIRECTION: {
            auto & child = *new Gtk::ComboBoxText();
            auto & spec = std::get<5>(ent.specifics);
            if (spec == dir_cat::ALL_DIRECTIONS) {
                child.append(std::to_string(int_t(direction::NONE)), "None");
            }
            if (spec == dir_cat::ALL_DIRECTIONS || spec == dir_cat::CARDINAL_DIRECTIONS) {
                child.append(std::to_string(int_t(direction::UP)), "Up");
                child.append(std::to_string(int_t(direction::DOWN)), "Down");
                child.append(std::to_string(int_t(direction::RIGHT)), "Right");
                child.append(std::to_string(int_t(direction::LEFT)), "Left");
            }
            if (spec == dir_cat::ALL_DIRECTIONS || spec == dir_cat::PIN_DIRECTIONS) {
                auto * ptr = dynamic_cast<grid_cell *>(&cl);
                if (ptr) {
                    auto & pt = cl.logic();
                    for (auto & u : pt.connection_uses()) {
                        child.append(std::to_string(int_t(u.first)), make_ustring(u.second));
                    }
                }
            }
            if (spec == dir_cat::CONNECTED_ONLY) {
                auto * ptr = dynamic_cast<grid_cell *>(&cl);
                if (ptr) {
                    auto & pt = cl.logic();
                    auto & uses = pt.connection_uses();
                    for (auto &[use, cell] : ptr->connected()) {
                        auto it = uses.find(use);
                        if (it != uses.end()) {
                            child.append(make_ustring(it->second), std::to_string(int_t(use)));
                        } else {
                            child.append(std::to_string(int_t(direction::LEFT)),
                                         std::string("Pin ") + std::to_string(int_t(direction::LEFT)));
                        }
                    }
                }
            }

            child.set_active_id(std::to_string(int_t(direction_t(prop))));
            child.signal_changed().connect([id = ent.id, callback, &child]() {
                callback(id, value(direction_t(std::stoi(child.get_active_id()))));
            });
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::COLOR: {
            auto & child = *new Gtk::ColorButton();
            auto & color = prop.as<color_t>();
            Gdk::RGBA rgba;
            rgba.set_rgba_u(color.r * 256u,
                            color.g * 256u,
                            color.b * 256u,
                            color.a * 256u);
            child.set_rgba(rgba);
            child.signal_color_set().connect([id = ent.id, callback, &child]() {
                Gdk::RGBA rgba = child.get_rgba();
                color_t color(rgba.get_red_u() / 256u,
                              rgba.get_green_u() / 256u,
                              rgba.get_blue_u() / 256u,
                              rgba.get_alpha_u() / 256u);
                callback(id, value(color));
            });
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::SPECIAL: {
            auto & child = *new Gtk::Entry();
            child.set_text(prop.any().type().name());
            child.set_editable(false);
            _child = Gtk::manage(&child);
        }
        case datatype::CALLBACK: {
            auto & child = *new Gtk::Button("Call");
            child.signal_clicked().connect([id = ent.id, callback, val = prop.val()]() {
                callback(id, value(val));
            });
            if (!callback_t(prop)) {
                child.set_sensitive(false);
            }
            _child = Gtk::manage(&child);
            break;
        }
        case datatype::HASH: {
            auto & child = *new Gtk::Label();
            if (ent.id == of::TYPE) {
                child.set_text(make_ustring(cl.logic().friendly_name()));
                child.set_alignment(0., .5);
            } else {
                std::stringstream ss;
                ss << prop.as<part_h>();
                child.set_text(ss.str());
            }

            _child = Gtk::manage(&child);
            break;
        }
    }

    auto * box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    box->set_spacing(5);
    box->set_homogeneous(true);
    box->set_size_request(0, 40);
    box->set_margin_right(5);
    box->set_margin_left(5);
    box->pack_start(_label, Gtk::PACK_EXPAND_WIDGET);
    box->pack_end(*_child, Gtk::PACK_EXPAND_WIDGET);

    Gtk::ListBoxRow::add(*box);
}

void property_row::set_value(const value & val) {
    return set_value(std::forward<value>(value(val)));
}

void property_row::set_value(value && val) {
    switch (val.type()) {
        case datatype::VOID: {
            break;
        }
        case datatype::BOOLEAN: {
            dynamic_cast<Gtk::CheckButton &>(*_child).set_active(get<bool_t>(val));
            break;
        }
        case datatype::INTEGER: {
            dynamic_cast<Gtk::SpinButton &>(*_child).set_value(double(get<int_t>(val)));
            break;
        }
        case datatype::UNSIGNED: {
            try {
                dynamic_cast<Gtk::SpinButton &>(*_child).set_value(double(get<uint_t>(val)));
            } catch (std::bad_cast &) {
                dynamic_cast<Gtk::ComboBoxText &>(*_child).set_active_id(std::to_string(get<uint_t>(val)));
            }
            break;
        }
        case datatype::DOUBLE: {
            dynamic_cast<Gtk::SpinButton &>(*_child).set_value(get<double_t>(val));
            break;
        }
        case datatype::STRING: {
            dynamic_cast<Gtk::Entry &>(*_child).set_text(make_ustring(get<string_t>(val)));
            break;
        }
        case datatype::C_COORDINATES: {
            auto xy = dynamic_cast<Gtk::Box &>(*_child).get_children();
            auto cc = get<ccoords_t>(val);
            dynamic_cast<Gtk::Entry &>(*xy.at(0)).set_text(std::to_string(cc.x));
            dynamic_cast<Gtk::Entry &>(*xy.at(1)).set_text(std::to_string(cc.y));
            break;
        }
        case datatype::D_COORDINATES: {
            auto xy = dynamic_cast<Gtk::Box &>(*_child).get_children();
            auto dc = get<dcoords_t>(val);
            dynamic_cast<Gtk::Entry &>(*xy.at(0)).set_text(std::to_string(dc.x));
            dynamic_cast<Gtk::Entry &>(*xy.at(1)).set_text(std::to_string(dc.y));
            break;
        }
        case datatype::DIRECTION: {
            auto dir = get<direction_t>(val);
            dynamic_cast<Gtk::ComboBoxText &>(*_child).set_active_id(std::to_string(int_t(dir)));
            break;
        }
        case datatype::COLOR: {
            auto color = get<color_t>(val);
            Gdk::RGBA rgba;
            rgba.set_rgba_u(color.r * 256u,
                            color.g * 256u,
                            color.b * 256u,
                            color.a * 256u);
            dynamic_cast<Gtk::ColorButton &>(*_child).set_rgba(rgba);
            break;
        }
        case datatype::SPECIAL: {
            break;
        }
        case datatype::CALLBACK: {
            break;
        }
        case datatype::HASH: {
            break;
        }
    }
}

void property_row::set_locked(bool_t lock) {
    if (auto * eptr = dynamic_cast<Gtk::Editable *>(_child)) {
        eptr->set_editable(!lock);
    } else if (auto * bptr = dynamic_cast<Gtk::Box *>(_child)) {
        for (auto boxc : bptr->get_children()) {
            dynamic_cast<Gtk::Editable *>(boxc)->set_editable(!lock);
        }
    } else {
        _child->set_sensitive(!lock);
    }
}

property_row::~property_row() noexcept = default;

//endregion

//region property_list

property_list::property_list(prop_cb_t callback) : Gtk::ScrolledWindow(),
                                                   _list(),
                                                   _callback(std::move(callback)),
                                                   _rows() {
    auto & vprt = *new Gtk::Viewport(Gtk::ScrolledWindow::get_hadjustment(),
                                     Gtk::ScrolledWindow::get_vadjustment());

    _list.set_selection_mode(Gtk::SELECTION_NONE);

    vprt.add(_list);
    Gtk::ScrolledWindow::set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    Gtk::ScrolledWindow::add(*Gtk::manage(&vprt));

    Gtk::ScrolledWindow::show_all_children();
}

void property_list::set_cell(cell & cl, const prop_cb_t & callback, bool lock) {
    unset_cell();

    for (auto & e : cl.logic().model()) {
        auto[it, ok] = _rows.try_emplace(e.first, e.second, callback, cl);
        if (lock) {
            it->second.set_locked(lock);
        }
        _list.add(it->second);
    }
}

void property_list::unset_cell() {
    for (auto &[e, w] : _rows) {
        _list.remove(w);
    }
    _rows.clear();
}

property_row & property_list::get_row(entry_h id) {
    return _rows.at(id);
}

property_list::~property_list() = default;

//endregion
