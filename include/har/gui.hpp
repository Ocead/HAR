//
// Created by Johannes on 28.06.2020.
//

#ifndef HAR_GUI_GUI_HPP
#define HAR_GUI_GUI_HPP

#include <thread>

#include <gtkmm/application.h>

#include <har/co_queue.hpp>
#include <har/participant.hpp>

namespace har {

    namespace gui_ {
        class main_win;
    }

    class gui : public har::participant {
    private:
        std::thread _thread;
        Glib::RefPtr<Gtk::Application> _app;
        gui_::main_win * _mwin;
        std::atomic<bool_t> _responsible;
        std::optional<sigc::connection> _connection;
        std::chrono::microseconds _cycle_delta;

        co_queue<std::function<void()>> _queue;

        void set_cycle(std::chrono::microseconds delta);

    public:
        explicit gui();

        [[nodiscard]]
        std::string name() const override;

        [[nodiscard]]
        har::image_t get_image_base(const cell_h & pos) override;

        [[nodiscard]]
        har::image_t process_image(har::cell_h hnd, har::image_t & img) override;

        istream & input() override;

        ostream & output() override;

        void on_cycle(participant::context & ctx) override;

        void on_attach(int argc, char * const * argv, char * const * envp) override;

        void on_part_included(const har::part & pt, bool_t commit) override;

        void on_part_removed(part_h id) override;

        void on_resize_grid(const gcoords_t & to) override;

        void on_model_loaded() override;

        void on_info_updated(const model_info & info) override;

        void on_run(bool_t responsible) override;

        void on_step() override;

        void on_stop() override;

        void on_message(const string_t & header, const string_t & content) override;

        void on_exception(const har::exception::exception & e) override;

        void on_selection_update(const cell_h & hnd, entry_h id, const value & val, bool_t commit) override;

        void on_redraw(const cell_h & hnd, har::image_t && img, bool_t commit) override;

        void on_connection_added(const gcoords_t & from, const gcoords_t & to, direction_t use) override;

        void on_connection_removed(const gcoords_t & from, direction_t use) override;

        void on_cargo_spawned(cargo_h num) override;

        void on_cargo_moved(cargo_h num, ccoords_t to) override;

        void on_cargo_destroyed(cargo_h num) override;

        void on_commit() override;

        void on_detach() override;

        ~gui() noexcept override;

        friend class gui_::main_win;
    };
}

#endif //HAR_GUI_GUI_HPP
