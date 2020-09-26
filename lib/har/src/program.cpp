//
// Created by Johannes on 04.07.2020.
//

#include <har/program.hpp>

using namespace har;

program::program() = default;

std::string program::name() const {
    return std::string("Generic Program");
}

image_t program::get_image_base(const cell_h & hnd) {
    return std::any();
}

har::image_t program::process_image(har::cell_h hnd, har::image_t & img) {
    return img;
}

istream & program::input() {
#if defined(UNICODE)
    return std::wcin;
#else
    return std::cin;
#endif
}

ostream & program::output()  {
#if defined(UNICODE)
    return std::wcout;
#else
    return std::cout;
#endif
}

void program::on_cycle(participant::context & ctx) {

}

void program::on_attach(int argc, char * const * argv, char * const * envp) {

}

void program::on_part_included(const class part & pt, bool_t commit) {

}

void program::on_part_removed(part_h id) {

}

void program::on_resize_grid(const gcoords_t & to) {

}

void program::on_model_loaded() {

}

void program::on_info_updated(const model_info & info) {

}

void program::on_run() {

}

void program::on_step() {

}

void program::on_stop() {

}

void program::on_message(const string_t & header, const string_t & content) {
#if defined(UNICODE)
    std::wclog
#else
    std::clog
#endif
            << header + text(": ") + content + text("\n");
}

void program::on_exception(const exception::exception & e) {
    std::cerr << e.what() << "\n";
}

void program::on_selection_update(const cell_h & hnd, entry_h id, const value & val, bool_t commit) {

}

void program::on_redraw(const cell_h & hnd, image_t && img, bool_t commit) {

}

void program::on_connection_added(const gcoords_t & from, const gcoords_t & to, direction_t use) {

}

void program::on_connection_removed(const gcoords_t & from, direction_t use) {

}

void program::on_cargo_spawned(cargo_h num) {

}

void program::on_cargo_moved(cargo_h num, ccoords_t to) {

}

void program::on_cargo_destroyed(cargo_h num) {

}

void program::on_commit() {

}

void program::on_detach() {

}

program::~program() noexcept {

}
