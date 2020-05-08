#pragma once
#include <cstddef>
#include <utility>
#include <vector>

// #include "SymplecticStepperKernel.hpp"
namespace timestepper {

namespace symplectic {

template <class SymplecticKernel> class SystemStepper {
public:
  SymplecticKernel stepper;
  using StepperCategoryTag = typename SymplecticKernel::StepperCategoryTag;
  static constexpr std::size_t n_steps() {
    return decltype(stepper)::n_steps();
  }
  static constexpr std::size_t nfeval_per_step() {
    return decltype(stepper)::nfeval_per_step();
  }

private:
  template <std::size_t CurrentStep, class System>
  void do_step_impl_single_pass(System &system, double time_v, double dt) {
    stepper.template kinematic_step<CurrentStep>(system, time_v, dt);
    time_v += stepper.template kinematic_prefactor<CurrentStep>() * dt;
    stepper.template dynamic_step<CurrentStep>(system, time_v, dt);
  }

  template <typename System, std::size_t... Steps>
  void do_step_impl(System &system, double time_v, double dt,
                    std::index_sequence<Steps...>) {
    using expander = int[];

    (void)expander{
        0, ((void)do_step_impl_single_pass<Steps>(system, time_v, dt), 0)...};
  }

public:
  SystemStepper() = default;
  ~SystemStepper() = default;

  template <typename System>
  inline void do_step(System &system, double time_v, double dt) {
    do_step_impl(system, time_v, dt, std::make_index_sequence<n_steps()>());
  }
};
} // namespace symplectic
} // namespace timestepper
