//
// Created by Tejaswin Parthasarathy on 7/7/22.
//

#pragma once

#include <random>
#include <sstream>
#include <tuple>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Custom/Timer.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/Timing.h"

// equality of timers via == is meaningless, so we just check all values
namespace elastica {

  namespace timing {

    template <typename TP>
    bool operator==(Timer<TP> const& first, Timer<TP> const& second) {
      return first.total() == approx(second.total()) &&
             first.counter() == second.counter() &&
             first.average() == approx(second.average()) &&
             first.min() == approx(second.min()) &&
             first.max() == approx(second.max()) &&
             first.last() == approx(second.last());
    };

  }  // namespace timing

}  // namespace elastica

template <typename IArchive, typename OArchive>
struct TestTimer {
  static inline void run() {
    tmpl::for_each<tmpl::list<::elastica::timing::WcPolicy,
                              ::elastica::timing::CpuPolicy>>([](auto p) {
      using Policy = tmpl::type_from<decltype(p)>;
      run<Policy>();
    });
  }

  template <typename Policy>
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    using type = ::elastica::timing::Timer<Policy>;

    for (int ii = 0; ii < 10; ++ii) {
      type o_timer{};
      o_timer.get_total() = random_value<double>(gen);
      o_timer.get_counter() = random_value<std::size_t>(gen);
      o_timer.get_min() = random_value<double>(gen);
      o_timer.get_max() = random_value<double>(gen);
      o_timer.get_last() = random_value<double>(gen);

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(o_timer);
      }

      type i_timer{};

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_timer);
      }

      CHECK(o_timer == i_timer);
    }
  }
};
