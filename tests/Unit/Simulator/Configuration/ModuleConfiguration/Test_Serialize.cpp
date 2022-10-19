//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <sstream>
#include <tuple>

#include "ErrorHandling/NestedExceptions.hpp"
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Serialize.hpp"
#include "Simulator/Configuration/ModuleConfiguration/common/Serialize.hpp"
#include "Simulator/Configuration/ModuleConfiguration/common/common.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/RepeatAsTuple.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
  "Unit.Simulator.Configuration.ModuleConfiguration.Serialize",
  "[Unit][Simulator][Configuration][Serialization]") {
    // clang-format on
    using namespace elastica::io;
    using namespace elastica::configuration;

    SECTION("Non-composites") {
      std::stringstream ss;  // any stream can be used

      using Block = int;
      using single_size_types =
          tmpl::list<RestrictSize<Block>, LooselyRestrictSize<Block>,
                     SerialBelow<Block>, AllSerialBelow,
                     AcrossBlockTypes<RestrictSize>,
                     AcrossBlockTypes<LooselyRestrictSize>,
                     AcrossBlockTypes<AlwaysNew>, AcrossBlockTypes<AlwaysSame>>;

      using Tup = tmpl::as_tuple<single_size_types>;
      constexpr std::size_t N = tmpl::size<single_size_types>::value;
      Tup saved_t(repeat_as_tuple<N>(50UL));

      BinaryOutputArchive oarchive(ss);  //  Create an output archive
      oarchive(saved_t);

      BinaryInputArchive iarchive(ss);  //  Create an output archive
      Tup badly_loaded_t(repeat_as_tuple<N>(2UL));

      // without this
      CHECK_THROWS_NESTED_WITH(iarchive(badly_loaded_t), "Loading tuple failed",
                               "Value from the configuration");

      //      Tup loaded_t(repeat_as_tuple<N>(50UL));
      Tup loaded_t(badly_loaded_t);

      index_apply<N>([&](auto... Is) {
        EXPAND_PACK_LEFT_TO_RIGHT(std::get<Is>(saved_t).value() ==
                                  std::get<Is>(loaded_t).value());
      });
    }
    SECTION("Composites") {
      using FirstBlock = int;
      using SecondBlock = char;
      using ThirdBlock = float;

      using F = LooselyRestrictSize<FirstBlock>;
      using S = LooselyRestrictSize<SecondBlock>;
      using T = RestrictSize<ThirdBlock>;
      auto criterion = CompositeCriteria<F, S, T>(F{100UL}, S{20UL}, T{30UL});
      auto bad_criterion =
          CompositeCriteria<F, S, T>(F{50UL}, S{10UL}, T{40UL});

      std::stringstream ss;  // any stream can be used

      BinaryOutputArchive oarchive(ss);  //  Create an output archive
      oarchive(criterion);

      BinaryInputArchive iarchive(ss);  //  Create an output archive

      // without this
      CHECK_THROWS_NESTED_WITH(iarchive(bad_criterion),
                               "Value from the configuration");
    }
  }

}  // namespace