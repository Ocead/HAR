//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_HEADERBAR_HPP
#define HAR_GUI_HEADERBAR_HPP

#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/menuitem.h>

namespace har::gui_ {
    class headerbar : public Gtk::HeaderBar {
    private:
        Gtk::Button _btn_new;
        Gtk::Button _btn_open;
        Gtk::Button _btn_plugin;

        Gtk::Button _btn_save;
        Gtk::Button _btn_save_as;

        Gtk::Button _btn_revert;

        Gtk::Button _btn_undo;
        Gtk::Button _btn_redo;

        Gtk::Button _btn_properties;
        Gtk::MenuButton _mbn_info;

        Gtk::Menu _men_info;
        Gtk::MenuItem _mei_help;
        Gtk::MenuItem _mei_about;

    public:
        headerbar();

        Glib::SignalProxy<void> signal_new_clicked();

        Glib::SignalProxy<void> signal_open_clicked();

        Glib::SignalProxy<void> signal_save_clicked();

        Glib::SignalProxy<void> signal_save_as_clicked();

        Glib::SignalProxy<void> signal_revert_clicked();

        Glib::SignalProxy<void> signal_undo_clicked();

        Glib::SignalProxy<void> signal_redo_clicked();

        Glib::SignalProxy<void> signal_help_activate();

        Glib::SignalProxy<void> signal_about_activate();

        void enable_revert(bool enable);

        void enable_undo(bool enable);

        void enable_redo(bool enable);

        ~headerbar() noexcept override;
    };
}

#endif //HAR_GUI_HEADERBAR_HPP
