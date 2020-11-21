//
// Created by Johannes on 28.06.2020.
//

#include <filesystem>
#include <fstream>

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/paned.h>
#include <gtkmm/viewport.h>

#include <har/types.hpp>
#include <memory>

#include "main_win.hpp"
#include "about.hpp"

using namespace har::gui_;

main_win::main_win(gui & parti,
                   har::co_queue<std::function<void()>> & queue) : Gtk::Window(),
                                                                   _parti(parti),
                                                                   _dispatcher(),
                                                                   _queue(queue),
                                                                   _headerbar(),
                                                                   _model(grid_t::MODEL_GRID, 48u),
                                                                   _bank(grid_t::BANK_GRID, 32u),
                                                                   _action_bar(parti._cycle_delta),
                                                                   _terminal(),
                                                                   _properties([this](of id, value && val) {
                                                                       prop_changed(id, std::forward<value>(val));
                                                                   }),
                                                                   _conn_popover(),
                                                                   _selected(gcoords_t{ grid_t::INVALID_GRID, -1, -1 }),
                                                                   _pressed(gcoords_t{ grid_t::INVALID_GRID, -1, -1 }),
                                                                   _selected_img(nullptr),
                                                                   _updating(0),
                                                                   _path(),
                                                                   _last_serialized(),
                                                                   _undo_queue(),
                                                                   _redo_queue() {
    bind();

    set_titlebar(_headerbar);
    auto & lbox = *Gtk::manage(new Gtk::VBox());
    auto & lpnd = *Gtk::manage(new Gtk::VPaned());
    lpnd.set_wide_handle(true);
    auto & pnd_model = *Gtk::manage(new Gtk::HPaned());
    pnd_model.set_wide_handle(true);
    pnd_model.pack1(_model, Gtk::EXPAND);
    pnd_model.pack2(_bank, Gtk::EXPAND);

    lbox.pack_start(*Gtk::manage(&pnd_model), Gtk::PACK_EXPAND_WIDGET);
    lbox.pack_end(_action_bar, Gtk::PACK_SHRINK);

    auto & tscrl = *Gtk::manage(new Gtk::ScrolledWindow());
    auto & tvprt = *Gtk::manage(new Gtk::Viewport(tscrl.get_hadjustment(), tscrl.get_vadjustment()));
    tvprt.add(_terminal);
    tscrl.add(tvprt);
    tscrl.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    lpnd.pack1(lbox, Gtk::EXPAND);
    lpnd.pack2(tscrl, Gtk::EXPAND);

    auto & pnd_main = *new Gtk::HPaned();
    pnd_main.set_wide_handle(true);
    pnd_main.pack1(lpnd, Gtk::EXPAND);
    pnd_main.pack2(_properties, Gtk::FILL);
    Gtk::Window::add(pnd_main);

    show_all_children();

    _dispatcher.connect(([&] { dispatch(); }));
}

