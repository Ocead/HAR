//
// Created by Johannes on 28.06.2020.
//

#include <gtkmm/buttonbox.h>
#include <gtkmm/image.h>
#include <gtkmm/separator.h>

#include <headerbar.hpp>

using namespace har::gui_;

headerbar::headerbar() : Gtk::HeaderBar() {
    set_title("HAR");
    set_subtitle("~");
    set_show_close_button();

    Gtk::Image * imgptr;

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("document-new-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_new.set_image(*imgptr);
    _btn_new.set_tooltip_text("New");

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("document-open-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_open.set_image(*imgptr);
    _btn_open.set_tooltip_text("Open");

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("application-x-addon-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_plugin.set_image(*imgptr);
    _btn_plugin.set_tooltip_text("Load plugin");

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("document-save-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_save.set_image(*imgptr);
    _btn_save.set_tooltip_text("Save");

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("document-save-as-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_save_as.set_image(*imgptr);
    _btn_save_as.set_tooltip_text("Save as");

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("document-revert-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_revert.set_image(*imgptr);
    _btn_revert.set_tooltip_text("Revert");
    _btn_revert.set_sensitive(false);

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("edit-undo-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_undo.set_image(*imgptr);
    _btn_undo.set_tooltip_text("Undo");
    _btn_undo.set_sensitive(false);

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("edit-redo-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_redo.set_image(*imgptr);
    _btn_redo.set_tooltip_text("Redo");
    _btn_redo.set_sensitive(false);

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("document-properties-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _btn_properties.set_image(*imgptr);
    _btn_properties.set_tooltip_text("Properties");

    imgptr = Gtk::manage(new Gtk::Image());
    imgptr->set_from_icon_name("help-about-symbolic", Gtk::IconSize(Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
    _mbn_info.set_image(*imgptr);
    _mbn_info.set_tooltip_text("Info");

    Gtk::ButtonBox * bbx;

    bbx = Gtk::manage(new Gtk::ButtonBox());
    bbx->set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_EXPAND);
    bbx->pack_start(_btn_new);
    bbx->pack_start(_btn_open);
    bbx->pack_start(_btn_plugin);
    Gtk::HeaderBar::pack_start(*bbx);

    bbx = Gtk::manage(new Gtk::ButtonBox());
    bbx->set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_EXPAND);
    bbx->pack_start(_btn_save);
    bbx->pack_start(_btn_save_as);
    Gtk::HeaderBar::pack_start(*bbx);

    Gtk::HeaderBar::pack_start(*Gtk::manage(new Gtk::Separator(Gtk::Orientation::ORIENTATION_VERTICAL)));

    Gtk::HeaderBar::pack_start(_btn_revert);

    bbx = Gtk::manage(new Gtk::ButtonBox());
    bbx->set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_EXPAND);
    bbx->pack_start(_btn_undo);
    bbx->pack_start(_btn_redo);
    Gtk::HeaderBar::pack_start(*bbx);

    _mei_help.set_label("Help");
    _men_info.append(_mei_help);
    _mei_about.set_label("About");
    _men_info.append(_mei_about);
    _men_info.show_all();
    _mbn_info.set_menu(_men_info);
    Gtk::HeaderBar::pack_end(_mbn_info);

    Gtk::HeaderBar::pack_end(*Gtk::manage(new Gtk::Separator(Gtk::Orientation::ORIENTATION_VERTICAL)));

    Gtk::HeaderBar::pack_end(_btn_properties);
}

Glib::SignalProxy<void> headerbar::signal_new_clicked() {
    return _btn_new.signal_clicked();
}

Glib::SignalProxy<void> headerbar::signal_open_clicked() {
    return _btn_open.signal_clicked();
}

Glib::SignalProxy<void> headerbar::signal_save_clicked() {
    return _btn_save.signal_clicked();
}

Glib::SignalProxy<void> headerbar::signal_save_as_clicked() {
    return _btn_save_as.signal_clicked();
}

Glib::SignalProxy<void> headerbar::signal_revert_clicked() {
    return _btn_revert.signal_clicked();
}

Glib::SignalProxy<void> headerbar::signal_undo_clicked() {
    return _btn_undo.signal_clicked();
}

Glib::SignalProxy<void> headerbar::signal_redo_clicked() {
    return _btn_redo.signal_clicked();
}

Glib::SignalProxy<void> headerbar::signal_help_activate() {
    return _mei_help.signal_activate();
}

Glib::SignalProxy<void> headerbar::signal_about_activate() {
    return _mei_about.signal_activate();
}


void headerbar::enable_revert(bool enable) {
    _btn_revert.set_sensitive(enable);
}

void headerbar::enable_undo(bool enable) {
    _btn_undo.set_sensitive(enable);
}

void headerbar::enable_redo(bool enable) {
    _btn_redo.set_sensitive(enable);
}


headerbar::~headerbar() noexcept = default;
