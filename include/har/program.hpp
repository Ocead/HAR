//
// Created by Johannes on 04.07.2020.
//

#ifndef HAR_PROGRAM_HPP
#define HAR_PROGRAM_HPP

#include <har/part.hpp>
#include <har/participant.hpp>

namespace har {

    ///
    class program : public participant {
    public:

        /// \brief Default destructor
        program();

        using participant::request;

        using participant::start;

        using participant::step;

        using participant::stop;

        using participant::select;

        using participant::attached;

        using participant::detach;

        using participant::exit;

        [[nodiscard]]
        std::string name() const override;

        [[nodiscard]]
        image_t get_image_base(const cell_h & hnd) override;

        har::image_t process_image(har::cell_h hnd, har::image_t & img) override;

        istream & input() override;

        ostream & output() override;

        void on_cycle(participant::context & ctx) override;

        void on_attach(int argc, char * const * argv, char * const * envp) override;

        void on_part_included(const part & pt) override;

        void on_part_removed(part_h id) override;

        void on_resize_grid(const gcoords_t & to) override;

        void on_model_loaded() override;

        void on_info_updated(const model_info & info) override;

        void on_run() override;

        void on_step() override;

        void on_stop() override;

        void on_message(const string_t & header, const string_t & content) override;

        void on_exception(const exception::exception & e) override;

        void on_selection_update(const cell_h & hnd, entry_h id, const value & val) override;

        void on_redraw(const cell_h & hnd, image_t && img) override;

        void on_connection_added(const gcoords_t & from, const gcoords_t & to, direction_t use) override;

        void on_connection_removed(const gcoords_t & from, direction_t use) override;

        void on_cargo_spawned(cargo_h num) override;

        void on_cargo_moved(cargo_h num, ccoords_t to) override;

        void on_cargo_destroyed(cargo_h num) override;

        void on_detach() override;

        ~program() noexcept override;
    };
}

#endif //HAR_PROGRAM_HPP
