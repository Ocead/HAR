//
// Created by Johannes on 28.08.2020.
//

#include <filesystem>
#include <fstream>

#include <har/duino.hpp>

extern unsigned char uno_ham[];
extern unsigned int uno_ham_len;

using namespace har;

duino::duino() = default;

full_grid_cell duino::map_cell_digital(context & ctx, uint8_t pin) {
    if (pin <= 7u) {
        return ctx.at(gcoords_t(grid_t::BANK_GRID, 8, 20 - pin));
    } else if (pin <= 13u) {
        return ctx.at(gcoords_t(grid_t::BANK_GRID, 8, 19 - pin));
    } else {
        raise(std::runtime_error(std::string("Illegal digital pin number ") + std::to_string(pin)));
    }
}

full_grid_cell duino::map_cell_analog(context & ctx, uint8_t pin) {
    if (pin - 14 <= 5) {
        return ctx.at({ grid_t::BANK_GRID, dcoords_t(0, 15 + pin - 14) });
    } else {
        raise(std::runtime_error(std::string("Illegal analog pin number ") + std::to_string(pin - 14)));
    }
}

full_grid_cell duino::map_cell_interrupt(context & ctx, uint8_t num) {
    switch (num) {
        case 0u:
            return ctx.at(gcoords_t{ grid_t::BANK_GRID, 8, 18 });
        case 1u:
            return ctx.at(gcoords_t{ grid_t::BANK_GRID, 8, 17 });
        default:
            raise(std::runtime_error(std::string("Illegal interrupt number ") + std::to_string(num)));
    }
}

void duino::on_attach(int argc, char * const * argv, char * const * envp) {
    bool_t loaded{ false };
    for (auto i = 1; i < argc; ++i) {
        std::string_view model_option{ argv[i - 1] };
        if (model_option == "-m" || model_option == "--model") {
            std::string_view model_path_view{ argv[i] };
            string_t model_path{ model_path_view.begin(), model_path_view.end() };
            ifstream ifs{ std::filesystem::path(model_path) };
            load_model(ifs);
            loaded = true;
        }
    }
    if (!loaded) {
        std::string model{ reinterpret_cast<const char *>(uno_ham), uno_ham_len };
        load_model(model);
    }
}

void duino::on_model_loaded() {
    program::on_model_loaded();
    _start = clock::now();
    debug_log("Calling setup()");
    setup();
}

duino::context duino::request_or_terminate() {
    if (!attached()) {
        std::terminate();
    } else {
        return program::request();
    }
}

const decltype(duino::_start) & duino::start() const {
    return _start;
}

int duino::digitalRead(uint8_t pin) {
    auto ctx = request_or_terminate();
    auto fgcl = map_cell_digital(ctx, pin);
    return double_t(fgcl[of::POWERED_PIN]) > .01;
}

void duino::digitalWrite(uint8_t pin, uint8_t val) {
    auto ctx = request_or_terminate();
    auto fgcl = map_cell_digital(ctx, pin);
    if (fgcl.has(of::PWM_DUTY)) {
        fgcl[of::PWM_DUTY] = double_t(val ? 1. : 0.);
    } else {
        fgcl[of::POWERING_PIN] = fgcl[val ? of::HIGH_VOLTAGE : of::LOW_VOLTAGE];
    };
}

void duino::pinMode(uint8_t pin, uint8_t mode) {
    auto ctx = request_or_terminate();
    auto fgcl = map_cell_digital(ctx, pin);
    fgcl[of::PIN_MODE] = uint_t(mode);
}

int duino::analogRead(uint8_t pin) {
    auto ctx = request_or_terminate();
    auto fgcl = map_cell_analog(ctx, pin);
    return int(double_t(fgcl[of::POWERED_PIN]) / double_t(fgcl[of::HIGH_VOLTAGE]) * 1023.);
}

void duino::analogReference(uint8_t type) {
    double ref;
    switch (type) {
        case 0: {
            ref = 5.;
            break;
        }
        case 2u: {
            ref = 1.1;
            break;
        }
        case 1: {
            auto ctx = request_or_terminate();
            auto fgcl = ctx.at(gcoords_t(grid_t::BANK_GRID, 8, 4));
            ref = double_t(fgcl[of::POWERED_PIN]);
            break;
        }
        default: {
            debug_log("Mode " + std::to_string(type) + " is not supported");
            raise(std::runtime_error(""));
        }
    }
    auto ctx = request_or_terminate();
    for (uint8_t i = 0; i <= 5.; ++i) {
        map_cell_analog(ctx, i)[of::HIGH_VOLTAGE] = double_t(ref);
    }
}

void duino::analogWrite(uint8_t pin, int val) {
    auto ctx = request_or_terminate();
    auto fgcl = map_cell_digital(ctx, pin);
    fgcl[of::PWM_DUTY] = double_t(double(val) / 255.);
}

uint8_t duino::digitalPinToInterrupt(uint8_t pin) {
    switch (pin) {
        case 2u:
            return 0u;
        case 3u:
            return 1u;
        default:
            return 255u;
    }
}

void duino::attachInterrupt(uint8_t interruptNum, void (* userFunc)(), int mode) {
    //TODO: Implement
}

void duino::detachInterrupt(uint8_t interruptNum) {
    //TODO: Implement
}

duino::~duino() noexcept = default;
