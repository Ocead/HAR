//
// Created by Johannes on 04.07.2020.
//

#include <iomanip>

#include "world/model.hpp"
#include "logic/inner_simulation.hpp"

using namespace har;

//region model_format_error

exception::model_format_error::model_format_error(const std::string & source,
                                                  const std::string & line) : exception(source,
                                                                                        "model_format_error in " +
                                                                                        source +
                                                                                        ":\n\t faulty line is \"" +
                                                                                        line +
                                                                                        "\""),
                                                                              _line(line) {

}

exception::model_format_error::~model_format_error() noexcept = default;

//endregion

//region model

model::model(const inner_simulation & sim) : world(),
                                             _sim(sim),
                                             _info() {

}

model::model(const model & ref) : world(static_cast<const world &>(ref)),
                                  _sim(ref._sim),
                                  _info(ref._info) {

}

model::model(model && fref) noexcept: world(std::forward<world>(fref)),
                                      _sim(fref._sim),
                                      _info(std::move(fref._info)) {
    _info.titles.insert_or_assign(grid_t::MODEL_GRID, get_model().title());
    _info.titles.insert_or_assign(grid_t::BANK_GRID, get_bank().title());
}

string_t & model::title() {
    return _info.title;
}

const string_t & model::title() const {
    return _info.title;
}

string_t & model::author() {
    return _info.author;
}

const string_t & model::author() const {
    return _info.author;
}

string_t & model::version() {
    return _info.version;
}

const string_t & model::version() const {
    return _info.version;
}

string_t & model::description() {
    return _info.description;
}

const string_t & model::description() const {
    return _info.description;
}

bool_t & model::editable() {
    return _info.editable;
}

const bool_t & model::editable() const {
    return _info.editable;
}

model_info & model::info() {
    return _info;
}

const model_info & model::info() const {
    return _info;
}

model & model::operator=(const model & ref) = default;

model & model::operator=(model && fref) noexcept {
    if (this != &fref) {
        this->~model();
        new(this) model(std::forward<model>(fref));
    }
    return *this;
}

model::~model() = default;

ostream & har::operator<<(ostream & os, const model & ref) {
    os << string_t(text("simulation\n")) +
          text("version 2") +
          text("\nname ") << std::quoted(ref._info.title, text('\"'), text('\\')) <<
       text("\nauthor ") << std::quoted(ref._info.author, text('\"'), text('\\')) <<
       text("\ndescription ") << std::quoted(ref._info.description, text('\"'), text('\\')) <<
       text("\nlocked ") << std::boolalpha << ref._info.editable << std::noboolalpha
       << text("\n")
       << static_cast<const world &>(ref);
    return os;
}

istream & har::operator>>(istream & is, model & model) {
    bool_t ok;
    return is >> std::tie(model, ok);
}

istream & har::operator>>(istream & is, std::tuple<model &, bool_t &> model_ok) {
    //TODO: OK flag for correct models
    auto &[model, ok] = model_ok;
    string_t line;

    ok = true;
    std::getline(is >> std::ws, line, text('\n'));
    remove_r(line);
    if (line == text("simulation")) {
        std::getline(is >> std::ws, line, text('\n'));
        remove_r(line);
        if (line == text("version 2")) {
            std::getline(is >> std::ws, line, text(' '));
            remove_r(line);
            if (line == text("name")) {
                is >> std::ws >> std::quoted(model._info.title, text('\"'), text('\\'));
            } else {
                std::string eline{ line.begin(), line.end() };
                raise(*new exception::model_format_error("har::operator>>", eline));
            }
            std::getline(is >> std::ws, line, text(' '));
            remove_r(line);
            if (line == text("author")) {
                is >> std::ws >> std::quoted(model._info.author, text('\"'), text('\\'));
            } else {
                std::string eline{ line.begin(), line.end() };
                raise(*new exception::model_format_error("har::operator>>", eline));
            }
            std::getline(is >> std::ws, line, text(' '));
            remove_r(line);
            if (line == text("description")) {
                is >> std::ws >> std::quoted(model._info.description, text('\"'), text('\\'));
            } else {
                std::string eline{ line.begin(), line.end() };
                raise(*new exception::model_format_error("har::operator>>", eline));
            }
            std::getline(is >> std::ws, line, text('\n'));
            remove_r(line);
            if (line.substr(0, 7) == text("locked ")) {
                model._info.editable = line.substr(7) == text("true");
            } else {
                std::string eline{ line.begin(), line.end() };
                raise(*new exception::model_format_error("har::operator>>", eline));
            }
            std::getline(is, line, text('\n'));
            remove_r(line);
            std::tie(is, model._sim.get().inventory()) >> std::tie(static_cast<world &>(model), ok);
            model._info.titles.try_emplace(grid_t::MODEL_GRID, model.get_model().title());
            model._info.titles.try_emplace(grid_t::BANK_GRID, model.get_bank().title());
        } else {
            std::string eline{ line.begin(), line.end() };
            raise(*new exception::model_format_error("har::operator>>", eline));
        }
    } else {
        std::string eline{ line.begin(), line.end() };
        raise(*new exception::model_format_error("har::operator>>", eline));
    }
    return is;
}

//endregion
