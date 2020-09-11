//
// Created by Johannes on 10.06.2020.
//

#include <har/grid_cell.hpp>

#include "logic/context.hpp"
#include "world/grid_cell_base.hpp"

#include "static_for.hpp"
#include "values.hpp"

#include <catch2/catch.hpp>

using namespace har;

template<std::size_t I>
struct variant_type_iterator {
    inline void operator()(grid_cell_base & clb, context & ctx, of id) {
        using type = typename value::dtype<I>::outer_type;
        DYNAMIC_SECTION("for datatype " << value::datatype_name(datatype(I))
                                        << " (" << typeid(type).name() << ")") {
            const type t1{ random_value<type>() };
            const type t2{ random_value<type>() };

            const size_t size{ 5u };
            const ccoords_t offset{ 0.5, 0.5 };

            std::vector<cargo_cell_base> cclbs;
            std::vector<artifact> artis;
            std::vector<value> values;
            cclbs.reserve(size);
            artis.reserve(size);
            values.reserve(size);

            clb.set(id, value(t1));
            clb.transit();

            grid_cell gcl{ ctx, clb };

            SECTION("Properties can be read from contexts") {
                if constexpr(!std::is_same_v<special_t, type>) {
                    const type t3{ type(gcl[id]) };

                    REQUIRE(t1 == type(gcl[id]));
                    REQUIRE(t1 == (type) gcl[id]);
                    REQUIRE(t1 == t3);
                } else {
                    const type t3{ gcl[id].any() };

                    REQUIRE(std::any_cast<uint_t>(t1) == std::any_cast<uint_t>(gcl[id].any()));
                    REQUIRE(std::any_cast<uint_t>(t1) == std::any_cast<uint_t>(t3));
                }
            }


            SECTION("Properties can be set from contexts") {
                REQUIRE_NOTHROW(gcl[id] = t2);
                clb.transit();
                if constexpr(!std::is_same_v<special_t, type>) {
                    REQUIRE(type(gcl[id]) == t2);
                } else {
                    REQUIRE(std::any_cast<uint_t>(t2) == std::any_cast<uint_t>(gcl[id].any()));
                }
                REQUIRE(!ctx.changed().empty());
                ctx.changed().clear();
            }

            SECTION("Properties of neighboring cells can be read") {
                grid_cell_base ngclb{ clb.logic(), gcoords_t(INVALID_GRID, 0, 0) };
                context nctx{ };
                grid_cell ngcl{ nctx, ngclb };

                for (auto d : direction::cardinal) {
                    clb.set_neighbor(d, &ngclb);
                }

                for (auto d : direction::cardinal) {
                    auto ctxn = ngcl[d];
                    if constexpr(!std::is_same_v<special_t, type>) {
                        REQUIRE(t1 == type(ctxn[id]));
                    } else {
                        REQUIRE(std::any_cast<uint_t>(t1) == std::any_cast<uint_t>(gcl[id].any()));
                    }
                }
            }


            SECTION("Properties of connected cells can be read") {
                grid_cell_base cgclb{ clb.logic(), gcoords_t(INVALID_GRID, 0, 0) };
                direction_t dir{ direction::PIN[0] };

                cgclb.set(id, value(t1));
                cgclb.transit();
                clb.add_connection(dir, cgclb);

                if constexpr(!std::is_same_v<special_t, type>) {
                    REQUIRE(t1 == type(gcl[dir][id]));
                } else {
                    REQUIRE(std::any_cast<uint_t>(t1) == std::any_cast<uint_t>(gcl[dir][id].any()));
                }
            }


            SECTION("Properties of cargo centered over the cell_base can be read") {
                for (std::size_t i = 0u; i < size; ++i) {
                    auto & cclb = cclbs.emplace_back(CARGO[0], clb.logic(), ccoords_t(clb.position().pos) + offset);
                    auto & arti = artis.emplace_back(cclb, dcoords_t());
                    auto & t = values.emplace_back(value(random_value<type>()));

                    cclb.set(id, t);
                    cclb.transit();
                    clb.add_cargo(CARGO[i], std::move(arti));
                }
                REQUIRE(size == gcl.cargo().size());

                //Test range-based access
                std::size_t count = 0;
                for (auto & [num, ccl] : gcl.cargo()) {
                    type t = get<type>(values.at(count));
                    auto prx = ccl[id];
                    REQUIRE(ccl.position() == offset);
                    if constexpr(!std::is_same_v<special_t, type>) {
                        REQUIRE(t == type(prx));
                    } else {
                        REQUIRE(std::any_cast<uint_t>(t) == std::any_cast<uint_t>(prx.any()));
                    }
                    count++;
                }

                REQUIRE(count == size);
            }

            SECTION("Properties of cargo over the cell_base can be set") {
                for (std::size_t i = 0u; i < size; ++i) {
                    auto & cclb = cclbs.emplace_back(CARGO[0], clb.logic(), ccoords_t(clb.position().pos) + offset);
                    auto & arti = artis.emplace_back(cclb, dcoords_t());
                    values.emplace_back(value(random_value<type>()));

                    cclb.set(id, value(type()));
                    cclb.transit();
                    clb.add_cargo(ARTIFACT[i], std::move(arti));
                }
                REQUIRE(size == gcl.cargo().size());

                //Test range-based access
                std::size_t count = 0;
                for (auto & [num, ccl] : gcl.cargo()) {
                    type t = get<type>(values.at(count));
                    REQUIRE_NOTHROW(ccl[id] = t);
                    const type & tr = get<type>(cclbs.at(count).intermediate().at(id));
                    if constexpr(!std::is_same_v<special_t, type>) {
                        REQUIRE(tr == t);
                    } else {
                        REQUIRE(std::any_cast<uint_t>(tr) == std::any_cast<uint_t>(t));
                    }
                    count++;
                }

                REQUIRE(count == size);
                REQUIRE(!ctx.changed().empty());
                ctx.changed().clear();
            }

            SECTION("Properties of cargo not centered over the cell_base can be read") {
                for (std::size_t i = 0u; i < size; ++i) {
                    auto & cclb = cclbs.emplace_back(CARGO[0], clb.logic(), ccoords_t(clb.position().pos) + offset);
                    auto & arti = artis.emplace_back(cclb, dcoords_t());
                    auto & t = values.emplace_back(value(random_value<type>()));

                    cclb.set(id, t);
                    cclb.transit();
                    clb.add_artifact(ARTIFACT[i], std::move(arti));
                }
                REQUIRE(size == gcl.artifacts().size());

                //Test range-based access
                std::size_t count = 0;
                for (auto & [num, ccl] : gcl.artifacts()) {
                    type t = get<type>(values.at(count));
                    auto prx = ccl[id];
                    REQUIRE(ccl.position() == offset);
                    if constexpr(!std::is_same_v<special_t, type>) {
                        REQUIRE(t == type(prx));
                    } else {
                        REQUIRE(std::any_cast<uint_t>(t) == std::any_cast<uint_t>(prx.any()));
                    }
                    count++;
                }

                REQUIRE(count == size);
            }

        }
    }
};

