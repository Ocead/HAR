//
// Created by Johannes on 04.08.2020.
//

#ifndef HAR_GUI_TERMINAL_HPP
#define HAR_GUI_TERMINAL_HPP

#include <gtkmm/textview.h>

namespace har::gui_ {
    class terminal : public Gtk::TextView {
    public:
        terminal();

        ~terminal() noexcept override;
    };
}

#endif //HAR_GUI_TERMINAL_HPP
