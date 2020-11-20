//
// Created by Johannes on 10.06.2020.
//

#include <cassert>

#include <har/grid_cell.hpp>
#include <har/property.hpp>

#include "logic/context.hpp"

using namespace har;

property::property(context & ctx,
                   cell_base & cell,
                   cell_cat cat,
                   of id,
                   datatype type,
                   bool_t now) : _ctx(ctx),
                                 _cell(cell),
                                 _cat(cat),
                                 _id(id),
                                 _type(type),
                                 _now(now) {
    DEBUG {
        if (&cell == &grid_cell_base::invalid() ||
            &cell == &cargo_cell_base::invalid()) {
            raise(std::runtime_error(""));
        }
    }
}

datatype property::type() const {
    return _type;
}

const value & property::val() const {
    return _cell.get(_id, _now);
}

special_t property::any() const {
    auto & val = _cell.get(_id, _now);
    switch (_type) {
        case datatype::VOID:
            return property(_ctx, _cell, _cat, _id, datatype(val.index())).any();
        case datatype::BOOLEAN:
            return std::make_any<bool_t>(get<bool_t>(val));
        case datatype::INTEGER:
            return std::make_any<int_t>(get<int_t>(val));
        case datatype::UNSIGNED:
            return std::make_any<uint_t>(get<uint_t>(val));
        case datatype::DOUBLE:
            return std::make_any<double_t>(get<double_t>(val));
        case datatype::STRING:
            return std::make_any<string_t>(get<string_t>(val));
        case datatype::C_COORDINATES:
            return std::make_any<ccoords_t>(get<ccoords_t>(val));
        case datatype::D_COORDINATES:
            return std::make_any<dcoords_t>(get<dcoords_t>(val));
        case datatype::DIRECTION:
            return std::make_any<direction_t>(get<direction_t>(val));
        case datatype::COLOR:
            return std::make_any<color_t>(get<color_t>(val));
        case datatype::SPECIAL:
            return get<special_t>(val);
        case datatype::CALLBACK:
            return std::make_any<callback_t>(get<callback_t>(val));
        case datatype::HASH:
            return std::make_any<part_h>(get<part_h>(val));
        default:
            return std::any();
    }
}

property & property::operator=(const value & ref) {
    operator=(value(ref));
    return *this;
}


property & property::operator=(value && fref) {
    assert(val().type() == fref.type());
    _cell.set(_id, std::forward<value>(fref));
    if (_cat == cell_cat::GRID_CELL && &_ctx != &context::invalid()) {
        _ctx.change(static_cast<grid_cell_base &>(_cell).position());
    } else {
        _ctx.change(static_cast<cargo_cell_base &>(_cell).id());
    }
    auto & visual = _cell.logic().visual();
    if (visual.find(_id) != visual.end()) {
        if (_cat == cell_cat::GRID_CELL && &_ctx != &context::invalid()) {
            _ctx.draw(static_cast<grid_cell_base &>(_cell).position());
        } else {
            _ctx.draw(static_cast<cargo_cell_base &>(_cell).id());
        }
    }
    return *this;
}

property & property::operator=(const property & ref) {
    operator=(ref.val());

    return *this;
}

property::~property() = default;
