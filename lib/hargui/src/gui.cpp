//
// Created by Johannes on 28.06.2020.
//

#include <giomm.h>

#include <har/gui.hpp>

using namespace har;

gui::gui() : har::participant(), _thread(), _app(), _mwin(nullptr), _queue() {

}

std::string gui::name() const {
    return "HAR reference GUI";
}

har::image_t gui::get_image_base(const cell_h & hnd) {
    return _mwin->get_image_base(hnd);
}

har::image_t gui::process_image(har::cell_h hnd, har::image_t & img) {
    return _mwin->process_image(hnd, img);
}

istream & gui::input() {
#if defined(UNICODE)
    return std::wcin;
#else
    return std::cin;
#endif
}

ostream & gui::output()  {
#if defined(UNICODE)
    return std::wcout;
#else
    return std::cout;
#endif
}

void gui::on_cycle(participant::context & ctx) {

}

void gui::on_attach(int argc, char * const * argv, char * const * envp) {
    std::mutex latch;
    latch.lock();
    _thread = std::thread([this, &latch]() {
        Glib::setenv("LANGUAGE", "en_US", true);
        Glib::setenv("LANG", "en_US.UTF8", true);
        Glib::setenv("LC_ALL", "en_US", true);
        Glib::setenv("LC_MESSAGES", "en_US", true);

        auto app = Gtk::Application::create("de.ocead.har.gui", Gio::APPLICATION_FLAGS_NONE);
        gui_::main_win main_win{ *this, _queue };

        _app = app;
        _mwin = &main_win;
        latch.unlock();

        Glib::set_application_name("HAR");
        _app->run(*_mwin);
        _mwin = nullptr;
        if (attached()) {
            exit();
        }
    });
    latch.lock();
}

void gui::on_part_included(const har::part & pt) {
    _queue.push([&win = *_mwin, &pt]() { win.include_part(pt); });
    _mwin->emit();
}

void gui::on_part_removed(part_h id) {
    _queue.push([&win = *_mwin, id]() { win.remove_part(id); });
    _mwin->emit();
}

void gui::on_resize_grid(const har::gcoords_t & to) {
    _queue.push([this, &win = *_mwin, to]() {
        auto ctx = request();
        win.resize_grid(to, ctx);
    });
    _mwin->emit();
}

void gui::on_model_loaded() {
    _queue.push([&win = *_mwin]() {
        win.model_loaded();
    });
    redraw_all();
    _mwin->emit();
}

void gui::on_info_updated(const model_info & info) {
    _queue.push([&win = *_mwin, info]() { win.info_updated(info); });
    redraw_all();
    _mwin->emit();
}

void gui::on_run() {
    _queue.push([&win = *_mwin]() { win.run(); });
    _mwin->emit();
}

void gui::on_step() {
    _queue.push([&win = *_mwin]() { win.step(); });
    _mwin->emit();
}

void gui::on_stop() {
    _queue.push([&win = *_mwin]() { win.stop(); });
    _mwin->emit();
}

void gui::on_message(const string_t & header, const string_t & content) {
    _queue.push([&win = *_mwin, header, content]() { win.message(header, content); });
    _mwin->emit();
}

void gui::on_exception(const har::exception::exception & e) {
    _queue.push([&win = *_mwin, &e]() { win.exception(e); });
    _mwin->emit();
}

void gui::on_selection_update(const cell_h & hnd, entry_h id, const value & val) {
    _queue.push([&win = *_mwin, hnd, id, val]() { win.selection_update(hnd, id, val); });
    _mwin->emit();
}

void gui::on_redraw(const har::cell_h & hnd, har::image_t && img) {
    _queue.push([&win = *_mwin, hnd, img]() {
        auto captured_img{ img };
        win.redraw(hnd, std::forward<har::image_t>(captured_img));
    });
    _mwin->emit();
}

void gui::on_connection_added(const gcoords_t & from, const gcoords_t & to, direction_t use) {
    _queue.push([&win = *_mwin, from, to, use]() {
        win.connection_added(from, to, use);
    });
    _mwin->emit();
}

void gui::on_connection_removed(const gcoords_t & from, direction_t use) {
    _queue.push([&win = *_mwin, from, use]() {
        win.connection_removed(from, use);
    });
    _mwin->emit();
}

void gui::on_cargo_spawned(har::cargo_h num) {
    _queue.push([&win = *_mwin, num]() { win.cargo_spawned(num); });
    _mwin->emit();
}

void gui::on_cargo_moved(har::cargo_h num, har::ccoords_t to) {
    _queue.push([&win = *_mwin, num, to]() { win.cargo_moved(num, to); });
    _mwin->emit();
}

void gui::on_cargo_destroyed(har::cargo_h num) {
    _queue.push([&win = *_mwin, num]() { win.cargo_destroyed(num); });
    _mwin->emit();
}

void gui::on_detach() {
    if (_mwin) {
        std::mutex latch;
        latch.lock();
        _queue.push([this, &latch]() {
            if (_mwin) _mwin->close();
            latch.unlock();
        });
        _mwin->emit();
        latch.lock();
    }
}

gui::~gui() noexcept {
    if (attached()) {
        detach();
    }
    if (_thread.joinable()) {
        _thread.join();
    }
}
