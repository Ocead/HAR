//
// Created by Johannes on 19.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::destructor(part_h offset) {
    part pt{ PART[standard_ids::DESTRUCTOR + offset],
             text("har:destructor"),
             traits::COMPONENT_PART | traits::INPUT | traits::MOVING,
             text("Destructor") };

    return pt;
}
