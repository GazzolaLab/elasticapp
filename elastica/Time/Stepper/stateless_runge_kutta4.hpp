#pragma once

#include <type_traits>

#include "Time/Stepper/Bases/AlgebraStepperBase.hpp"

/*
 * TODO Make it genereic to support RKn using Butcher Table
 * TODO rvalue references?
 */
template <unsigned short Order, class Value = real, class Time = Value,
          class Algebra, class Operations>
class stateless_runge_kutta : public AlgebraStepperBase<Algebra, Operations> {};

// Favorite fourth order algorithm
template <class Value = real, class Time = Value, class Algebra,
          class Operations>
class stateless_runge_kutta<4, Value, Time, Algebra, Operations>
    : public AlgebraStepperBase<Algebra, Operations> {
  using stepper_base_type = AlgebraStepperBase<Algebra, Operations>;
  using algebra_type = typename stepper_base_type::algebra_type;
  using operations_type = typename stepper_base_type::operations_type;

  /* This should really be part of explicit_base*/
  template <unsigned short US>
  using stage_type = std::integral_constant<unsigned short, US>;

 public:
  template <class State, class Deriv = State, unsigned short StageNumber>
  void do_stage(const State& /*initial_state*/, const Deriv& /*deriv*/,
                Time& /*t*/, State& /*current_state*/, Time /*dt*/,
                stage_type<StageNumber>) {
    /* You should never be here anyway*/
    static_assert(StageNumber < 5, "Bad stage number!");
  };

  // Assume Sys call before, ie. deriv = f(t_n, y_n)
  template <class State, class Deriv = State>
  void do_stage(const State& initial_state, const Deriv& deriv, Time& t,
                State& current_state, Time dt, stage_type<1>) noexcept {
    /*
     *      Prepare for next stage
     *      We now need at the new stage operation
     *      y* = y_n + f(t_n,y_n)*dt/2
     *      t* = t + h/2
     */
    const Time dt_by_2 = dt / static_cast<Value>(2);

    stepper_base_type::m_algebra.for_each3(
        current_state, initial_state, deriv,
        typename operations_type::template scale_sum2<Value, Time>(1.0,
                                                                   dt_by_2));
    t += dt_by_2;
  };

  // Assume Sys call before
  template <class State, class Deriv = State>
  void do_stage(const State& initial_state, const Deriv& deriv, Time& /*t*/,
                State& current_state, Time dt, stage_type<2>) noexcept {
    /*
           Prepare for next stage
           We now need at the new stage (3)
           y* = y_n + h * f(t*, y*)/2
           t* = t + h/2, so leave alone
    */
    const Time dt_by_2 = dt / static_cast<Value>(2);
    stepper_base_type::m_algebra.for_each3(
        current_state, initial_state, deriv,
        typename operations_type::template scale_sum2<Value, Time>(1.0,
                                                                   dt_by_2));
  };

  template <class State, class Deriv = State>
  void do_stage(const State& initial_state, const Deriv& deriv, Time& t,
                State& current_state, Time dt, stage_type<3>) noexcept {
    /*
           Prepare for next stage
           We now need at the new stage (4)
           y* = y_n + h * f(t*, y*)
           t* = t + h
    */
    stepper_base_type::m_algebra.for_each3(
        current_state, initial_state, deriv,
        typename operations_type::template scale_sum2<Value, Time>(1.0,
                                                                   dt));
    t += 0.5 * dt;  // Update from last t, no new calculation
  };

  template <class State, class Deriv = State>
  void do_stage(const State& initial_state, const Deriv& deriv, Time& t,
                State& current_state, Time dt, stage_type<4>) noexcept {
    /*
           Finish all stages
           We now finalize that
           y* = y_n + h * f(t*, y*)
           t* = t + h
    */
    stepper_base_type::m_algebra.for_each3(
        current_state, initial_state, deriv,
        typename operations_type::template scale_sum2<Value, Time>(1.0,
                                                                   dt));
    current_state = initial_state + deriv;
  };

};
