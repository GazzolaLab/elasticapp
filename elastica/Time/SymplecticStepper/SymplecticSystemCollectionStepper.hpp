#pragma once
#include <cstddef>
#include <utility>
#include <vector>

// #include "SymplecticStepperKernel.hpp"

namespace timestepper {

namespace symplectic {

// template <template <std::size_t> class SymplecticStepper, std::size_t Order>
template <class SymplecticKernel> class SystemCollectionStepper {
public:
  SymplecticKernel stepper;
  using StepperCategoryTag = typename SymplecticKernel::StepperCategoryTag;
  static constexpr std::size_t n_steps() {
    return decltype(stepper)::n_steps();
  }

private:
  template <std::size_t CurrentStep, typename SystemCollection>
  void do_step_impl_single_pass(SystemCollection &system_collection,
                                double time_v, double dt) {
    // Is this the best way to do it?
    for (auto &system : system_collection) {
      stepper.template kinematic_step<CurrentStep>(system, time_v, dt);
    }

    time_v += stepper.template kinematic_prefactor<CurrentStep>() * dt;
    // Apply boundary conditions that constrain solely the primitives
    // that is (r, Q)
    system_collection.constrain_primitive_values(time_v);
    // time += stepper.template;

    // Note : There's an inefficiency here.
    // In the last step, there's never a dynamic step applied
    // for any symplectic algorithm for preserving time-symmetry.
    // This implies that we can break the loop at this point and
    // the algorithm will still be the same.
    // So need not do unnecessary work here

    // For lack of a better term, we call it synchronize
    // it applies all external forces/torques, resolves joints
    // resolves contacts, controls etc.
    // system_collection.synchronize(time);

    for (auto &system : system_collection) {
      stepper.template dynamic_step<CurrentStep>(system, time_v, dt);
    }

    system_collection.constrain_primitive_rates(time_v);
  }

  template <typename SystemCollection, std::size_t... Steps>
  void do_step_impl(SystemCollection &system_collection, double time_v,
                    double dt, std::index_sequence<Steps...>) {
    using expander = int[];

    (void)expander{0, ((void)do_step_impl_single_pass<Steps>(system_collection,
                                                             time_v, dt),
                       0)...};
  }

public:
  SystemCollectionStepper() = default;
  ~SystemCollectionStepper() = default;

  template <typename SystemCollection>
  inline void do_step(SystemCollection &system_collection, double time_v,
                      double dt) {
    do_step_impl(system_collection, time_v, dt,
                 std::make_index_sequence<n_steps()>());
  }
};
} // namespace symplectic

} // namespace timestepper
