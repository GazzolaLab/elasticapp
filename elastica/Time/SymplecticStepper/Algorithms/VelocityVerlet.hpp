#pragma once
#include <cstddef>
#include <iostream>

#include "../SymplecticStepperTags.hpp"

namespace timestepper {

namespace symplectic {

// TODO : Think of copy constructor etc.
struct VelocityVerlet {
  static constexpr auto mirrored_about() {
    return symplectic::MirroredAbout::velocity_step;
  };
  static constexpr std::size_t n_stages() { return 1UL; };

  VelocityVerlet() = default;
  ~VelocityVerlet() = default;

  template <std::size_t CurrentStage,
            typename std::enable_if<(CurrentStage == 0UL), int>::type = 0>
  void kinematic_step(std::shared_ptr<double> &) {
    std::cout << "K : Order 0" << std::endl;
  }
  // Should be TimeType
  // template <std::size_t CurrentStage> double get_prefactor();
  template <std::size_t CurrentStage,
            typename std::enable_if<(CurrentStage == 0UL), int>::type = 0>
  void dynamic_step(std::shared_ptr<double> &) {
    std::cout << "D : Order 0" << std::endl;
  }
};

// template <>
// void VelocityVerlet::kinematic_step<0UL>(std::shared_ptr<double> &k) {
//   std::cout << "K : Order 0" << std::endl;
//   // std::cout << k + 2 << std::endl;
// };

// template <> void VelocityVerlet::get_prefactor<0UL>() {
//   std::cout << "K : Order 0" << std::endl;
// };

// template <> void VelocityVerlet::dynamic_step<0UL>(std::shared_ptr<double>
// &k) {
//   std::cout << "D : Order 0" << std::endl;
//   // std::cout << k - 1 << std::endl;
// };

} // namespace symplectic
} // namespace timestepper
