//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::proximity_sensor(part_h offset) {
    part pt{ PART[standard_ids::PROXIMITY_SENSOR + offset],
             text("har:proximity_sensor"),
             traits::COMPONENT_PART | traits::SENSOR | traits::OUTPUT | traits::ORIENTABLE,
             text("Proximity sensor") };

    return pt;
}
