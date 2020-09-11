//
// Created by Johannes on 10.09.2020.
//

#ifndef HAR_GUI_ABOUT_HPP
#define HAR_GUI_ABOUT_HPP

#include <gtkmm/aboutdialog.h>

namespace har::gui_ {
    class about : public Gtk::AboutDialog {
    public:
        about();

        ~about() noexcept override;
    };
}

#endif //HAR_GUI_ABOUT_HPP
