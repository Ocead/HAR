//
// Created by Johannes on 19.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::producer(part_h offset) {
    part pt{ PART[standard_ids::PRODUCER + offset],
             text("har:producer"),
             traits::COMPONENT_PART | traits::INPUT | traits::MOVING | traits::ORIENTABLE,
             text("Producer") };

    return pt;
}
