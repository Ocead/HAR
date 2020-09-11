//
// Created by Johannes on 03.06.2020.
//

#include <array>
#include <sstream>

#include <har/coords.hpp>
#include <har/types.hpp>

using namespace har::exception;

invalid_direction::invalid_direction(std::string source, direction_t dir) :
        exception(std::move(source), "invalid_direction in " + source + ":\n\t got "),
        _actual(dir) {
    stringstream css{ };
    css << _actual;
    auto str = css.str();
    _message += std::string(str.begin(), str.end());
}

const char * invalid_direction::what() const noexcept {
    return _message.c_str();
}

har::ostream & har::operator<<(har::ostream & os, const har::direction_t & d) {
    switch (d) {
        case direction::UP:
            os << "UP";
            break;
        case direction::DOWN:
            os << "DOWN";
            break;
        case direction::RIGHT:
            os << "RIGHT";
            break;
        case direction::LEFT:
            os << "LEFT";
            break;
        case direction::NONE:
            os << "NONE";
            break;
        default:
            os << "PIN[" << uint_t(d) << "]";
            break;
    }
    return os;
}

har::istream & har::operator>>(har::istream & is, har::direction_t & d) {
    char_t name[5]{ '\0' };
    is.readsome(name, 2);
    string_t s(name, 2);
    if (s == text("UP")) {
        d = direction::UP;
        return is;
    }
    is.readsome(name + 2, 2);
    s = string_t(name, 4);
    if (s == text("NONE")) {
        d = direction::NONE;
    } else if (s == text("DOWN")) {
        d = direction::DOWN;
    } else if (s == text("LEFT")) {
        d = direction::LEFT;
    } else if (s == text("PIN[")) {
        uint_t i;
        char_t c;
        is >> i;
        is.readsome(&c, 1);
        if (c == ']')
            d = direction::PIN[i];
        else
            d = direction::NONE;
        return is;
    } else {
        is.readsome(name + 4, 1);
        s = string_t(name, 5);
        if (s == text("RIGHT"))
            d = direction::RIGHT;
        else
            d = direction::NONE;
    }
    return is;
}
