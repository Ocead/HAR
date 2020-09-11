//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::box_cargo(part_h offset) {
    part pt{ PART[standard_ids::BOX_CARGO + offset],
             text("har:box_cargo"),
             traits::CARGO_PART,
             text("Box cargo") };

    return pt;
}
