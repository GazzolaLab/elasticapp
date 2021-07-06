//
// Created by Tejaswin Parthasarathy on 6/18/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Connections/ConnecterBase.hpp"
#include "Simulator/Interface/Connections/MockConnects.hpp"

namespace {

  using namespace ::elastica::interface::connections;

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.ConnecterBase.constructor",
      "[Unit][Simulator][Interface][Connections]") {
    {
      using Connecter = ::mock::Connecter;
      Connecter(ConnectionUUID(4, "test"));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.ConnecterBase.id",
                     "[Unit][Simulator][Interface][Connections]") {
    {
      using Connecter = ::mock::Connecter;
      Connecter cb(ConnectionUUID(4, "test"));
      // id&
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
    }

    {
      using Connecter = ::mock::Connecter;
      // id const &
      CHECK(Connecter(ConnectionUUID(4, "test")).id().uuid == 4);
      CHECK(Connecter(ConnectionUUID(4, "test")).id().name == "test");
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.ConnecterBase.apply",
                     "[Unit][Simulator][Interface][Connections]") {
    {
      // clang-format on
      using Connecter = ::mock::Connecter;
      Connecter cb(ConnectionUUID(4, "test"));

      Connecter::TimeType t{};
      CHECK(cb.value() == 2);
      CHECK((cb.apply_forces(t), cb.value()) == 4);
      CHECK((cb.apply_torques(t), cb.value()) == 3);
    }
  }

}  // namespace