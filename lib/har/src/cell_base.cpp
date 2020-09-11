//
// Created by Johannes on 09.06.2020.
//

#include <har/cell_base.hpp>

using namespace har;

//region cell_format_error

exception::cell_format_error::cell_format_error(const std::string & source,
                                                const std::string & line) : exception(source,
                                                                                      "cell_format_error in " +
                                                                                      source +
                                                                                      ":\n\t faulty line is \"" +
                                                                                      line +
                                                                                      "\""),
                                                                            _line(line) {

}

exception::cell_format_error::~cell_format_error() noexcept = default;

//endregion

//region cell_base

cell_base & cell_base::invalid() {
    static cell_base iclb{ part::invalid() };
    return iclb;
}

cell_base::cell_base(const part & pt) : _logic(pt),
                                        _properties(),
                                        _intermediate() {
    _logic.get().init_standard(*this);
    transit();

}

cell_base::cell_base(const cell_base & ref) : _logic(ref._logic),
                                              _properties(ref._properties),
                                              _intermediate(ref._intermediate) {

}

cell_base::cell_base(cell_base && fref) noexcept: _logic(fref._logic),
                                                  _properties(std::forward<Map>(fref._properties)),
                                                  _intermediate(std::forward<Map>(fref._intermediate)) {

}

const part & cell_base::logic() const {
    return _logic;
}

const cell_base::Map & cell_base::properties() const {
    return _properties;
}

const cell_base::Map & cell_base::intermediate() const {
    return _intermediate;
}

void cell_base::set_type(const part & pt) {
    _logic = pt;
}

const value & cell_base::get(of id, bool_t now) const {
    if (now) {
        auto it = _intermediate.find(id);
        if (it != _intermediate.end()) {
            return it->second;
        }
    }
    debug({
       if (auto it = _properties.find(id); it != _properties.end()) {
           return it->second;
       } else {
           debug_log("cell has no property with ID " + value::to_string(id) + " (" + std::to_string(id) + ")");
           return value::invalid();
       }
    });
    return _properties.at(id);
}

void cell_base::set(of id, const value & val) noexcept {
    _intermediate.insert_or_assign(id, val);
}

void cell_base::set(of id, value && val) noexcept {
    _intermediate.insert_or_assign(id, std::forward<value>(val));
}

void cell_base::rollback() {
    _intermediate.clear();
}

void cell_base::clear() {
    _properties.clear();
    rollback();
}

void cell_base::transit() {
    for (auto & e : _intermediate) {
        _properties.insert_or_assign(e.first, e.second);
    }
    rollback();
}

bool_t cell_base::adopt(const cell_base & cell) {
    for (auto & p : cell.properties()) {
        set(p.first, p.second);
    }
    return !cell.properties().empty();
}

bool_t cell_base::adopt(cell_base && cell) {
    bool_t empty = !cell.properties().empty();
    std::swap(_intermediate, cell._properties);
    _properties.merge(cell._properties);
    return empty;
}

bool_t cell_base::operator==(const cell_base & rhs) const {
    return _properties == rhs._properties && _intermediate == rhs._intermediate;
}

cell_base & cell_base::operator=(const cell_base & ref) = default;

cell_base & cell_base::operator=(cell_base && fref) noexcept = default;

cell_base::~cell_base() noexcept = default;

ostream & har::operator<<(ostream & os, const cell_base & cell) {
    os << string_t(text("part ")) + cell.logic().unique_name() + text('\n');
    std::map<entry_h, string_t> lines;
    for (auto & p : cell.properties()) {
        string_t line{ text("prop ") };
        const auto & model = cell.logic().model();
        auto eit = model.find(p.first);
        if (eit != model.end()) {
            const entry & ent = eit->second;
            if (ent.serializable == serialize::ANYWAY || (ent.serializable == serialize::SERIALIZE && !ent.is_standard(p.second))) {
                if (p.first < of::NEXT_FREE) {
                    line += value::to_string(p.first);
                } else {
                    line += ent.unique_name;
                }
                line += text(" ") + ent.to_string(p.second) + text('\n');
                lines.emplace(p.first, line);
            }
        }
    }
    for (auto & l : lines) {
        os << l.second;
    }
    return os;
}

std::tuple<istream &, const std::map<part_h, part> &>
har::operator>>(std::tuple<istream &, const std::map<part_h, part> &> is_inv, cell_base & cell) {
    auto & is = std::get<0>(is_inv);
    auto & inv = std::get<1>(is_inv);
    if (is.peek() == 'p') {
        string_t part_line;
        std::getline(is >> std::ws, part_line, text('\n'));
        remove_r(part_line);
        if (part_line.substr(0, 4) == text("part")) {
            string_t part_name = part_line.substr(5);
            auto pit = std::find_if(inv.begin(), inv.end(), [&part_name](const auto & e) {
                return e.second.unique_name() == part_name;
            });
            if (pit != inv.end()) {
                const part & pt = pit->second;
                const auto & model = pt.model();
                cell._logic = pt;
                pt.init_standard(cell);

                while (is.peek() == text('p')) {
                    string_t prop_line;
                    std::getline(is, prop_line, text('\n'));
                    remove_r(prop_line);

                    stringstream ss{ prop_line };
                    string_t prop;
                    std::getline(ss >> std::ws, prop, text(' '));
                    remove_r(prop);
                    if (prop == text("prop")) {
                        std::getline(ss >> std::ws, prop, text(' '));
                        remove_r(prop);
                        of of = value::from_string(prop);
                        const entry * ent;
                        if (of == of::VOID) {
                            auto eit = std::find_if(model.begin(), model.end(), [&prop](const auto & e) {
                                return e.second.unique_name == prop;
                            });
                            ent = &eit->second;
                        } else {
                            auto eit = model.find(of);
                            if (eit != model.end()) {
                                ent = &eit->second;
                            } else {
                                std::string eline{ prop_line.begin(), prop_line.end() };
                                raise(*new exception::cell_format_error("har::operator>>", eline));
                            }
                        }
                        if (ent) {
                            std::getline(ss >> std::ws, prop, text('\n'));
                            remove_r(prop);
                            cell.set(ent->id, ent->from_string(prop));
                        } else {
                            std::string eline{ prop_line.begin(), prop_line.end() };
                            raise(*new exception::cell_format_error("har::operator>>", eline));
                        }
                    } else {
                        std::string eline{ prop_line.begin(), prop_line.end() };
                        raise(*new exception::cell_format_error("har::operator>>", eline));
                    }
                }
            } else {
                std::string eline{ part_line.begin(), part_line.end() };
                raise(*new exception::cell_format_error("har::operator>>", eline));
            }
        } else {
            std::string eline{ part_line.begin(), part_line.end() };
            raise(*new exception::cell_format_error("har::operator>>", eline));
        }

    }

    return is_inv;
}

//endregion cell_base
