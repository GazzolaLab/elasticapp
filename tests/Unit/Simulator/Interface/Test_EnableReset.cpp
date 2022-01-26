//
// Created by Tejaswin Parthasarathy on 1/25/22.
//
#include <cstdint>

#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/EnableReset.hpp"

namespace {

  using ::elastica::modules::EnableReset;

  struct Traits {
    using SizeType = std::size_t;
    using TimeType = double;  // doesnt matter here
  };

  class TestReset : public EnableReset<Traits, TestReset> {
    friend EnableReset<Traits, TestReset>;

   protected:
    void reset_modules(Traits::TimeType) { v.clear(); }

   public:
    std::vector<int> v;
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.EnableReset",
      "[Unit][Simulator][Interface]") {
    // clang-format on
    TestReset tr;
    TestReset* sim = &tr;

    sim->v.push_back(1);
    sim->v.push_back(2);
    sim->v.push_back(3);

    CHECK(sim->n_times_reset() == 0UL);
    CHECK(sim->v[1] == 2);  // buffer is not empty

    using TimeType = Traits::TimeType;
    /// [reset_eg]
    sim->v.push_back(4);
    sim->v.push_back(5);
    sim->v.push_back(6);

    TimeType t{2.14};
    sim->reset(t);
    CHECK(sim->v.empty());  // returns true!
    /// [reset_eg]

    CHECK(sim->n_times_reset() == 1UL);

    sim->v.push_back(4);
    sim->v.push_back(5);
    sim->v.push_back(7);
    CHECK(sim->v[2] == 7);  // buffer is not empty

    /// [clear_eg]
    sim->v.push_back(4);
    sim->v.push_back(5);
    sim->v.push_back(7);

    sim->clear();
    CHECK(sim->v.empty());  // returns true!
    /// [clear_eg]

    CHECK(sim->n_times_reset() == 2UL);
  }

  struct TestResetErrorPropogation
      : public EnableReset<Traits, TestResetErrorPropogation> {
   private:
    friend EnableReset<Traits, TestResetErrorPropogation>;

   protected:
    void reset_modules(Traits::TimeType t) {
      throw std::runtime_error("Test error propogation");
    }
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.EnableReset.propogates_error",
      "[Unit][Simulator][Interface]") {
    // clang-format on
    TestResetErrorPropogation a;

    CHECK_THROWS_WITH(a.reset(), Catch::Contains("Test error"));
  }

}  // namespace