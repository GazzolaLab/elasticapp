//
// Created by Tejaswin Parthasarathy on 4/21/22.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Utilities/MemoryMeters.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.MemoryMeters",
                     "[Unit][Simulator]") {
    using Tags = tmpl::integral_list<int, 1, 2, 3, 4>;
    using T = elastica::MemoryMeters<Tags>;
    T meters{};

    using elastica::get_meter;

    CHECK(get_meter<tmpl::integral_constant<int, 1>>(std::as_const(meters))
              .getCounter() == 0UL);
    CHECK(get_meter<tmpl::integral_constant<int, 2>>(std::as_const(meters))
              .getCounter() == 0UL);
    CHECK(get_meter<tmpl::integral_constant<int, 3>>(std::as_const(meters))
              .getCounter() == 0UL);
    CHECK(get_meter<tmpl::integral_constant<int, 4>>(std::as_const(meters))
              .getCounter() == 0UL);

    get_meter<tmpl::integral_constant<int, 1>>(meters).start();
    get_meter<tmpl::integral_constant<int, 2>>(meters).start();
    get_meter<tmpl::integral_constant<int, 3>>(meters).start();
    get_meter<tmpl::integral_constant<int, 4>>(meters).start();

    for_each(meters, [](auto& m) { m.stop(); });

    // CHECK_FALSE(
    //     get_meter<tmpl::integral_constant<int, 1>>(std::as_const(meters))
    //         .getCounter() == 0UL);
    // CHECK_FALSE(
    //     get_meter<tmpl::integral_constant<int, 2>>(std::as_const(meters))
    //         .getCounter() == 0UL);
    // CHECK_FALSE(
    //     get_meter<tmpl::integral_constant<int, 3>>(std::as_const(meters))
    //         .getCounter() == 0UL);
    // CHECK_FALSE(
    //     get_meter<tmpl::integral_constant<int, 4>>(std::as_const(meters))
    //         .getCounter() == 0UL);

    CHECK(tuples::apply(
              [](auto&... meter) {
                return std::make_tuple(meter.getCounter()...);
              },
              std::as_const(meters).data()) ==
          std::make_tuple(1UL, 1UL, 1UL, 1UL));
  }

}  // namespace
