//
// Created by Tejaswin Parthasarathy on 7/7/21.
//

#include "Time/Protocols.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers.hpp"
//
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  // [symplectic_steppable_eg]
  struct MyTimeIntegrator
      : public ::tt::ConformsTo<
            ::elastica::timestepper::protocols::SymplecticSteppable> {
    MyTimeIntegrator() = default;

    template <typename SymplecticAlgorithm>
    void step(::elastica::timestepper::SymplecticStepper<
                  SymplecticAlgorithm> const& /*time_stepper*/,
              ::elastica::Time& time_v,
              ::elastica::TimeDelta const dt) const noexcept {
      // This system only integrates the time variable!
      time_v += dt;
    }
  };
  // [symplectic_steppable_eg]

  static_assert(::tt::assert_conforms_to<
                    MyTimeIntegrator,
                    ::elastica::timestepper::protocols::SymplecticSteppable>,
                "Not a valid symplectic stepper!");

}  // namespace