TEST_CASE("Grid cells") {
    part pt{ };
    grid_cell_base clb{ pt, gcoords_t(INVALID_GRID, 0, 0) };
    const of id{ of::NEXT_FREE };
    context ctx{ };
    grid_cell gcl{ ctx, clb };

    SECTION("Property access") {
        static_for<1, std::variant_size<value_base>::value - 1, variant_type_iterator>(std::ref(clb), ctx, id);
    }

    const size_t size{ 5u };
    const ccoords_t offset{ 0.5, 0.5 };

    std::vector<cargo_cell_base> cclbs;
    std::vector<artifact> artis;
    cclbs.reserve(size);
    artis.reserve(size);

    SECTION("Cargos can be spawned from contexts") {
        cargo_cell ccl = gcl.spawn(gcl.logic(), offset);

        REQUIRE(ctx.spawned().size() == 1u);

        cargo_cell_base & cclb = *ctx.spawned().at(0);

        REQUIRE(cclb.position() == offset);
        REQUIRE(cclb.position() == ccoords_t(clb.position().pos) + offset);
    }

    SECTION("Cargos can be moved from contexts") {
        for (std::size_t i = 0u; i < size; ++i) {
            auto & ccell = cclbs.emplace_back(CARGO[i], clb.logic(), ccoords_t(clb.position().pos) + offset);
            auto & arti = artis.emplace_back(ccell, dcoords_t());

            clb.add_cargo(CARGO[i], std::move(arti));
        }
        REQUIRE(size == gcl.cargo().size());

        REQUIRE_NOTHROW(gcl.cargo(CARGO[2]).move(offset / 2.));
        REQUIRE(ctx.moved().size() == 1);
        REQUIRE(*ctx.moved().begin() == CARGO[2]);
        REQUIRE(clb.cargo().at(CARGO[2]).base().move_delta() == offset / 2.);
    }

    SECTION("Cargos can be destroyed from contexts") {
        for (std::size_t i = 0u; i < size; ++i) {
            auto & ccell = cclbs.emplace_back(CARGO[i], clb.logic(), ccoords_t(clb.position().pos) + offset);
            auto & arti = artis.emplace_back(ccell, dcoords_t());

            clb.add_cargo(CARGO[i], std::move(arti));
        }
        REQUIRE(size == gcl.cargo().size());

        REQUIRE_NOTHROW(gcl.cargo(CARGO[2]).destroy());
        REQUIRE(ctx.destroyed().size() == 1);
        REQUIRE(*ctx.destroyed().begin() == CARGO[2]);
    }
}
