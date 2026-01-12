//
// Created by Tejaswin Parthasarathy on 4/21/22.
//

#include "Framework/TestHelpers.hpp"  // serialize and deserialize
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/Serialization/detail/Test_Timer.hpp"  // for equality operator
#include "Simulator/Utilities/Serialize.hpp"
#include "Simulator/Utilities/Timers.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  // needed for testing serialization
  template <typename Tags, typename TP>
  bool operator==(Timers<Tags, TP> const& first,
                  Timers<Tags, TP> const& second) {
    return first.data() == second.data();
  };

}  // namespace elastica

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.Timers", "[Unit][Simulator]") {
    using Tags = tmpl::integral_list<int, 1, 2, 3, 4>;
    using T = elastica::Timers<Tags>;
    T meters{};

    using elastica::for_each;
    using elastica::get_meter;

    CHECK(get_meter<tmpl::integral_constant<int, 1>>(std::as_const(meters))
              .get_counter() == 0UL);
    CHECK(get_meter<tmpl::integral_constant<int, 2>>(std::as_const(meters))
              .get_counter() == 0UL);
    CHECK(get_meter<tmpl::integral_constant<int, 3>>(std::as_const(meters))
              .get_counter() == 0UL);
    CHECK(get_meter<tmpl::integral_constant<int, 4>>(std::as_const(meters))
              .get_counter() == 0UL);

    get_meter<tmpl::integral_constant<int, 1>>(meters).start();
    get_meter<tmpl::integral_constant<int, 2>>(meters).start();
    get_meter<tmpl::integral_constant<int, 3>>(meters).start();
    get_meter<tmpl::integral_constant<int, 4>>(meters).start();

    for_each(meters, [](auto& m) { m.stop(); });

    CHECK(tuples::apply(
              [](auto&... meter) {
                return std::make_tuple(meter.get_counter()...);
              },
              std::as_const(meters).data()) ==
          std::make_tuple(1UL, 1UL, 1UL, 1UL));
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.Timers.stream",
                     "[Unit][Simulator]") {
    // Creating a new wall clock timer
    using Tags = tmpl::integral_list<int, 1, 2, 3, 4>;
    using T = elastica::Timers<Tags>;
    T timers{};

    std::stringstream ss;
    ss << timers;
    INFO(ss.str());

    //    auto count_occurrences = [](std::string const& src,
    //                                std::string const& target) -> size_t {
    //      std::size_t occurrences = 0UL;
    //      std::string::size_type pos = 0;
    //      while ((pos = src.find(target, pos)) != std::string::npos) {
    //        ++occurrences;
    //        pos += target.length();
    //      }
    //      return occurrences;
    //    };

    // Each words must appear size times
    using namespace std::string_literals;
    for (auto const& substr :
         {"total"s, "counter"s, "average"s, "min"s, "max"s}) {
      INFO(substr);
      CHECK(ss.str().find(substr) != std::string::npos);
      // CHECK(count_occurrences(ss.str(), substr) == timers.size());
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.Timers.serialize",
                     "[Unit][Simulator]") {
    // Creating a new wall clock timer
    using Tags = tmpl::integral_list<int, 1, 2, 3, 4>;
    using T = elastica::Timers<Tags>;
    T timers{};

    // tests only the binary archive code-path, other archives are implicitly
    // tested since backend tests are done elsewhere.
    test_serialization(timers);
  }

}  // namespace