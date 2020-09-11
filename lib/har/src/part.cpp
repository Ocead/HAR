//
// Created by Johannes on 28.05.2020.
//

#include <iomanip>
#include <utility>

#include <har/part.hpp>
#include <har/grid_cell.hpp>

using namespace har;

//region entry

entry::entry(entry_h id, string_t unique_name, string_t friendly_name,
             value && type_and_default, ui_access access, serialize serializable,
             const entry::Specifics & specifics) : id(id),
                                                   unique_name(std::move(unique_name)),
                                                   friendly_name(std::move(friendly_name)),
                                                   type_and_default(std::forward<value>(type_and_default)),
                                                   access(access),
                                                   serializable(serializable),
                                                   specifics(specifics) {
    auto type = type_and_default.type();
    switch (specifics.index()) {
        case 0: {
            if (!(type == value::datatype::BOOLEAN ||
                  type == value::datatype::COLOR ||
                  type == value::datatype::SPECIAL ||
                  type == value::datatype::CALLBACK)) {
                //TODO: Throw datatype specifics mismatch
            }
            break;
        }
        case 1: {
            if (!(type == value::datatype::INTEGER)) {
                //TODO: Throw datatype specifics mismatch
            }
            break;
        }
        case 2: {
            if (!(type == value::datatype::UNSIGNED)) {
                //TODO: Throw datatype specifics mismatch
            }
            break;
        }
        case 3: {
            if (!(type == value::datatype::DOUBLE)) {
                //TODO: Throw datatype specifics mismatch
            }
            break;
        }
        case 4: {
            if (!(type == value::datatype::UNSIGNED)) {
                //TODO: Throw datatype specifics mismatch
            }
            break;
        }
        case 5: {
            if (!(type == value::datatype::DIRECTION)) {
                //TODO: Throw datatype specifics mismatch
            }
            break;
        }
        default: {
            break;
        }
    }
}

string_t entry::to_string(const value & val) const {
    if (type_and_default.type() == value::datatype::UNSIGNED && specifics.index() == 4) {
        return std::get<4>(specifics)->at(get<uint_t>(val));
    } else {
        return string_t(val);
    }
}

value entry::from_string(const string_t & str) const {
    stringstream ss{ str };
    switch (type_and_default.type()) {
        case datatype::VOID: {
            return value();
        }
        case datatype::BOOLEAN: {
            bool_t v;
            ss >> std::boolalpha >> v >> std::noboolalpha;
            return value(v);
        }
        case datatype::INTEGER: {
            int_t v;
            auto & ref = std::get<1>(specifics);
            ss >> v;
            v = std::clamp(v, std::get<0>(ref), std::get<1>(ref));
            return value(v);
        }
        case datatype::UNSIGNED: {
            if (specifics.index() != 4) {
                uint_t v;
                auto & ref = std::get<2>(specifics);
                ss >> v;
                v = std::clamp(v, std::get<0>(ref), std::get<1>(ref));
                return value(v);
            } else {
                auto & ref = *std::get<4>(specifics);
                auto it = std::find_if(ref.begin(), ref.end(), [&str](const auto & p) {
                    return p.second == str;
                });
                if (it != ref.end()) {
                    return value(it->first);
                } else {
                    return value(uint_t());
                }
            }
        }
        case datatype::DOUBLE: {
            double_t v;
            auto & ref = std::get<3>(specifics);
            ss >> v;
            v = std::clamp(v, std::get<0>(ref), std::get<1>(ref));
            return value(v);
        }
        case datatype::STRING: {
            string_t v{ };
            ss >> std::quoted(v, text('\"'), text('\\'));
            return value(v);
        }
        case datatype::C_COORDINATES: {
            ccoords_t v;
            ss >> v;
            return value(v);
        }
        case datatype::D_COORDINATES: {
            dcoords_t v;
            ss >> v;
            return value(v);
        }
        case datatype::DIRECTION: {
            direction_t v;
            ss >> v;
            switch (std::get<5>(specifics)) {
                case dir_cat::CARDINAL_DIRECTIONS: {
                    if (v != direction::UP && v != direction::DOWN && v != direction::RIGHT && v != direction::LEFT) {
                        v = direction::NONE;
                    }
                    break;
                }
                case dir_cat::PIN_DIRECTIONS: {
                    if (v < direction::PIN[0]) {
                        v = direction::NONE;
                    }
                    break;
                }
                default:
                    break;
            }
            return value(v);
        }
        case datatype::COLOR: {
            color_t v;
            ss >> v;
            return value(v);
        }
        case datatype::SPECIAL:
        case datatype::CALLBACK:
        case datatype::HASH:
        default: {
            return value();
        }
    }
}

