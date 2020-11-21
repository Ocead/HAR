//
// Created by Johannes on 21.11.2020.
//

#ifndef HAR_GUI_TIMING_CONTROL_HPP
#define HAR_GUI_TIMING_CONTROL_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>

namespace har::gui_ {
    class timing_control : public Gtk::Box {
    private:
        Gtk::Entry _entry;
        Gtk::ComboBoxText _units;
        Gtk::Button _lock;
        std::chrono::microseconds _timing;
        std::function<void(std::chrono::microseconds)> _on_change_fun;

        void update_timing(bool_t updating);

    public:
        explicit timing_control(std::chrono::microseconds us);

        decltype(_on_change_fun) & on_change_fun();

        std::chrono::microseconds get_timing() const;

        void set_timing(std::chrono::microseconds timing);

        ~timing_control() noexcept override;
    };
}

#endif //HAR_GUI_TIMING_CONTROL_HPP
