//
// Created by Johannes on 28.06.2020.
//

#include <gtkmm/aspectframe.h>
#include <gtkmm/overlay.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>
#include <gtkmm/viewport.h>

#include <har/sketch_cell.hpp>

#include "grid.hpp"

using namespace har::gui_;

//region inventory

inventory::inventory() : Gtk::Popover(),
                         _tentries(),
                         _part_map(),
                         _drag_cb(),
                         _flowbox() {
    _flowbox.set_selection_mode(Gtk::SELECTION_NONE);
    _flowbox.set_homogeneous(true);
    _flowbox.set_min_children_per_line(3);
    _flowbox.set_max_children_per_line(3);

    auto & scrl = *Gtk::manage(new Gtk::ScrolledWindow());
    scrl.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
    scrl.set_hexpand(true);

    auto & vprt = *Gtk::manage(new Gtk::Viewport(scrl.get_hadjustment(), scrl.get_vadjustment()));
    vprt.set_hexpand(true);
    vprt.add(_flowbox);

    scrl.add(vprt);
    scrl.set_propagate_natural_width();
    Gtk::Popover::add(scrl);

    show_all_children();
}

std::vector<GtkTargetEntry> & inventory::target_entries() {
    return _tentries;
}

image_out_t inventory::get_image_base() {
    return Glib::RefPtr<Gdk::Pixbuf>(nullptr);
}

void inventory::add_part(const har::part & pt) {
    using ImageType = std::tuple<Cairo::RefPtr<Cairo::Surface>, uint_t>;
    auto it = std::get<0>(_part_map.try_emplace(pt.id()));

    har::sketch_grid_cell sgcl{ pt };
    std::any img = get_image_base();
    pt.draw(sgcl, img);
    auto & wgt = it->second;
    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    if (img.type() == typeid(ImageType)) {
        auto[sf, res] = std::any_cast<ImageType>(img);
        pixbuf = Gdk::Pixbuf::create(sf, 0, 0, res, res)
                ->scale_simple(32, 32, Gdk::INTERP_BILINEAR);
    } else {
        pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, 32, 32);
        pixbuf->fill(0x00000000);

    }
    wgt.set_image(pixbuf);
    wgt.set_label(make_ustring(pt.friendly_name()));
    wgt.set_tooltip_text(make_ustring(pt.friendly_name() + text(" (") + pt.unique_name() + text(")")));

    {
        //In my defense: Gtk::Widget::drag_source_set accepts a std::vector that clashes
        // with it's -D_GLIBCXX_DEBUG replacement std::__debug::vector
        gtk_drag_source_set(reinterpret_cast<GtkWidget *>(wgt.gobj()),
                            GDK_BUTTON1_MASK,
                            _tentries.data(),
                            _tentries.size(),
                            GDK_ACTION_MOVE);

        wgt.signal_drag_begin().connect([&](const auto & context) {
            if (_drag_begin_fun)
                _drag_begin_fun(context);
        });

        wgt.drag_source_set_icon(pixbuf);

        wgt.signal_drag_data_get().connect([&](const auto & context,
                                               auto & selection_data, guint info, guint time) {
            auto * ptptr = &pt;
            selection_data.set(sizeof(::har::part * ), reinterpret_cast<const guint8 *>(&ptptr),
                               sizeof(::har::part * ));
            if (_drag_data_get_fun)
                _drag_data_get_fun(context, selection_data, info, time);
        });
    }

    _flowbox.add(wgt);

    show_all_children();
}

void inventory::remove_part(part_h id) {
    auto node = _part_map.extract(id);
    if (!node.empty()) {
        _flowbox.remove(_part_map.at(id));
    }
}

decltype(inventory::_drag_begin_fun) & inventory::drag_begin_fun() {
    return _drag_begin_fun;
}

decltype(inventory::_drag_data_get_fun) & inventory::drag_data_get_fun() {
    return _drag_data_get_fun;
}

inventory::~inventory() noexcept = default;

//endregion

//region grid_properties