bool_t entry::is_standard(const value & val) const {
    /*switch (type_and_default.type()) {
        case datatype::VOID: {
            return true;
        }
        case datatype::BOOLEAN: {
            return get<bool_t>(val);
        }
        case datatype::INTEGER: {
            return get<int_t>(val) == int_t() || get<int_t>(val) == std::get<1>(specifics).at(0);
        }
        case datatype::UNSIGNED: {
            if (specifics.index() != 4) {
                return get<uint_t>(val) == uint_t() || get<uint_t>(val) == std::get<2>(specifics).at(0);
            } else {
                return get<uint_t>(val) == std::get<4>(specifics)->begin()->first;
            }
        }
        case datatype::DOUBLE: {
            return get<double_t>(val) == double_t() || get<double_t>(val) == std::get<3>(specifics).at(0);
        }
        case datatype::STRING: {
            return get<string_t>(val).empty();
        }
        case datatype::C_COORDINATES: {
            return get<ccoords_t>(val) == ccoords_t();
        }
        case datatype::D_COORDINATES: {
            return get<dcoords_t>(val) == dcoords_t();
        }
        case datatype::DIRECTION: {
            return get<direction_t>(val) == direction::NONE;
        }
        case datatype::COLOR: {
            return get<color_t>(val) == color_t();
        }
        case datatype::SPECIAL: {
            return true;
        }
        case datatype::CALLBACK: {
            return true;
        }
        case datatype::HASH: {
            return true;
        }
        default: {
            return true;
        }
    }*/
    return val == type_and_default;
}

value entry::standard_value() const {
    /*
    switch (type_and_default.type()) {
        case datatype::VOID: {
            return value();
            break;
        }
        case datatype::BOOLEAN: {
            return value(false);
        }
        case datatype::INTEGER: {
            auto & bounds = std::get<std::array<int_t, 3>>(specifics);
            return value(
                    std::clamp(
                            int_t(),
                            std::get<0>(bounds),
                            std::get<1>(bounds)
                    )
            );
        }
        case datatype::UNSIGNED: {
            if (specifics.index() == 4) {
                auto & vec = *std::get<4>(specifics);
                return value(vec.begin()->first);
            } else {
                auto & bounds = std::get<std::array<uint_t, 3>>(specifics);
                return value(
                        std::clamp(
                                uint_t(),
                                std::get<0>(bounds),
                                std::get<1>(bounds)
                        )
                );
            }
        }
        case datatype::DOUBLE: {
            auto & bounds = std::get<std::array<double_t, 3>>(specifics);
            return value(
                    std::clamp(
                            double_t(),
                            std::get<0>(bounds),
                            std::get<1>(bounds)
                    )
            );
        }
        case datatype::STRING: {
            return value(string_t());
        }
        case datatype::C_COORDINATES: {
            return value(ccoords_t());
        }
        case datatype::D_COORDINATES: {
            return value(dcoords_t());
        }
        case datatype::DIRECTION: {
            return value(direction::NONE);
        }
        case datatype::COLOR: {
            return value(color_t());
        }
        case datatype::SPECIAL: {
            return value(special_t());
        }
        case datatype::CALLBACK: {
            return value(callback_t());
        }
        case datatype::HASH: {
            return value(part_h());
        }
        default: {
            return value();
        }
    }
    */
    return type_and_default;
}

//endregion

//region delegate_error

using namespace har::exception;

delegate_error::delegate_error(std::string source,
                               std::exception & inner) : exception(std::move(source),
                                                                   "delegate_error in " + source + ":\n\t"),
                                                         _inner(inner) {

}

const std::exception & delegate_error::inner() const noexcept {
    return _inner;
}

const char * delegate_error::what() const noexcept {
    return _message.c_str();
}

//endregion

//region part

part & part::invalid() {
    static part ipt{ };
    return ipt;
}

part::part(part_h id,
           string_t & name,
           traits_h traits) : part(id, name, traits, name) {

}

part::part(part_h id,
           string_t unique_name,
           traits_h traits,
           string_t friendly_name) : _id(id),
                                     _unique_name(std::move(unique_name)),
                                     _friendly_name(std::move(friendly_name)),
                                     _traits(traits),
                                     _model(),
                                     _conn_use(),
                                     delegates() {

}

part::part(const part & ref) : _id(ref._id),
                               _unique_name(ref._unique_name),
                               _friendly_name(ref._friendly_name),
                               _traits(ref._traits),
                               _model(ref._model),
                               _conn_use(ref._conn_use),
                               _visual(ref._visual),
                               _waking(ref._waking),
                               delegates(ref.delegates) {

}

