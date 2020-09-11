//
// Created by Johannes on 05.07.2020.
//

#include <atomic>

#include <har/grid_cell.hpp>
#include <har/part.hpp>

#include "logic/context.hpp"
#include "har/cell_base.hpp"

using namespace har;

void sketch1() {
    part pt{ PART[5], "test_part" };
    entry e{ of::ANALOG_VOLTAGE,
             "ANALOG_VOLTAGE",
             "Analog voltage",
             datatype::DOUBLE,
             ui_access::INVISIBLE,
             serialize::SERIALIZE,
             std::array<double_t, 3>{ -5., 10., .1 }};
    entry f{ of::DIGITAL_VOLTAGE,
             "DIGITAL_VOLTAGE",
             "Digital voltage",
             datatype::UNSIGNED,
             ui_access::INVISIBLE,
             serialize::SERIALIZE,
             std::array<uint_t, 3>{ 0, 5, 1 }};
    entry g{ of::PIN_MODE,
             "PIN_MODE",
             "Pin Mode",
             datatype::UNSIGNED,
             ui_access::INVISIBLE,
             false,
             {
                     { 0, "In" },
                     { 1, "Out" },
                     { 2, "Z" }
             }};

    pt.add_entry(e);
    pt.add_entry(f);
    pt.add_entry(g);
    pt.add_entry(of::NEXT_FREE,
                 "gnampf",
                 "Gnampf",
                 datatype::DIRECTION,
                 ui_access::INVISIBLE,
                 dir_cat::ALL_DIRECTIONS);

    cell_base oclb{ pt };

    pt.init_standard(oclb);

    oclb.set(of::ANALOG_VOLTAGE, value(double_t(5.4)));
    oclb.set(of::PIN_MODE, value(uint_t(1)));
    oclb.set(of::NEXT_FREE, value(direction::PIN[24]));

    oclb.transit();

    std::stringstream ss{ };

    ss << oclb;

    std::string ser{ ss.str() };

    std::cout << ser << std::endl;

    map<part_h, part> inv{{ pt.id(), pt }};

    cell_base ic{ part::invalid() };

    std::tie(ss, inv) >> ic;

    ic.transit();

    std::cout << ic << std::endl;
}

void sketch2() {
    part pt{ PART[5], "test_part" };

    entry e{ of::ANALOG_VOLTAGE,
             "ANALOG_VOLTAGE",
             "Analog voltage",
             datatype::DOUBLE,
             ui_access::INVISIBLE,
             serialize::SERIALIZE,
             std::array<double_t, 3>{ -5., 10., .1 }
    };

    entry f{ of::DIGITAL_VOLTAGE,
             "DIGITAL_VOLTAGE",
             "Digital voltage",
             datatype::UNSIGNED,
             ui_access::INVISIBLE,
             serialize::SERIALIZE,
             std::array<uint_t, 3>{ 0, 5, 1 }
    };

    entry g{ of::PIN_MODE,
             "PIN_MODE",
             "Pin Mode",
             datatype::UNSIGNED,
             ui_access::INVISIBLE,
             false,
             {{ 0, "In" },
              { 1, "Out" },
              { 2, "Z" }}
    };

    entry h{ of::NEXT_FREE,
             "gnampf",
             "Gnampf",
             datatype::DIRECTION,
             ui_access::INVISIBLE,
             dir_cat::ALL_DIRECTIONS
    };

    pt.add_entry(e);
    pt.add_entry(f);
    pt.add_entry(g);
    pt.add_entry(h);

    map<part_h, part> inv{{ pt.id(), pt }};

    grid_cell_base gclb1{ pt, gcoords(grid_t::MODEL_GRID, 0, 0) };
    grid_cell_base gclb2{ pt, gcoords(grid_t::BANK_GRID, 0, 0) };
    grid_cell_base gclb1in{ part::invalid(), gcoords_t() };

    std::queue<unresolved_connection> conns;

    gclb1.set(of::ANALOG_VOLTAGE, value(double_t(5.4)));
    gclb1.set(of::PIN_MODE, value(uint_t(1)));
    gclb1.set(of::NEXT_FREE, value(direction::PIN[24]));

    gclb2.set(of::ANALOG_VOLTAGE, value(double_t(3.2)));
    gclb2.set(of::PIN_MODE, value(uint_t(4)));
    gclb2.set(of::NEXT_FREE, value(direction::RIGHT));

    gclb1.transit();
    gclb2.transit();

    gclb1.add_connection(direction::PIN[4], gclb2);
    gclb1.add_connection(direction::PIN[2], gclb2);

    std::stringstream ss{ };

    ss << gclb1;

    std::string gcs{ ss.str() };

    std::cout << gcs << std::endl;

    std::tie(ss, inv) >> std::tie(gclb1in, conns);

    std::cout << "";
}

int main(int argc, char * argv[]) {
    std::cout << sizeof(cell_h) << std::endl;
    std::cout << alignof(cell_h) << std::endl;
    std::cout << std::boolalpha << std::atomic<cell_h>::is_always_lock_free << std::endl;
    std::cout << sizeof(gcoords_t) << std::endl;
    std::cout << alignof(gcoords_t) << std::endl;
    sketch1();
    sketch2();
}
