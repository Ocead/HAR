//
// Created by Johannes on 19.07.2020.
//

#ifndef HAR_GUI_TYPES_HPP
#define HAR_GUI_TYPES_HPP

#include <gdkmm/pixbuf.h>

#include <har/coords.hpp>
#include <har/types.hpp>

namespace har::gui_ {
    using image_out_t = Glib::RefPtr<Gdk::Pixbuf>;

    using image_in_t = std::tuple<Cairo::RefPtr<Cairo::Surface>, har::uint_t>;

    using button_press_t = std::function<bool_t(har::gcoords_t pos, GdkEventButton *)>;

    using button_release_t = std::function<bool_t(har::gcoords_t pos, GdkEventButton *)>;

    using drag_begin_t = std::function<void(har::gcoords_t from,
                                            const Glib::RefPtr<Gdk::DragContext> & context)>;

    using drag_data_get_t = std::function<void(har::gcoords_t from,
                                               const Glib::RefPtr<Gdk::DragContext> & ctx,
                                               Gtk::SelectionData & sel, guint info, guint time)>;

    using drag_data_received_t = std::function<void(har::gcoords_t to,
                                                    const Glib::RefPtr<Gdk::DragContext> & context,
                                                    int x, int y,
                                                    const Gtk::SelectionData & sel,
                                                    guint info, guint time)>;

    using conn_add_t = std::function<void(const har::gcoords_t & from,
                                          const har::gcoords_t & to,
                                          direction_t use)>;

    template<typename T>
    Glib::ustring make_ustring(const std::basic_string<T> & str) {
        if constexpr(std::is_same_v<T, char>) {
            return Glib::ustring(str);
        } else if constexpr(std::is_same_v<T, wchar_t>) {
            auto const * utf16 = reinterpret_cast<gunichar2 const *>(str.c_str());
            gchar * utf8 = g_utf16_to_utf8(utf16, -1, nullptr, nullptr, nullptr);
            Glib::ustring u(utf8);
            g_free(utf8);
            return u;
        }
    }
}

#endif //HAR_GUI_TYPES_HPP