grid_properties::grid_properties(uint_t res, grid_t cat) : Gtk::Popover(),
                                                           _sizex(Gtk::Adjustment::create(0, 0, 64, 1)),
                                                           _sizey(Gtk::Adjustment::create(0, 0, 64, 1)),
                                                           _imgres(Gtk::Adjustment::create(res, 32, 256, 1)),
                                                           _cat(cat),
                                                           _updating(false) {
    auto & grd = *new Gtk::Grid();
    grd.set_column_spacing(5);
    grd.attach(*Gtk::manage(new Gtk::Label("Width:", 1., .5)), 0, 0, 1, 1);
    grd.attach(_sizex, 1, 0, 1, 1);
    grd.attach(*Gtk::manage(new Gtk::Label("Height:", 1., .5)), 0, 1, 1, 1);
    grd.attach(_sizey, 1, 1, 1, 1);
    grd.attach(*Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL)), 0, 2, 2, 1);
    grd.attach(*Gtk::manage(new Gtk::Label("Resolution:", 1., .5)), 0, 3, 1, 1);
    grd.attach(_imgres, 1, 3, 1, 1);

    _sizex.signal_value_changed().connect([&]() {
        if (_resize_fun && !_updating) {
            _resize_fun({ _cat, dcoords_t(_sizex.get_value_as_int(), _sizey.get_value_as_int()) });
        }
    });

    _sizey.signal_value_changed().connect([&]() {
        if (_resize_fun && !_updating) {
            _resize_fun({ _cat, dcoords_t(_sizex.get_value_as_int(), _sizey.get_value_as_int()) });
        }
    });

    _imgres.signal_value_changed().connect([&]() {
        if (_resolution_fun && !_updating) {
            _resolution_fun(_imgres.get_value_as_int());
        }
    });

    Gtk::Popover::add(*Gtk::manage(&grd));

    show_all_children();
}

har::dcoords_t grid_properties::size() const {
    return dcoords_t(_sizex.get_value(), _sizey.get_value());
}

har::uint_t grid_properties::resolution() const {
    return _imgres.get_value_as_int();
}

void grid_properties::set_size(const dcoords_t & to) {
    _updating = true;
    _sizex.set_value(to.x);
    _sizey.set_value(to.y);
    _updating = false;
}

std::function<void(const har::gcoords_t &)> & grid_properties::resize_fun() {
    return _resize_fun;
}

std::function<void(har::uint_t)> & grid_properties::resolution_fun() {
    return _resolution_fun;
}

grid_properties::~grid_properties() noexcept = default;

//endregion

//region grid

