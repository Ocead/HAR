//
// Created by Johannes on 17.07.2020.
//

#ifndef HAR_GUI_ACTION_BAR_HPP
#define HAR_GUI_ACTION_BAR_HPP

#include <har/types.hpp>

#include <gtkmm/actionbar.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/togglebutton.h>

namespace har::gui_ {
    class action_bar : public Gtk::ActionBar {
    private:
        Gtk::ToggleButton _run;
        Gtk::Button _step;
        Gtk::ToggleButton _stop;

        std::function<void()> _run_fun;
        std::function<void()> _step_fun;
        std::function<void()> _stop_fun;

        har::bool_t _updating;

    public:
        action_bar();

        void set_run();

        void set_step();

        void set_stop();

        std::function<void()> & run_fun();

        std::function<void()> & step_fun();

        std::function<void()> & stop_fun();

        ~action_bar() noexcept override;
    };
}

#endif //HAR_GUI_ACTION_BAR_HPP
