//
// Created by Tejaswin Parthasarathy on 4/18/22.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Utilities/Offsets.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.Offsets", "[Unit][Simulator]") {
    using Tags = tmpl::integral_list<int, 1, 2, 3, 4>;
    using O = elastica::Offsets<Tags>;
    O off{};

    using elastica::get_offset;

    CHECK(get_offset<tmpl::integral_constant<int, 1>>(std::as_const(off)) ==
          0UL);
    CHECK(get_offset<tmpl::integral_constant<int, 2>>(std::as_const(off)) ==
          0UL);
    CHECK(get_offset<tmpl::integral_constant<int, 3>>(std::as_const(off)) ==
          0UL);
    CHECK(get_offset<tmpl::integral_constant<int, 4>>(std::as_const(off)) ==
          0UL);

    get_offset<tmpl::integral_constant<int, 1>>(off) = 1UL;
    get_offset<tmpl::integral_constant<int, 2>>(off) = 2UL;
    get_offset<tmpl::integral_constant<int, 3>>(off) = 3UL;
    get_offset<tmpl::integral_constant<int, 4>>(off) = 4UL;

    CHECK(tuples::apply([](auto... x) { return std::make_tuple(x...); },
                        std::as_const(off).data()) ==
          std::make_tuple(1UL, 2UL, 3UL, 4UL));
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.OffsetsMap",
                     "[Unit][Simulator]") {
    using Tags = tmpl::integral_list<int, 1, 2, 3, 4>;
    using O = elastica::OffsetsMap<Tags>;
    O off{};

    using _1 = tmpl::integral_constant<int, 1>;
    using _2 = tmpl::integral_constant<int, 2>;
    using _3 = tmpl::integral_constant<int, 3>;
    using _4 = tmpl::integral_constant<int, 4>;
    _1 x_1{};
    _2 x_2{};
    _3 x_3{};
    _4 x_4{};

    using elastica::get_offset;

    get_offset<_1>(off)[&x_1] = 4UL;
    get_offset<_2>(off)[&x_2] = 3UL;
    get_offset<_3>(off)[&x_3] = 2UL;
    get_offset<_4>(off)[&x_4] = 1UL;

    CHECK(get_offset<_1>(std::as_const(off)).at(&x_1) == 4UL);
    CHECK(get_offset<_2>(std::as_const(off)).at(&x_2) == 3UL);
    CHECK(get_offset<_3>(std::as_const(off)).at(&x_3) == 2UL);
    CHECK(get_offset<_4>(std::as_const(off)).at(&x_4) == 1UL);
  }

}  // namespace