void main_win::bind() {
    /*Headerbar*/ {
        _headerbar.signal_open_clicked().connect([&]() {
            btn_open_clicked();
        });

        _headerbar.signal_save_clicked().connect([&]() {
            btn_save_clicked();
        });

        _headerbar.signal_save_as_clicked().connect([&]() {
            btn_save_as_clicked();
        });

        _headerbar.signal_about_activate().connect([&]() {
            about dlg{ };
            dlg.run();
        });
    }

    /*Grids*/ {
        _model.resize_fun() =
        _bank.resize_fun() = [&](const gcoords_t & to) {
            auto ctx = _parti.get().request();
            ctx.resize_grid(to);
        };

        GtkTargetEntry model_place_entry{
                const_cast<gchar *>("har::model_place"),
                GTK_TARGET_SAME_APP,
                0 };

        GtkTargetEntry model_entry{
                const_cast<gchar *>("har::model"),
                GTK_TARGET_SAME_APP,
                1 };

        GtkTargetEntry bank_place_entry{
                const_cast<gchar *>("har::bank_place"),
                GTK_TARGET_SAME_APP,
                2 };

        GtkTargetEntry bank_entry{
                const_cast<gchar *>("har:bank"),
                GTK_TARGET_SAME_APP,
                4 };

        GtkTargetEntry conn_entry{
                const_cast<gchar *>("har::conn"),
                GTK_TARGET_SAME_APP,
                8 };

        _model.cell_target_entries() = { model_entry, conn_entry, model_place_entry };
        _model.inventory_target_entries() = { model_place_entry };

        _bank.cell_target_entries() = { bank_entry, conn_entry, bank_place_entry };
        _bank.inventory_target_entries() = { bank_place_entry };

        _model.drag_begin_fun() =
        _bank.drag_begin_fun() = [this](auto ...) {
            {
                auto ctx = _parti.get().request();
                gcoords_t pressed{ grid_t::INVALID_GRID, 0, 0 };
                if (_pressed.index() == cell_cat::GRID_CELL) {
                    pressed = std::get<cell_cat::GRID_CELL>(_pressed);
                    auto fgcl = ctx.at(pressed);
                    fgcl.logic().release(fgcl, ccoords_t());
                }
            }
            _model.hide_overlay();
            _bank.hide_overlay();
        };

        _model.button_press_fun() =
        _bank.button_press_fun() = [this](const gcoords_t & pos, GdkEventButton * ebtn) -> bool {
            if (ebtn->button == 1u) {
                auto ctx = _parti.get().request();
                cell_clicked(pos, ccoords_t(ebtn->x, ebtn->y), ctx);
                return true;
            } else if (ebtn->button == 3u) {
                auto ctx = _parti.get().request();
                if (ebtn->state & Gdk::SHIFT_MASK) {
                    cell_cycle(pos, ctx);
                } else {
                    cell_selected(pos, ctx);
                }
                return true;
            } else {
                return false;
            }
        };

        _model.button_release_fun() =
        _bank.button_release_fun() = [this](const gcoords_t & pos, GdkEventButton * ebtn) -> bool {
            if (ebtn->button == 1u) {
                auto ctx = _parti.get().request();
                cell_released(pos, ccoords_t(ebtn->x, ebtn->y), ctx);
                return true;
            } else {
                return false;
            }
        };

        _model.drag_data_get_fun() =
        _bank.drag_data_get_fun() = [&](const gcoords_t & pos,
                                        const Glib::RefPtr<Gdk::DragContext> & dctx,
                                        Gtk::SelectionData & sel,
                                        guint info,
                                        guint time) {

        };

        _model.drag_failed_fun() =
        _bank.drag_failed_fun() = [this] {
            _model.show_overlay();
            _bank.show_overlay();
        };

        _model.drag_data_received_fun() =
        _bank.drag_data_received_fun() = [=](const gcoords_t & to,
                                             const Glib::RefPtr<Gdk::DragContext> & context, int x, int y,
                                             const Gtk::SelectionData & selection_data,
                                             guint info, guint time) {
            auto target = selection_data.get_target();
            auto actions = context->get_suggested_action();

            if ((target == model_entry.target ||
                 target == bank_entry.target ||
                 target == conn_entry.target) && actions == Gdk::ACTION_COPY) {
                gcoords_t from = reinterpret_cast<const gcoords_t &>(*selection_data.get_data());
                auto ctxptr = std::make_unique<participant::context>(_parti.get().request());
                auto & ctx = *ctxptr;
                if (!(ctx.at(to).traits() & traits::EMPTY_PART)) {
                    std::unique_ptr<full_grid_cell> fromgcl{ new full_grid_cell(ctx.at(from)) };
                    std::unique_ptr<full_grid_cell> togcl{ new full_grid_cell(ctx.at(to)) };
                    _conn_popover.set_cell(std::move(ctxptr),
                                           std::move(fromgcl), get_cell_image(from),
                                           std::move(togcl), get_cell_image(to));
                    _conn_popover.set_relative_to(to.cat == grid_t::MODEL_GRID ?
                                                  _model.at(to.pos) :
                                                  _bank.at(to.pos));
                    _conn_popover.popup();
                }
            } else if ((target == model_entry.target ||
                        target == bank_entry.target) && actions == Gdk::ACTION_MOVE) {
                auto ctx = _parti.get().request();
                gcoords_t from = reinterpret_cast<const gcoords_t &>(*selection_data.get_data());
                cell_moved(from, to, ctx);
                cell_selected(to, ctx);
            } else if ((target == model_place_entry.target ||
                        target == bank_place_entry.target) && actions == Gdk::ACTION_MOVE) {
                auto ctx = _parti.get().request();
                auto & pt = **reinterpret_cast<const ::har::part * const *>(selection_data.get_data());
                cell_placed(to, pt, ctx);
            }
            _model.show_overlay();
            _bank.show_overlay();
        };

        _model.resolution_fun() =
        _bank.resolution_fun() = [=](uint_t res) {
            _parti.get().redraw_all();
        };
    }

    /*Action bar*/ {
        _action_bar.run_fun() = [&]() {
            _parti.get().start();
        };

        _action_bar.step_fun() = [&]() {
            _parti.get().step();
        };

        _action_bar.stop_fun() = [&]() {
            _parti.get().stop();
        };

        _action_bar.get_timing_control().on_change_fun() = [this](auto us) {
            _parti.get().set_cycle(us);
        };
    }
}