grid::grid(grid_t grid, uint_t res) : Gtk::Box(Gtk::ORIENTATION_VERTICAL),
                                      _header(Gtk::ORIENTATION_HORIZONTAL),
                                      _name(),
                                      _inv_btn(),
                                      _grid(),
                                      _over(),
                                      _fixed(),
                                      _img_ref(),
                                      _ref_pixbuf(),
                                      _updating(false),
                                      _target_entries(),
                                      _inventory(), _properties(res, grid),
                                      _size(), _res(res),
                                      _cargo_map(),
                                      _grid_hnd(grid) {
    _inventory.drag_begin_fun() = [&](auto ...) {
        _inv_btn.set_active(false);
        hide_overlay();
    };

    _inventory.drag_data_get_fun() = [&](auto ...) {
        show_overlay();
    };

    _ref_pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, _res, _res);
    _ref_pixbuf->fill(0x00000000);

    _name.set_has_frame(false);
    _name.set_alignment(0.5f);

    _header.set_margin_start(2);
    _header.set_margin_end(2);
    _header.set_margin_top(5);
    _header.set_margin_bottom(5);

    auto & inv_img = *new Gtk::Image();
    inv_img.set_from_icon_name("preferences-other-symbolic",
                               Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _inv_btn.set_image(*Gtk::manage(&inv_img));
    _inv_btn.set_direction(Gtk::ARROW_DOWN);
    _inv_btn.set_popover(_inventory);
    _inv_btn.set_relief(Gtk::RELIEF_NONE);
    _header.pack_start(*Gtk::manage(&_inv_btn), Gtk::PACK_SHRINK);

    _header.pack_start(_name, Gtk::PACK_EXPAND_WIDGET);

    auto & prop_img = *new Gtk::Image();
    prop_img.set_from_icon_name("view-more-symbolic", Gtk::ICON_SIZE_BUTTON);
    auto & prop_btn = *new Gtk::MenuButton();
    prop_btn.set_image(*Gtk::manage(&prop_img));
    prop_btn.set_direction(Gtk::ARROW_DOWN);
    prop_btn.set_popover(_properties);
    prop_btn.set_relief(Gtk::RELIEF_NONE);
    _header.pack_end(*Gtk::manage(&prop_btn), Gtk::PACK_SHRINK);

    _grid.set_column_spacing(0);
    _grid.set_row_spacing(0);
    _grid.set_column_homogeneous(true);
    _grid.set_row_homogeneous(true);
    _grid.insert_column(0);
    _grid.insert_column(1);
    _grid.insert_row(0);
    _grid.insert_row(1);
    _grid.set_halign(Gtk::ALIGN_CENTER);
    _grid.set_valign(Gtk::ALIGN_CENTER);

    _img_ref.set(_ref_pixbuf);
    _grid.attach(_img_ref, 0, 0, 1, 1);

    auto & scrl = *Gtk::manage(new Gtk::ScrolledWindow());
    auto & vprt = *Gtk::manage(new Gtk::Viewport(scrl.get_hadjustment(), scrl.get_vadjustment()));
    auto & aspc = *Gtk::manage(new Gtk::AspectFrame());

    _over.add(_grid);
    _over.add_overlay(_fixed);
    _over.reorder_overlay(_fixed, 1);
    _over.set_overlay_pass_through(_fixed, true);
    _over.set_halign(Gtk::ALIGN_CENTER);
    _over.set_valign(Gtk::ALIGN_CENTER);

    aspc.set_shadow_type(Gtk::SHADOW_NONE);
    aspc.property_obey_child() = true;
    aspc.add(_over);

    vprt.add(aspc);

    scrl.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrl.set_min_content_height(240);
    scrl.add(vprt);

    Gtk::Box::pack_start(_header, Gtk::PACK_SHRINK);
    Gtk::Box::pack_start(*Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL)),
                         Gtk::PACK_SHRINK);
    Gtk::Box::pack_end(scrl, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box::show_all_children();
}

cell & grid::create_cell(const gcoords_t & gc) {
    auto & c = *Gtk::manage(new cell(_target_entries));
    c.set_image(_ref_pixbuf);

    c.signal_button_press_event().connect([gc, &c, this](GdkEventButton * ebtn) {
        c.grab_focus();
        return _button_press_fun ? _button_press_fun(gc, ebtn) : false;
    });

    c.signal_button_release_event().connect([gc, this](GdkEventButton * ebtn) {
        return _button_release_fun ? _button_release_fun(gc, ebtn) : false;
    });

    c.signal_drag_begin().connect([gc, this](const Glib::RefPtr<Gdk::DragContext> & context) {
        hide_overlay();
        if (_drag_begin_fun)
            _drag_begin_fun(gc, context);
    });

    c.signal_drag_data_get().connect([gc, this](const Glib::RefPtr<Gdk::DragContext> & context,
                                                Gtk::SelectionData & sel,
                                                guint info,
                                                guint time) {
        show_overlay();
        sel.set(sizeof(gcoords_t), reinterpret_cast<const guint8 *>(&gc), sizeof(gcoords_t));
        if (_drag_data_get_fun)
            _drag_data_get_fun(gc, context, sel, info, time);
    });

    c.signal_drag_failed().connect([this](const Glib::RefPtr<Gdk::DragContext> &,
                                          Gtk::DragResult) -> bool {
        show_overlay();
        _drag_failed_fun();
        return false;
    });

    c.signal_drag_data_received().connect([gc, this](const Glib::RefPtr<Gdk::DragContext> & context,
                                                     int x, int y,
                                                     const Gtk::SelectionData & selection_data,
                                                     guint info,
                                                     guint time) {
        if (_drag_data_received_fun)
            _drag_data_received_fun(gc, context, x, y, selection_data, info, time);
        context->drag_finish(true, false, time);
    });
    return c;
}

