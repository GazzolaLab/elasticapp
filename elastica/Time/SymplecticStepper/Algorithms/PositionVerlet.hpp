#pragma once
#include <cstddef>
#include <iostream>

#include "../SymplecticStepperTags.hpp"

namespace timestepper {

  namespace symplectic {

    // TODO : Think of copy constructor etc.
    // TODO : Replace TimeTypes etc.
    struct PositionVerlet {
      static constexpr auto mirrored_about() {
        return symplectic::MirroredAbout::velocity_step;
      };
      static constexpr std::size_t n_stages() { return 1UL; };

      PositionVerlet() = default;
      ~PositionVerlet() = default;

      template <std::size_t CurrentStage, typename System,
                typename std::enable_if<(CurrentStage == 0UL), int>::type = 0>
      void kinematic_step(System& sys, double time_v, double dt) {
        //        std::cout << "K : Order 0" << std::endl;
        sys.kinematic_states() += kinematic_prefactor<CurrentStage>() * dt *
                                  sys.kinematic_rates(time_v);
      }

      template <std::size_t CurrentStage,
                typename std::enable_if<(CurrentStage == 0UL), int>::type = 0>
      double kinematic_prefactor() {
        return static_cast<double>(0.5);
      }
      // Should be TimeType
      // template <std::size_t CurrentStage> double get_prefactor();
      template <std::size_t CurrentStage, typename System,
                typename std::enable_if<(CurrentStage == 0UL), int>::type = 0>
      void dynamic_step(System& sys, double time_v, double dt) {
        //        std::cout << "D : Order 0" << std::endl;
        sys.dynamic_states() += dt * sys.dynamic_rates(time_v);
      }
    };

    // template <>
    // void PositionVerlet::kinematic_step<0UL>(std::shared_ptr<double> &k) {
    //   std::cout << "K : Order 0" << std::endl;
    //   // std::cout << k + 2 << std::endl;
    // };

    // template <> void PositionVerlet::get_prefactor<0UL>() {
    //   std::cout << "K : Order 0" << std::endl;
    // };

    // template <> void
    // PositionVerlet::dynamic_step<0UL>(std::shared_ptr<double> &k) {
    //   std::cout << "D : Order 0" << std::endl;
    //   // std::cout << k - 1 << std::endl;
    // };

  }  // namespace symplectic
}  // namespace timestepper
