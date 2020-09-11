//
// Created by Johannes on 18.07.2020.
//

#include <har/duino.hpp>
#include "parts.hpp"

using namespace har;
using namespace har::parts;

part duino::parts::thread_rod(part_h offset) {
    part pt{ PART[standard_ids::THREAD_ROD + offset],
             text("har:thread_rod"),
             traits::COMPONENT_PART | traits::MOVING | traits::ORIENTABLE,
                  text("Thread rod") };

    return pt;
}
