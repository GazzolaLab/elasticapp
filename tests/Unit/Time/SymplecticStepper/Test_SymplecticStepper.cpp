//
// Created by Tejaswin Parthasarathy on 6/1/21.
//
///
#include "Framework/TestingFramework.hpp"
///
#include <string>

#include "Time/AnalyticalSystems.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace elastica::timestepper;
  using namespace elastica::timestepper::symplectic;
  using elastica::Time;
  using elastica::TimeDelta;

  using SymplecticAlgorithms = tmpl::list<PositionVerlet>;
  using TestSystems =
      tmpl::list<testing::SymplecticSHO<testing::CustomDouble>,
                 testing::SymplecticDuffingLike<testing::CustomDouble> >;

  ELASTICA_TEST_CASE("Unit.Time.SymplecticSteppers.SymplecticStepper",
                     "[Time][Unit]") {
    // check default initialization
    tmpl::for_each<SymplecticAlgorithms>([](auto algo) {
      using Algorithm = tmpl::type_from<decltype(algo)>;
      const SymplecticStepper<Algorithm> time_stepper{};
      ignore_unused(time_stepper);
    });
  }

  ELASTICA_TEST_CASE("Unit.Time.SymplecticSteppers.apply_substeps_of",
                     "[Time][Unit]") {
    std::string base;
    auto first = [](std::string& s) noexcept -> void { s += "Hai"; };
    auto second = [](std::string& s) noexcept -> void { s += "Bai"; };
    struct Algorithm {
      using SubSteps = tmpl::list<decltype(first), decltype(second)>;
    };

    // check default initialization
    const SymplecticStepper<Algorithm> time_stepper(
        std::make_tuple(std::move(first), std::move(second)));

    apply_substeps_of(time_stepper,
                      [&base](auto const& update_step) { update_step(base); });

    CHECK(base == "HaiBai");
  }

  // check if all the factors for the algorithms are rightly implemented
  ELASTICA_TEST_CASE("Unit.Time.SymplecticSteppers.temporal_factors",
                     "[Time][Unit]") {
    // check default initialization
    tmpl::for_each<SymplecticAlgorithms>([](auto algo) {
      using Algorithm = tmpl::type_from<decltype(algo)>;
      const SymplecticStepper<Algorithm> time_stepper{};
      Time time_v(1.0);
      TimeDelta dt(1.0);

      apply_substeps_of(time_stepper, [&, dt = dt](auto const& update) {
        // now can be only update_step(system)
        // but need to dispatch based on time-only update
        //
        dispatch_based_on(
            update, [](...) {},
            [&](auto const& time_update) { time_v += time_update(dt); });
      });
      CHECK(cpp17::as_const(time_v).value() == approx(2.0));
    });
  }

  ELASTICA_TEST_CASE("Unit.Time.SymplecticSteppers.Algorithms.PositionVerlet",
                     "[Time][Unit]") {
    // check default initialization
    tmpl::for_each<TestSystems>([](auto sys) {
      using System = tmpl::type_from<decltype(sys)>;
      using Algorithm = PositionVerlet;
      const SymplecticStepper<Algorithm> time_stepper{};
      Time time_v(0.0);
      Time final_time_v(1.0);
      System system;
      TimeDelta dt(system.timestep_guess());

      while (time_v <= final_time_v) {
        apply_substeps_of(time_stepper, [&, dt = dt](auto const& update) {
          dispatch_based_on(
              update,
              [&, &time_v = cpp17::as_const(time_v),
               dt = dt](auto const& system_update) {
                system_update(system, time_v, dt);
              },
              [&, dt = dt](auto const& time_update) {
                time_v += time_update(dt);
              });
        });
      }

      CHECK(cpp17::as_const(time_v) >= approx(cpp17::as_const(final_time_v)));

      typename System::Type::ValueType analytical_primitive,
          analytical_derivative;
      std::tie(analytical_primitive, analytical_derivative) =
          system.analytical(time_v);
      CHECK(system.kinematic_states() == approx(analytical_primitive));
      CHECK(system.dynamic_states() == approx(analytical_derivative));
    });
  }

}  // namespace