part::part(part && fref) noexcept: _id(fref._id),
                                   _unique_name(std::move(fref._unique_name)),
                                   _friendly_name(std::move(fref._friendly_name)),
                                   _traits(fref._traits),
                                   _model(std::move(fref._model)),
                                   _conn_use(std::move(fref._conn_use)),
                                   _visual(std::move(fref._visual)),
                                   _waking(std::move(fref._waking)),
                                   delegates(std::move(fref.delegates)) {

}

part_h part::id() const {
    return _id;
}

const string_t & part::unique_name() const {
    return _unique_name;
}

string_t & part::friendly_name() {
    return _friendly_name;
}

const string_t & part::friendly_name() const {
    return _friendly_name;
}

traits_h part::traits() const {
    return _traits;
}

const entry & part::add_entry(const entry & e) {
    return _model.insert_or_assign(e.id, e).first->second;
}

const entry & part::add_entry(entry && e) {
    return _model.insert_or_assign(e.id, std::forward<entry>(e)).first->second;
}

void part::remove_entry(entry_h id) {
    _model.erase(id);
    _visual.erase(id);
}

const property_model & part::model() const {
    return _model;
}

void part::add_connection_use(direction_t dir, string_t name) {
    _conn_use.insert_or_assign(dir, name);
}

void part::add_connection_uses(std::initializer_list<std::pair<direction_t, string_t>> && uses) {
    for (auto & e : uses) {
        _conn_use.insert_or_assign(std::get<0>(e), std::get<1>(e));
    }
}

void part::remove_connection_use(direction_t use) {
    _conn_use.erase(use);
}

const decltype(part::_conn_use) & part::connection_uses() const {
    return _conn_use;
}

void part::add_visual(entry_h id) {
    _visual.emplace(id);
}

void part::add_visuals(std::initializer_list<entry_h> && ids) {
    for (auto & e : ids) {
        _visual.emplace(e);
    }
}

void part::remove_visual(entry_h id) {
    _visual.erase(id);
}

const decltype(part::_visual) & part::visual() const {
    return _visual;
}

void part::add_waking(entry_h id) {
    _waking.insert(id);
}

void part::add_wakings(std::initializer_list<entry_h> && ids) {
    for (auto & e : ids) {
        _waking.emplace(e);
    }
}

void part::remove_waking(entry_h id) {
    _waking.erase(id);
}

const decltype(part::_waking) & part::waking() const {
    return _waking;
}

void part::init_standard(cell_base & cell) const {
    for (auto & e : _model) {
        cell.set(e.first, e.second.standard_value());
    }
}

void part::init_static(cell & cl) const {
    if (delegates.init_static) {
        TRY_CATCH({
                      delegates.init_static(cl);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::init_static", e));
                  })
    }
}

void part::init_relative(cell & cl) const {
    if (delegates.init_relative) {
        TRY_CATCH({
                      delegates.init_relative(cl);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::init_relative", e));
                  })
    } else {
        init_static(cl);
    }
}

void part::clear(cell & cl) const {
    if (delegates.clear) {
        TRY_CATCH({
                      delegates.clear(cl);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::clear", e));
                  })
    }
}

void part::cycle(cell & cl) const {
    if (delegates.cycle) {
        TRY_CATCH({
                      delegates.cycle(cl);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::cycle", e));
                  })
    }
}

void part::move(cell & cl) const {
    if (delegates.move) {
        TRY_CATCH({
                      delegates.move(cl);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::move", e));
                  })
    }
}

void part::draw(cell & cl, image_t & im) const {
    if (delegates.draw) {
        TRY_CATCH({
                      delegates.draw(cl, im);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::draw", e));
                  })
    }
}

void part::press(cell & cl, const ccoords_t & pos) const {
    if (delegates.press) {
        TRY_CATCH({
                      delegates.press(cl, pos);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::press", e));
                  })
    }
}

void part::release(cell & cl, const ccoords_t & pos) const {
    if (delegates.release) {
        TRY_CATCH({
                      delegates.release(cl, pos);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::release", e));
                  })
    }
}

void part::regulate(cell & cl, of id, const property & prop) const {
    if (delegates.regulate) {
        TRY_CATCH({
                      delegates.regulate(cl, id, prop);
                  }, (std::exception & e), {
                      raise(delegate_error("har::part::regulate", e));
                  });
    }
}

bool_t part::operator==(const part & ref) const {
    return _id == ref._id;
}

bool_t part::operator!=(const part & ref) const {
    return _id != ref._id;
}

bool_t part::operator<(const part & ref) const {
    return _id < ref._id;
}

bool_t part::operator<=(const part & ref) const {
    return _id <= ref._id;
}

bool_t part::operator>(const part & ref) const {
    return _id > ref._id;
}

bool_t part::operator>=(const part & ref) const {
    return _id >= ref._id;
}

part & part::operator=(const part & ref) = default;

part & part::operator=(part && fref) noexcept = default;

part::~part() = default;

//endregion
