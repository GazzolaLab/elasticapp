#pragma once
#include <tuple>
#include <type_traits>
#include <utility>

#include "StepperTags.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

// template <template <typename, typename> class TimeStepper, typename StT,
//     typename DT>
// auto make_stepper() {
//   return TimeStepper<StT, DT>();
// };

namespace timestepper {

  namespace integrate_detail {
    // Convenience tags to provide same interface to integrate
    struct ConstantTimeStepperTag {};
    struct AdaptiveTimeStepperTag {};
  }  // namespace integrate_detail

  namespace traits {
    // Can be better but whatever
    // DONE : Put in separate TimeStepper traits class
    // TODO : Rewrite using contained_in and conjunction
    // TODO : Macro these things for maintainability
    template <typename T>
    struct IsExplicitTimeStepper
        : std::integral_constant<bool,
                                 std::is_same<typename T::StepperCategoryTag,
                                              ExplicitTag>::value> {};

    template <typename T>
    struct IsExplicitFSALTimeStepper
        : std::integral_constant<bool,
                                 std::is_same<typename T::StepperCategoryTag,
                                              ExplicitFSALTag>::value> {};

    template <typename T>
    struct IsSymplecticTimeStepper
        : std::integral_constant<bool,
                                 std::is_same<typename T::StepperCategoryTag,
                                              SymplecticTag>::value> {};

    // constant or variable time-stepper?
    template <bool B>
    struct TimeStepperStepSizeStrategyTrait {
      using type = integrate_detail::AdaptiveTimeStepperTag;
    };

    template <>
    struct TimeStepperStepSizeStrategyTrait<true> {
      using type = integrate_detail::ConstantTimeStepperTag;
    };
  }  // namespace traits

  template <typename T>
  using is_symplectic_timestepper_t =
      typename traits::IsSymplecticTimeStepper<T>::type;

  template <typename T>
  using timestepper_step_size_strategy_t =
      typename traits::TimeStepperStepSizeStrategyTrait<std::disjunction_v<
          traits::IsExplicitTimeStepper<T>::value,
          traits::IsExplicitFSALTimeStepper<T>::value,
          traits::IsSymplecticTimeStepper<T>::value>>::type;

  namespace integrate_detail {

    using IntegrateDetailExplicitTimeStepperTag = std::false_type;
    using IntegrateDetailSymplecticTimeStepperTag = std::true_type;

    template <typename ConstantTimeStepper, typename System, typename State,
              class... Callables, std::size_t... Is>
    auto integrate(ConstantTimeStepper& time_stepper, System& rod,
                   State /*&state*/, double max_simulation_time, double dt_,
                   std::tuple<Callables...> callables, ConstantTimeStepperTag,
                   IntegrateDetailSymplecticTimeStepperTag,
                   std::index_sequence<Is...>) {
      double time_v(0.0);
      std::size_t nfevals(0UL);

      // Call all simulation level callables at t = 0.0
      using expander = int[];
      (void)expander{0, (std::get<Is>(callables)(time_v, dt_, rod), 0)...};

      do {
        time_stepper.do_step(rod, time_v, dt_);
        time_v += dt_;
        nfevals += time_stepper.nfeval_per_step();

        // call all the callables...
        (void)expander{0, (std::get<Is>(callables)(time_v, dt_, rod), 0)...};

      } while (time_v <= max_simulation_time);

      return std::make_tuple(nfevals, time_v, dt_);
    }

    template <typename ConstantTimeStepper, typename System, typename State,
              class... Callables, std::size_t... Is>
    auto integrate(ConstantTimeStepper& time_stepper, System& rod, State& state,
                   double max_simulation_time, double dt_,
                   std::tuple<Callables...> callables, ConstantTimeStepperTag,
                   IntegrateDetailExplicitTimeStepperTag,
                   std::index_sequence<Is...>) {
      double time_v(0.0);
      std::size_t nfevals(0UL);

      // Call all simulation level callables at t = 0.0
      using expander = int[];
      (void)expander{0, (std::get<Is>(callables)(time_v, dt_, state), 0)...};

      do {
        time_stepper.do_step(rod, state, time_v, dt_);
        time_v += dt_;
        nfevals += time_stepper.nfeval_per_step();

        // call all the callables...
        (void)expander{0, (std::get<Is>(callables)(time_v, dt_, state), 0)...};

      } while (time_v <= max_simulation_time);

      return std::make_tuple(nfevals, time_v, dt_);
    }

    template <typename ConstantTimeStepper, typename System, typename State,
              class... Callables, std::size_t... Is>
    auto integrate(ConstantTimeStepper& time_stepper, System& rod, State& state,
                   double max_simulation_time, double dt_,
                   std::tuple<Callables...> callables, ConstantTimeStepperTag,
                   std::index_sequence<Is...> is) {
      return integrate(time_stepper, rod, state, max_simulation_time, dt_,
                       callables, ConstantTimeStepperTag{},
                       is_symplectic_timestepper_t<ConstantTimeStepper>{}, is);
    }

    enum controlled_step_result { pass, fail };

    template <typename AdaptiveTimeStepper, typename System, typename State,
              class... Callables, std::size_t... Is>
    auto integrate(AdaptiveTimeStepper& time_stepper, System& rod, State& state,
                   double max_simulation_time, double dt_,
                   std::tuple<Callables...> callables, AdaptiveTimeStepperTag,
                   std::index_sequence<Is...>) {
      double time_v(0.0);
      // Call all callables at t = 0.0
      using expander = int[];
      (void)expander{0, (std::get<Is>(callables)(time_v, dt_, state), 0)...};

      do {
        controlled_step_result res(fail);
        do {
          res = time_stepper.try_step(rod, state, time_v, dt_);
        } while (res == fail);
        // call all the callables...
        (void)expander{0, (std::get<Is>(callables)(time_v, dt_, state), 0)...};
      } while (time_v <= max_simulation_time);

      return std::make_tuple(time_stepper.n_evaluations(), time_v, dt_);
    }
  }  // namespace integrate_detail

  template <typename TimeStepper, typename System, typename State,
            class... Callables>
  auto integrate(TimeStepper& time_stepper, System& rod, State& state,
                 double max_simulation_time, double dt_,
                 std::tuple<Callables...> callables = {}) {
    return integrate_detail::integrate(
        time_stepper, rod, state, max_simulation_time, dt_, callables,
        timestepper_step_size_strategy_t<TimeStepper>{},
        std::make_index_sequence<sizeof...(Callables)>{});
  }

}  // namespace timestepper