void grid::insert_column() {
    ++_size.x;
    _grid.insert_column(_size.x);
    auto & lbl = *Gtk::manage(new Gtk::Label(std::to_string(_size.x)));
    _grid.attach(lbl, _size.x, 0, 1, 1);
    for (int_t i = 0; i < _size.y; ++i) {
        gcoords_t gc{ _grid_hnd, dcoords_t(_size.x - 1, i) };
        _grid.attach(create_cell(gc), _size.x, i + 1, 1, 1);
    }
    auto & pad = *Gtk::manage(new Gtk::Label());
    _grid.attach(pad, _size.x, _size.y + 1, 1, 1);
    show_all_children();
}

void grid::insert_row() {
    ++_size.y;
    _grid.insert_row(_size.y);
    auto & lbl = *Gtk::manage(new Gtk::Label(std::to_string(_size.y)));
    _grid.attach(lbl, 0, _size.y, 1, 1);
    for (int_t i = 0; i < _size.x; ++i) {
        gcoords_t gc{ _grid_hnd, dcoords_t(i, _size.y - 1) };
        _grid.attach(create_cell(gc), i + 1, _size.y, 1, 1);
    }
    auto & pad = *Gtk::manage(new Gtk::Label());
    _grid.attach(pad, _size.x + 1, _size.y, 1, 1);
    show_all_children();
}

void grid::remove_column() {
    for (dcoord_t i = 0; i <= _size.y + 1; ++i) {
        auto * child = _grid.get_child_at(_size.x, i);
        _grid.remove(*child);
    }
    _grid.remove_column(_size.x);
    --_size.x;
    show_all_children();
}

void grid::remove_row() {
    for (dcoord_t i = 0; i <= _size.x + 1; ++i) {
        auto * child = _grid.get_child_at(i, _size.y);
        _grid.remove(*child);
    }
    _grid.remove_row(_size.y);
    --_size.y;
    show_all_children();
}

har::dcoords_t grid::size() const {
    return _properties.size();
}

void grid::set_size(const dcoords_t & size) {
    _properties.set_size(size);
    while (size.x > _size.x) {
        insert_column();
    }
    while (size.y > _size.y) {
        insert_row();
    }
    while (size.x < _size.x) {
        remove_column();
    }
    while (size.y < _size.y) {
        remove_row();
    }
    _size = size;
}

har::uint_t grid::get_resolution() const {
    return _properties.resolution();
}

void grid::set_resolution(uint_t res) {
    _res = res;
}

cell & grid::at(const dcoords_t & pos) {
    return dynamic_cast<cell &>(*_grid.get_child_at(pos.x + 1, pos.y + 1));
}

cell & grid::at(cargo_h num) {
    return _cargo_map.at(num);
}

void grid::include_part(const har::part & pt) {
    _inventory.add_part(pt);
}

void grid::remove_part(part_h id) {
    _inventory.remove_part(id);
}

void grid::set_name(const string_t & name) {
    _updating = true;
    _name.set_text(make_ustring(name));
    _updating = false;
}

Glib::SignalProxy<void> grid::signal_name_set() {
    return _name.signal_activate();
}

std::function<void(const har::gcoords_t &)> & grid::resize_fun() {
    return _properties.resize_fun();
}

std::function<void(har::uint_t)> & grid::resolution_fun() {
    return _properties.resolution_fun();
}

std::vector<GtkTargetEntry> & grid::inventory_target_entries() {
    return _inventory.target_entries();
}

std::vector<GtkTargetEntry> & grid::cell_target_entries() {
    return _target_entries;
}

decltype(grid::_button_press_fun) & grid::button_press_fun() {
    return _button_press_fun;
}

decltype(grid::_button_release_fun) & grid::button_release_fun() {
    return _button_release_fun;
}

decltype(grid::_drag_begin_fun) & grid::drag_begin_fun() {
    return _drag_begin_fun;
}

decltype(grid::_drag_data_get_fun) & grid::drag_data_get_fun() {
    return _drag_data_get_fun;
}

decltype(grid::_drag_failed_fun) & grid::drag_failed_fun() {
    return _drag_failed_fun;
}

decltype(grid::_drag_data_received_fun) & grid::drag_data_received_fun() {
    return _drag_data_received_fun;
}

void grid::hide_overlay() {
    _fixed.set_visible(false);
}

void grid::show_overlay() {
    _fixed.set_visible(true);
}

grid::~grid() noexcept = default;

//endregion
