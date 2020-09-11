//
// Created by Johannes on 03.06.2020.
//

#include <har/value.hpp>

#include "static_for.hpp"
#include "values.hpp"

#include <catch2/catch.hpp>

using namespace har;

template<std::size_t I>
struct variant_type_iterator {
    void operator()() {
        using type = value::dtype<I>;
        using itype = typename type::inner_type;
        using otype = typename type::outer_type;

        DYNAMIC_SECTION("for datatype " << value::datatype_name(datatype(I)) << " (" << typeid(otype).name() << ")") {
            value val;
            const otype original{ random_value<otype>() };

            DYNAMIC_SECTION("Type is possibly_pointer of type (" << typeid(type).name() << ")") {
                SUCCEED();
            }

            if constexpr (type::is_pointed) {
                DYNAMIC_SECTION("Variant type is unique pointer of type (" << typeid(itype).name() << ")") {
                    STATIC_REQUIRE(std::is_same_v<itype, std::unique_ptr<otype>>);
                }
            } else {
                DYNAMIC_SECTION("Variant type is same as type (" << typeid(itype).name() << ")") {
                    STATIC_REQUIRE(std::is_same_v<itype, otype>);
                }
            }

            SECTION("Can be initialised with variant type") {
                value v{ otype() };
                otype t{ };

                REQUIRE(v.index() == I);

                REQUIRE_NOTHROW(v = t);
                REQUIRE(v.index() == I);

                REQUIRE_NOTHROW(v = std::move(t));
                REQUIRE(v.index() == I);
            }

            SECTION("Can be initialised without knowing underlying type") {
                auto v = make_value<otype>();

                REQUIRE(v.index() == I);

                REQUIRE_NOTHROW(val = random_value<otype>());
                REQUIRE(val.index() == I);
            }

            SECTION("Can be read from without knowing underlying type") {
                otype contained;
                val = original;

                REQUIRE_NOTHROW(contained = get<otype>(val));
                if constexpr (!std::is_same_v<otype, std::any>) {
                    REQUIRE(original == contained);
                } else {
                    REQUIRE(original.type() == contained.type());
                }
            }
        }
    }
};

TEST_CASE("Values", "[har::value]") {
    static_for<1, std::variant_size<value_base>::value - 1, variant_type_iterator>();
}
