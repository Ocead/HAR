//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_ARTIFACT_HPP
#define HAR_ARTIFACT_HPP

#include <har/coords.hpp>

#include "world/cargo_cell_base.hpp"

namespace har {

    class artifact {
    private:
        std::reference_wrapper<cargo_cell_base> _base;
        dcoords_t _section;

    public:
        artifact(cargo_cell_base & base, dcoords_t section);

        artifact(const artifact &) = delete;

        artifact(artifact && fref) noexcept;

        [[nodiscard]]
        cargo_cell_base & base() const;

        [[nodiscard]]
        dcoords_t section() const;

        void move_by(ccoords_t delta);

        void move_by(dcoords_t delta);

        void bend(cargo_cell_base & now);

        artifact & operator=(const artifact &) = delete;

        artifact & operator=(artifact && fref) noexcept;

        ~artifact();
    };

}

#endif //HAR_ARTIFACT_HPP
