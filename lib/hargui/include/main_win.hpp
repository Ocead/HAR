//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_MAIN_WIN_HPP
#define HAR_GUI_MAIN_WIN_HPP

#include <gtkmm/window.h>
#include <glibmm/dispatcher.h>

#include <har/co_queue.hpp>
#include <har/types.hpp>

#include "har/gui.hpp"
#include "action_bar.hpp"
#include "connection_popover.hpp"
#include "grid.hpp"
#include "headerbar.hpp"
#include "properties.hpp"
#include "terminal.hpp"
#include "types.hpp"

namespace har {
    class gui;
}

namespace har::gui_ {

    class main_win : public Gtk::Window {
    private:
        std::reference_wrapper<har::gui> _parti;
        Glib::Dispatcher _dispatcher;
        std::reference_wrapper<har::co_queue<std::function<void()>>> _queue;

        headerbar _headerbar;
        grid _model;
        grid _bank;
        action_bar _action_bar;
        terminal _terminal;
        properties _properties;
        connection_popover _conn_popover;

        std::optional<std::reference_wrapper<const har::part>> _empty_model_part;
        std::optional<std::reference_wrapper<const har::part>> _empty_bank_part;


        cell_h _selected;
        cell_h _pressed;
        Glib::RefPtr<Gdk::Pixbuf> _selected_img;
        std::atomic<std::uint16_t> _updating;

        std::string _path;
        std::string _last_serialized;

        model_info _model_info;

        std::deque<std::pair<std::string, std::function<void()>>> _undo_queue;
        std::deque<std::pair<std::string, std::function<void()>>> _redo_queue;

        void bind();

        void btn_new_clicked();

        void btn_open_clicked();

        bool btn_save_clicked();

        bool btn_save_as_clicked();

        void btn_reset_clicked();

        void prop_changed(of id, value && val);

        void cell_placed(const gcoords_t & pos, const har::part & pt, participant::context & ctx);

        void cell_clicked(const gcoords_t & pos, const ccoords_t & at, participant::context & ctx);

        void cell_selected(const gcoords_t & pos, participant::context & ctx);

        void cell_released(const gcoords_t & pos, const ccoords_t & at, participant::context & ctx);

        void cell_moved(const gcoords_t & from, const gcoords_t & to, participant::context & ctx);

        void cell_connected(const gcoords_t & from, const gcoords_t & to, participant::context & ctx);

        void cell_disconnected(const gcoords_t & pos, direction_t use);

        void cell_cycle(const gcoords_t & pos, participant::context & ctx);

        void draw(const cell_h & hnd, participant::context & ctx);

        Glib::RefPtr<const Gdk::Pixbuf> get_cell_image(const gcoords_t & pos);

        void dispatch();

    protected:
        bool on_key_release_event(GdkEventKey * key_event) override;

        bool on_delete_event(GdkEventAny * any_event) override;

    public:
        explicit main_win(har::gui & parti, har::co_queue<std::function<void()>> & queue);

        [[nodiscard]]
        const cell_h & get_selected() const;

        har::image_t process_image(har::cell_h hnd, har::image_t & img);

        void set_grid_size(const gcoords_t & to);

        image_out_t get_image_base(const cell_h & hnd);

        void include_part(const har::part & pt);

        void remove_part(part_h id);

        void resize_grid(const gcoords_t & pos);

        void model_loaded();

        void info_updated(const model_info & info);

        void run(bool_t responsible);

        void step();

        void stop();

        void message(const string_t & header, const string_t & content);

        void exception(const har::exception::exception & e);

        void selection_update(cell_h hnd, entry_h id, const value & val);

        void redraw(const cell_h & hnd, har::image_t && img);

        void connection_added(const gcoords_t & from, const gcoords_t & to, direction_t use);

        void connection_removed(const gcoords_t & from, direction_t use);

        void cargo_spawned(cargo_h num);

        void cargo_moved(cargo_h num, const ccoords_t & to);

        void cargo_destroyed(cargo_h num);

        void emit();

        ~main_win() noexcept override;
    };
}

#endif //HAR_GUI_MAIN_WIN_HPP
