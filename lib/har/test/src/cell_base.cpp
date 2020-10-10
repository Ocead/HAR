//
// Created by Johannes on 08.06.2020.
//

#include <har/coords.hpp>

#include "world/grid_cell_base.hpp"

#include "static_for.hpp"

#include <catch2/catch.hpp>

using namespace har;

template<std::size_t I>
struct variant_type_iterator {
    template<typename T>
    inline void operator()(std::reference_wrapper<T> rclb, of id) {
        T & clb = rclb.get();
        using type = typename value::dtype<I>::outer_type;
        DYNAMIC_SECTION("for datatype " << value::datatype_name(datatype(I))
                                        << " (" << typeid(type).name() << ")") {
            const type t1{ };
            const type t2{ };

            clb.set(id, value(t1));
            clb.transit();

            SECTION("Properties can be written") {
                REQUIRE_NOTHROW(clb.set(id, value(t1)));
                REQUIRE_NOTHROW(clb.set(id + 1, value(t2)));
            }

            SECTION("Properties can be read in intermediate state") {
                clb.set(id + 1, value(t2));

                if constexpr(!std::is_same_v<std::any, type>) {
                    REQUIRE(get<type>(clb.get(id + 1, true)) == t2);
                    REQUIRE_THROWS(get<type>(clb.get(id + 1, false)));
                } else {
                    REQUIRE(get<type>(clb.get(id + 1, true)).type() == t2.type());
                    REQUIRE_THROWS(get<type>(clb.get(id + 1, false)));
                }
            }

            SECTION("Properties can transit") {
                REQUIRE_NOTHROW(clb.transit());
            }

            SECTION("Properties can be read") {
                if constexpr(!std::is_same_v<std::any, type>) {
                    REQUIRE(get<type>(clb.get(id + 0, true)) == t1);
                    REQUIRE(get<type>(clb.get(id + 0, false)) == t1);
                } else {
                    REQUIRE(get<type>(clb.get(id + 0, true)).type() == t1.type());
                    REQUIRE(get<type>(clb.get(id + 0, false)).type() == t1.type());
                }
            }

            debug(SECTION("Non present properties map to invalid values") {
                REQUIRE(&clb.get(id + 1, true) == &value::invalid());
                REQUIRE(&clb.get(id + 1, false) == &value::invalid());
            });

            SECTION("Intermediate changes can be rolled back") {
                clb.set(id + 1, value(t2));

                REQUIRE_NOTHROW(clb.rollback());
                debug(
                        REQUIRE(&clb.get(id + 1, true) == &value::invalid());
                        REQUIRE(&clb.get(id + 1, false) == &value::invalid());
                );
                REQUIRE_NOTHROW(clb.get(id + 0, false));
            }

            SECTION("Cells can be cleared") {
                clb.set(id + 1, value(t2));

                REQUIRE_NOTHROW(clb.clear());
                debug(
                        REQUIRE(&clb.get(id + 0, false) == &value::invalid());
                        REQUIRE(&clb.get(id + 1, true) == &value::invalid());
                );
            }
        }
    }
};

TEST_CASE("Cargo cell bases", "[cell_base][cargo_cell_base]") {
    const part pt{ };
    const of id{ of::VOID };
    cargo_cell_base cclb{ CARGO[0], pt, ccoords_t() };

    SECTION("Property access") {
        static_for<1, std::variant_size<value_base>::value - 1, variant_type_iterator>(std::ref(cclb), id);
    }
}

TEST_CASE("Grid cell bases", "[cell_base][grid_cell_base]") {
    const part pt{ };
    const of id{ of::VOID };
    grid_cell_base gclb{ pt, gcoords_t() };

    SECTION("When moved in memory, cells keep their neighbors") {
        grid_cell_base c1{ pt, gcoords_t(MODEL_GRID, 0, 0) };
        grid_cell_base c2{ pt, gcoords_t(MODEL_GRID, 1, 0) };
        grid_cell_base c3{ pt, gcoords_t(MODEL_GRID, 2, 0) };

        c1.set_neighbor(direction::LEFT, &c2);

        REQUIRE(c1.get_neighbor(direction::LEFT) == &c2);
        REQUIRE(c2.get_neighbor(direction::RIGHT) == &c1);

        c3 = std::move(c1);

        REQUIRE(c3.get_neighbor(direction::LEFT) == &c2);
        REQUIRE(c2.get_neighbor(direction::RIGHT) == &c3);
    }

    SECTION("Property access") {
        static_for<1, std::variant_size<value_base>::value - 1, variant_type_iterator>(std::ref(gclb), id);
    }
}

TEST_CASE("Connections between cell bases", "[grid_cell_base]") {
    part pt{ };
    grid_cell_base gclb1{ pt, gcoords_t(MODEL_GRID, 0, 0) };
    grid_cell_base gclb2{ pt, gcoords_t(BANK_GRID, 0, 0) };
    direction_t use_forw{ direction::PIN[0] };

    SECTION("Cell bases can be connected to each other") {
        REQUIRE_NOTHROW(gclb1.add_connection(use_forw, gclb2));

        REQUIRE(gclb1.connected().size() == 1u);
        auto & forw = *gclb1.connected().find(use_forw);
        REQUIRE(forw.first == use_forw);
        REQUIRE(&forw.second.get() == &gclb2);

        REQUIRE(gclb2.iconnected().size() == 1u);
        REQUIRE(gclb2.iconnected().at(&gclb1) == 1u);
    }

    SECTION("When a grid_cell_base is moved, connections remain intact") {
        REQUIRE_NOTHROW(gclb1.add_connection(use_forw, gclb2));
        grid_cell_base gclb3{ std::move(gclb1) };

        REQUIRE(gclb3.connected().size() == 1u);
        auto & forw = *gclb3.connected().find(use_forw);
        REQUIRE(forw.first == use_forw);
        REQUIRE(&forw.second.get() == &gclb2);

        REQUIRE(gclb2.iconnected().size() == 1u);
        REQUIRE(gclb2.iconnected().at(&gclb3) == 1u);
    }

    SECTION("When a grid_cell_base is deleted, it's and only it's connections are severed") {
        grid_cell_base & gclb3 = *new grid_cell_base(pt);
        REQUIRE_NOTHROW(gclb2.add_connection(direction::PIN[1], gclb1));
        REQUIRE_NOTHROW(gclb2.add_connection(direction::PIN[0], gclb3));

        delete &gclb3;
        REQUIRE(&gclb2.connected().at(direction::PIN[1]).get() == &gclb1);
        REQUIRE(gclb2.connected().size() == 1u);
    }
}

TEST_CASE("Cargo on a grid", "[!mayfail][cargo_cell_base][artifact]") {
    SECTION("New cargo_cell_base can be spawned") {
        FAIL("Not implemented");
    }

    SECTION("Cargo can be moved") {
        FAIL("Not implemented");
    }

    SECTION("Cargo can't be moved over the grid's bounds") {
        FAIL("Not implemented");
    }

    SECTION("Cargo can be removed") {
        FAIL("Not implemented");
    }

    SECTION("When a grid_cell_base is removed, it's cargo_cells are removed as well") {
        FAIL("Not implemented");
    }
}
