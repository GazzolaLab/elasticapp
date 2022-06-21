//
// Created by Tejaswin Parthasarathy on 6/24/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Forcing/ForcerBase.hpp"
#include "Simulator/Interface/Forcing/MockForcing.hpp"

namespace {

  using namespace ::elastica::interface::forcing;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcerBase.constructor",
                     "[Unit][Simulator][Interface][Forcing]") {
    EMPTY_TEST();

    {
      using Forcer = ::mock::Forcer;
      Forcer(ForcingUUID(4, "test"));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcerBase.id",
                     "[Unit][Simulator][Interface][Forcing]") {
    {
      using Forcer = ::mock::Forcer;
      Forcer cb(ForcingUUID(4, "test"));
      // id&
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
    }

    {
      using Forcer = ::mock::Forcer;
      // id const &
      CHECK(Forcer(ForcingUUID(4, "test")).id().uuid == 4);
      CHECK(Forcer(ForcingUUID(4, "test")).id().name == "test");
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcerBase.apply",
                     "[Unit][Simulator][Interface][Forcing]") {
    {
      // clang-format on
      using Forcer = ::mock::Forcer;
      Forcer cb(ForcingUUID(4, "test"));

      Forcer::TimeType t{};
      CHECK(cb.value() == 2);
      CHECK((cb.apply_forces(t), cb.value()) == 4);
      CHECK((cb.apply_torques(t), cb.value()) == 3);
    }
  }

}  // namespace