void main_win::btn_new_clicked() {
    //TODO: Implement
}

void main_win::btn_open_clicked() {
    Gtk::FileChooserDialog dialog("Open", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    auto filter_all = Gtk::FileFilter::create();
    auto filter_ham = Gtk::FileFilter::create();
    filter_all->set_name("All (*.*)");
    filter_ham->set_name("HAR Model (*.ham)");
    filter_ham->add_mime_type("text/ham");
    filter_all->add_pattern("*");
    filter_ham->add_pattern("*.ham");

    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Open", Gtk::RESPONSE_OK);

    dialog.add_filter(filter_ham);
    dialog.add_filter(filter_all);
    dialog.set_filter(filter_ham);

    if (!_path.empty()) {
        dialog.select_filename(_path);
    }

    if (dialog.run() == Gtk::RESPONSE_OK) {
        _path = dialog.get_filename();
        if (std::filesystem::exists(_path)) {
            {
                auto ctx = _parti.get().request();
                cell_selected(gcoords_t(), ctx);
            }
            har::ifstream ifs(_path);
            _parti.get().load_model(ifs);
            _headerbar.set_subtitle(_path);
        } else {
            Gtk::MessageDialog dlg{ *this,
                                    "Error",
                                    false,
                                    Gtk::MESSAGE_ERROR,
                                    Gtk::BUTTONS_OK,
                                    true };
            dlg.set_secondary_text("File \"" + _path + "\" does not exist.");
            dlg.run();
        }
    }
}

bool main_win::btn_save_clicked() {
    if (_path.empty()) {
        return btn_save_as_clicked();
    } else {
        ofstream ofs(_path);
        if (ofs.is_open()) {
            _parti.get().store_model(ofs);
            return true;
        } else {
            Gtk::MessageDialog dlg{ *this,
                                    "Error",
                                    false,
                                    Gtk::MESSAGE_ERROR,
                                    Gtk::BUTTONS_OK,
                                    true };
            dlg.set_secondary_text("Can't save model under \"" + _path + "\".");
            dlg.run();
            return false;
        }
    }
}

bool main_win::btn_save_as_clicked() {
    Gtk::FileChooserDialog dialog("Save as", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    auto filter_all = Gtk::FileFilter::create();
    auto filter_ham = Gtk::FileFilter::create();
    filter_all->set_name("All (*.*)");
    filter_all->add_pattern("*");
    filter_ham->set_name("HAR Model (*.ham)");
    filter_ham->add_mime_type("text/x.har-model");
    filter_ham->add_pattern("*.ham");

    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Save", Gtk::RESPONSE_OK);

    dialog.add_filter(filter_ham);
    dialog.add_filter(filter_all);
    dialog.set_filter(filter_ham);

    if (_path.empty()) dialog.set_current_name("model.ham");
    else dialog.set_filename(_path);

    if (dialog.run() == Gtk::RESPONSE_OK) {
        _path = dialog.get_filename();
        har::ofstream ofs(_path);
        if (ofs.is_open()) {
            _parti.get().store_model(ofs);
            _headerbar.set_subtitle(_path);
            return true;
        } else {
            Gtk::MessageDialog dlg{ *this,
                                    "Error",
                                    false,
                                    Gtk::MESSAGE_ERROR,
                                    Gtk::BUTTONS_OK,
                                    true };
            dlg.set_secondary_text("Can't save model under \"" + _path + "\".");
            dlg.run();
            return false;
        }
    } else {
        return false;
    }
}

void main_win::btn_reset_clicked() {
    //TODO: Implement
}

void main_win::prop_changed(of id, value && val) {
    if (!_updating) {
        auto ctx = _parti.get().request();
        if (cell_cat(_selected.index()) == cell_cat::GRID_CELL) {
            auto gcl = ctx.at(std::get<uint_t(cell_cat::GRID_CELL)>(_selected));
            if (val.type() != value::datatype::CALLBACK) {
                gcl[id] = val;
            } else {
                auto & cb = gcl[id].as<callback_t>();
                if (cb) {
                    cb();
                }
            }
        }
    }
}

void main_win::cell_placed(const gcoords_t & pos, const har::part & pt, participant::context & ctx) {
    auto fgcl = ctx.at(pos);
    cell_selected(pos, ctx);
    fgcl.remove_all_connections();
    ctx.commit();
    fgcl.set_part(pt);
    ctx.commit();

    for (auto dir : direction::cardinal) {
        fgcl[dir].redraw();
    }

    cell_selected(pos, ctx);
}

void main_win::cell_clicked(const gcoords_t & pos, const ccoords_t & at, participant::context & ctx) {
    auto fgcl = ctx.at(pos);
    _pressed = pos;
    fgcl.logic().press(fgcl, at);
}

void main_win::cell_selected(const gcoords_t & pos, participant::context & ctx) {
    auto old_selected = _selected;
    _selected = pos;
    if (pos.cat != grid_t::INVALID_GRID) {
        auto fgcl = ctx.at(pos);
        _properties.set_cell(fgcl, [&](of id, value && val) {
            prop_changed(id, std::forward<value>(val));
        }, [pos, this](direction_t use) {
            cell_disconnected(pos, use);
        }, _model_info.editable);
        draw(_selected, ctx);
        if (old_selected.index() == 1 && std::get<1>(old_selected).cat != grid_t::INVALID_GRID) {
            draw(old_selected, ctx);
        }
    } else {
        auto fgcl = ctx.at(pos);
        _properties.unset_cell();
        draw(_selected, ctx);
        if (old_selected.index() == 1 && std::get<1>(old_selected).cat != grid_t::INVALID_GRID) {
            draw(old_selected, ctx);
        }
    }
    _parti.get().select(pos);
}

void main_win::cell_released(const gcoords_t & pos, const ccoords_t & at, participant::context & ctx) {
    auto fgcl = ctx.at(pos);
    _pressed = gcoords_t(grid_t::INVALID_GRID, 0, 0);
    fgcl.logic().release(fgcl, at);
}

void main_win::cell_moved(const gcoords_t & from, const gcoords_t & to, participant::context & ctx) {
    ctx.at(from).swap_with(ctx.at(to));
    cell_selected(to, ctx);
}

void main_win::cell_connected(const gcoords_t & from, const gcoords_t & to, participant::context & ctx) {
    if (cell_cat(_selected.index()) == cell_cat::GRID_CELL) {
        if (from == std::get<uint_t(cell_cat::GRID_CELL)>(_selected)) {
            cell_selected(from, ctx);
        }
    }
}

void main_win::cell_disconnected(const gcoords_t & pos, direction_t use) {
    auto ctx = _parti.get().request();
    auto fgcl = ctx.at(pos);
    if (fgcl.has_connection(use)) {
        fgcl.remove_connection(use);
    }
}

void main_win::cell_cycle(const gcoords_t & pos, participant::context & ctx) {
    auto fgcl = ctx.at(pos);
    fgcl.logic().cycle(fgcl);
}

void main_win::draw(const cell_h & hnd, participant::context & ctx) {
    har::image_t img = get_image_base(hnd);
    uint_t res = 64u;
    {
        switch (cell_cat(hnd.index())) {
            case cell_cat::GRID_CELL: {
                const gcoords_t & gc = std::get<uint_t(cell_cat::GRID_CELL)>(hnd);
                if (gc.cat == MODEL_GRID) {
                    res = _model.get_resolution();
                } else if (gc.cat == BANK_GRID) {
                    res = _bank.get_resolution();
                }

                auto gcl = ctx.at(gc);
                gcl.logic().draw(gcl, img);
                break;
            }
            case cell_cat::CARGO_CELL: {
                res = _model.get_resolution();
                auto ccl = ctx.at(std::get<uint_t(cell_cat::CARGO_CELL)>(hnd));
                ccl.logic().draw(ccl, img);
                break;
            }
            default: {
                return;
            }
        }
    }
    if (img.type() == typeid(image_in_t)) {
        auto[sf, dim] = std::any_cast<image_in_t>(img);
        auto base_img = Gdk::Pixbuf::create(sf, 0, 0, dim, dim)
                ->scale_simple(res, res, Gdk::INTERP_BILINEAR);
        if (hnd == _selected) {
            {
                auto cr = Cairo::Context::create(sf);

                cr->save();
                cr->set_source_rgba(.204, .396, .643, .5);
                cr->rectangle(0, 0, dim, dim);
                cr->fill();
                cr->stroke();
                cr->restore();
            }
            auto sel_img = Gdk::Pixbuf::create(sf, 0, 0, dim, dim)
                    ->scale_simple(res, res, Gdk::INTERP_BILINEAR);
            img = std::make_pair(sel_img, base_img);
        } else {
            img = base_img;
        }
        redraw(hnd, std::move(img));
    }
}

Glib::RefPtr<const Gdk::Pixbuf> main_win::get_cell_image(const gcoords_t & pos) {
    if (_selected.index() == uint_t(cell_cat::GRID_CELL)) {
        if (std::get<uint_t(cell_cat::GRID_CELL)>(_selected) == pos) {
            return _selected_img;
        }
    }
    switch (pos.cat) {
        case MODEL_GRID:
            return _model.at(pos.pos).get_image();
        case BANK_GRID:
            return _bank.at(pos.pos).get_image();
        default:
            return Glib::RefPtr<Gdk::Pixbuf>(nullptr);
    }
}

void main_win::dispatch() {
    auto & queue = _queue.get();
    while (!queue.empty()) {
        auto fun = queue.pop();
        fun();
    }
}

bool main_win::on_key_release_event(GdkEventKey * key_event) {
    switch (key_event->keyval) {
        case GDK_KEY_Escape: {
            auto ctx = _parti.get().request();
            cell_selected({ grid_t::INVALID_GRID, dcoords_t(-1, -1) }, ctx);
            return true;
        }
        case GDK_KEY_Delete: {
            if (_selected.index() == cell_cat::GRID_CELL) {
                gcoords_t pos = std::get<cell_cat::GRID_CELL>(_selected);
                if (pos.cat != grid_t::INVALID_GRID) {
                    const har::part & pt = (pos.cat == grid_t::MODEL_GRID) ?
                                           _empty_model_part.value() :
                                           _empty_bank_part.value();
                    auto ctx = _parti.get().request();
                    cell_placed(pos, pt, ctx);
                }
            }
        }
        default:
            break;
    }
    if (key_event->state & GDK_CONTROL_MASK) {
        switch (key_event->keyval) {
            case GDK_KEY_n:
            case GDK_KEY_N: {
                btn_new_clicked();
                return true;
            }
            case GDK_KEY_o:
            case GDK_KEY_O: {
                btn_open_clicked();
                return true;
            }
            case GDK_KEY_s:
            case GDK_KEY_S: {
                if (key_event->state & GDK_SHIFT_MASK) {
                    btn_save_as_clicked();
                } else {
                    btn_save_clicked();
                }
                return true;
            }
        }
    }
    return false;
}

bool main_win::on_delete_event(GdkEventAny * any_event) {
    if (!_undo_queue.empty()) {
        Gtk::MessageDialog dlg(*this,
                               "Unsaved changes",
                               false,
                               Gtk::MESSAGE_WARNING,
                               Gtk::BUTTONS_NONE,
                               true);
        dlg.set_secondary_text("There are unsaved changes to this model.\nClose anyway?");
        dlg.add_button("Save", 0);
        dlg.add_button("Cancel", 1);
        dlg.add_button("Close anyway", 2);
        bool done = false;
        do {
            switch (dlg.run()) {
                case 0: {
                    done = btn_save_clicked();
                    break;
                }
                case 1: {
                    return true;
                }
                case 2: {
                    return false;
                }
            }
        } while (!done);
        return true;
    } else {
        return false;
    }
}

const har::cell_h & main_win::get_selected() const {
    return _selected;
}

har::image_t main_win::process_image(har::cell_h hnd, har::image_t & img) {
    using ImageType = std::tuple<Cairo::RefPtr<Cairo::Surface>, uint_t>;
    if (img.type() == typeid(ImageType)) {
        uint_t res = 64u;
        switch (cell_cat(hnd.index())) {
            case cell_cat::GRID_CELL: {
                grid_t g = std::get<uint_t(cell_cat::GRID_CELL)>(hnd).cat;
                switch (g) {
                    case MODEL_GRID: {
                        res = _model.get_resolution();
                        break;
                    }
                    case BANK_GRID: {
                        res = _bank.get_resolution();
                        break;
                    }
                    case INVALID_GRID: {
                        res = 64u;
                        break;
                    }
                }
                break;
            }
            case cell_cat::CARGO_CELL: {
                res = _model.get_resolution();
                break;
            }
            case cell_cat::INVALID_CELL:
                res = 64u;
                break;
        }
        auto &[sf, dim] = *std::any_cast<ImageType>(&img);
        auto base_img = Gdk::Pixbuf::create(sf, 0, 0, dim, dim)
                ->scale_simple(res, res, Gdk::INTERP_BILINEAR);
        if (hnd == _selected) {
            {
                auto cr = Cairo::Context::create(sf);

                cr->save();
                cr->set_source_rgba(.204, .396, .643, .5);
                cr->rectangle(0, 0, dim, dim);
                cr->fill();
                cr->stroke();
                cr->restore();
            }
            auto sel_img = Gdk::Pixbuf::create(sf, 0, 0, dim, dim)
                    ->scale_simple(res, res, Gdk::INTERP_BILINEAR);
            img = std::make_pair(sel_img, base_img);
        } else {
            img = base_img;
        }
    }
    return img;
}

void main_win::set_grid_size(const gcoords_t & to) {
    _updating = true;
    switch (to.cat) {
        case MODEL_GRID: {
            break;
        }
        case BANK_GRID: {
            break;
        }
        case INVALID_GRID: {
            break;
        }
    }
    _updating = false;
}

image_out_t main_win::get_image_base(const cell_h & hnd) {
    return Glib::RefPtr<Gdk::Pixbuf>(nullptr);
}

void main_win::include_part(const har::part & pt) {
    auto traits = pt.traits();
    if (traits & traits::COMPONENT_PART) {
        _model.include_part(pt);
        if (pt.traits() & traits::EMPTY_PART) {
            _empty_model_part = std::ref(pt);
        }
    }
    if (traits & traits::BOARD_PART) {
        _bank.include_part(pt);
        if (pt.traits() & traits::EMPTY_PART) {
            _empty_bank_part = std::ref(pt);
        }
    }
}

void main_win::remove_part(part_h id) {
    _model.remove_part(id);
    _bank.remove_part(id);
}

void main_win::resize_grid(const gcoords_t & to) {
    switch (to.cat) {
        case MODEL_GRID: {
            _model.set_size(to.pos);
            break;
        }
        case BANK_GRID: {
            _bank.set_size(to.pos);
            break;
        }
        default: {
            break;
        }
    }
    if (cell_cat(_selected.index()) == cell_cat::GRID_CELL) {
        auto sel = std::get<1>(_selected);
        if (sel.cat == to.cat &&
            (sel.pos.x >= to.pos.x - 1 ||
             sel.pos.y >= to.pos.y - 1)) {
            auto ctx = _parti.get().request();
            cell_selected({ grid_t::INVALID_GRID, dcoords_t(-1, -1) }, ctx);
        }
    }
}

void main_win::model_loaded() {
    //TODO: Implement
}

void main_win::info_updated(const model_info & info) {
    if (!info.title.empty()) {
        _headerbar.set_title(make_ustring(info.title + text(" - HAR")));
    } else {
        _headerbar.set_title("HAR");
    }
    if (auto it = info.titles.find(grid_t::MODEL_GRID); it != info.titles.end()) {
        _model.set_name(it->second);
    } else {
        _model.set_name(text(""));
    }
    if (auto it = info.titles.find(grid_t::BANK_GRID); it != info.titles.end()) {
        _bank.set_name(it->second);
    } else {
        _bank.set_name(text(""));
    }
    _model_info = info;
    //TODO: Display remaining information somewhere
}

void main_win::run(bool_t responsible) {
    _action_bar.set_run();
}

void main_win::step() {
    _action_bar.set_step();
}

void main_win::stop() {
    _action_bar.set_stop();
}

void main_win::message(const string_t & header, const string_t & content) {
    Gtk::MessageDialog dlg{ *this,
                            make_ustring(header),
                            true,
                            Gtk::MESSAGE_INFO,
                            Gtk::BUTTONS_OK,
                            true };
    dlg.set_secondary_text(make_ustring(content));
    dlg.run();
}

void main_win::exception(const har::exception::exception & e) {
    Gtk::MessageDialog dlg{ *this,
                            "Exception",
                            true,
                            Gtk::MESSAGE_ERROR,
                            Gtk::BUTTONS_OK,
                            true };
    dlg.set_secondary_text(e.what());
    dlg.run();
}

void main_win::selection_update(cell_h hnd, entry_h id, const value & val) {
    if (_selected == hnd) {
        _updating++;
        _properties.get_proplist().get_row(id).set_value(val);
        _updating--;
    }
}

void main_win::redraw(const har::cell_h & hnd, har::image_t && img) {
    Glib::RefPtr<Gdk::Pixbuf> imgref{ nullptr };
    if (img.type() == typeid(decltype(imgref))) {
        imgref = std::any_cast<image_out_t>(img);
    } else {
        std::pair<Glib::RefPtr<Gdk::Pixbuf>, Glib::RefPtr<Gdk::Pixbuf>> img_pair;
        img_pair = std::any_cast<decltype(img_pair)>(img);
        std::tie(imgref, _selected_img) = img_pair;
    }
    switch (cell_cat(hnd.index())) {
        case cell_cat::GRID_CELL: {
            const auto & gc = std::get<uint_t(cell_cat::GRID_CELL)>(hnd);
            auto & pgrid = (gc.cat == grid_t::MODEL_GRID) ? _model : _bank;
            auto pimg = (hnd == _selected) ?
                        _selected_img->scale_simple(pgrid.get_resolution(),
                                                    pgrid.get_resolution(),
                                                    Gdk::INTERP_BILINEAR) :
                        imgref;
            if (gc.cat != grid_t::INVALID_GRID) {
                pgrid.at(gc.pos).set_image(imgref, pimg);
            }
            break;
        }
        case cell_cat::CARGO_CELL: {
            //TODO: Implement
            break;
        }
        default: {
            break;
        }
    }
}

void main_win::connection_added(const gcoords_t & from, const gcoords_t & to, direction_t use) {
    if (_selected == cell_h(from)) {
        _properties.get_connlist().get_row(use).set_adjacent(to);
    }
}

void main_win::connection_removed(const gcoords_t & from, direction_t use) {
    if (_selected == cell_h(from)) {
        _properties.get_connlist().get_row(use).set_adjacent(gcoords_t());
    }
}

void main_win::cargo_spawned(cargo_h num) {
    //TODO: Implement
}

void main_win::cargo_moved(cargo_h num, const ccoords_t & to) {
    //TODO: Implement
}

void main_win::cargo_destroyed(cargo_h num) {
    //TODO: Implement
}

void main_win::emit() {
    _dispatcher.emit();
}

main_win::~main_win() noexcept = default;
