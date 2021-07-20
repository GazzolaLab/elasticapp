//
// Created by Tejaswin Parthasarathy on 7/20/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/QueryID.hpp"

namespace {

  using namespace ::elastica::interface::detail;
  struct ModuleTag;
  struct TypeTag;
  using Q = QueryID<ModuleTag, TypeTag>;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Wrappers.QueryID.id",
                     "[Unit][Simulator][Interface]") {
    Q key(25UL);
    CHECK(id(key) == 25UL);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Wrappers.QueryID.logical",
                     "[Unit][Simulator][Interface]") {
    Q first_key(25UL);
    Q second_key(25UL);
    Q third_key(30UL);

    CHECK(first_key == second_key);
    CHECK(first_key != third_key);
  }

}  // namespace
