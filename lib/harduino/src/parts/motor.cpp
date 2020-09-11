//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::motor(part_h offset) {
    part pt{ PART[standard_ids::MOTOR + offset],
             text("har:motor"),
             traits::COMPONENT_PART | traits::INPUT | traits::MOVING | traits::ORIENTABLE,
             text("Motor") };

    return pt;
}
