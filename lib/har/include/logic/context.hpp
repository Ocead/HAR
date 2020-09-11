//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_CONTEXT_HPP
#define HAR_CONTEXT_HPP

#include <deque>
#include <queue>

#include <har/property.hpp>
#include <har/value.hpp>

#include "world/cargo_cell_base.hpp"
#include "world/grid_cell_base.hpp"
#include "world/model.hpp"

namespace har {

    class context {
    private:
        model * _model;

        std::set<cell_h> _changed;
        std::set<cell_h> _redraw;
        std::deque<unresolved_connection> _connected;
        std::deque<unresolved_connection> _disconnected;
        std::deque<cargo_cell_base *> _spawned;
        std::set<cargo_h> _moved;
        std::set<cargo_h> _destroyed;
        std::deque<std::array<string_t, 2>> _messages;

    public:
        static context & invalid();

        context();

        explicit context(model & model);

        void message(const string_t & header, const string_t & content);

        grid_cell_base & at(const gcoords_t & pos);

        cargo_cell_base & at(cargo_h num);

        decltype(_changed) & changed();

        [[nodiscard]]
        const decltype(_changed) & changed() const;

        decltype(_redraw) & redraw();

        [[nodiscard]]
        const decltype(_redraw) & redraw() const;

        decltype(_connected) & connected();

        [[nodiscard]]
        const decltype(_connected) & connected() const;

        decltype(_disconnected) & disconnected();

        [[nodiscard]]
        const decltype(_disconnected) & disconnected() const;

        decltype(_spawned) & spawned();

        [[nodiscard]]
        const decltype(_spawned) & spawned() const;

        decltype(_moved) & moved();

        [[nodiscard]]
        const decltype(_moved) & moved() const;

        decltype(_destroyed) & destroyed();

        [[nodiscard]]
        const decltype(_destroyed) & destroyed() const;

        decltype(_messages) & messages();

        [[nodiscard]]
        const decltype(_messages) & messages() const;

        void change(const cell_h & hnd);

        void draw(const cell_h & hnd);

        void connect(unresolved_connection && conn);

        void disconnect(unresolved_connection && conn);

        cargo_cell_base & spawn(cargo_cell_base && cell);

        void move(cargo_h num);

        void destroy(cargo_h num);

        void reset();

        ~context();
    };

}

#endif //HAR_CONTEXT_HPP
