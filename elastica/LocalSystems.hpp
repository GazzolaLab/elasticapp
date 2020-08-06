#pragma once
#include <cmath>

#include "elastica/Systems/SystemBase.hpp"

namespace elastica {

  namespace systems {

    template <typename T>
    class SHO final : public SystemBase {
     private:
      T omega_;

     public:
      using ValueType = T;
      // explicit SHO(T omega)
      //     : omega_(omega), kinematic_states(initial_position()),
      //       dynamic_states(initial_velocity()){};
      explicit SHO(T omega)
          : SystemBase(initial_position(), initial_velocity()), omega_(omega){};
      ~SHO() = default;

      template <template <typename> class DerivType, typename StateType>
      DerivType<T> get_initial_derivative(const StateType& st,
                                          const ValueType /*t = 0.0*/) {
        return DerivType<T>{st.get_velocity(),
                            compute_acceleration(st.get_position())};
      }

      double kinematic_rates(double /*time_v*/) noexcept override {
        return dynamic_states;
      };
      double dynamic_rates(double /*time_v*/) noexcept override {
        return compute_acceleration(kinematic_states());
      };

      // T kinematic_rates(double /*time_v*/) {
      //   return compute_acceleration(kinematic_states);
      // };
      // T dynamic_rates(double /*time_v*/) {
      //   return compute_acceleration(kinematic_states);
      // };

      T compute_acceleration(T pos) { return -omega_ * omega_ * pos; }

      template <class StateType, class DerivType>
      void operator()(const StateType& state, DerivType& dxdt, const T /*t*/) {
        T pos = state.get_position();
        dxdt.set_velocity(state.get_velocity());
        dxdt.set_acceleration(compute_acceleration(pos));
      }

      T initial_position() const { return 2.0; }
      T initial_velocity() const { return -2.0 * omega_; };

      std::pair<T, T> analytical(T time) const {
        T amplitude = std::sqrt(std::pow(initial_position(), 2.0) +
                                std::pow(initial_velocity() / omega_, 2));
        T phase = std::acos(initial_position() / amplitude);
        T ap = amplitude * std::cos(omega_ * time + phase);
        T av = -amplitude * omega_ * std::sin(omega_ * time + phase);
        return std::make_pair(ap, av);
      };

      // T kinematic_states, dynamic_states;
    };
    // https://computing.llnl.gov/sites/default/files/public/ark_examples.pdf
    template <typename T>
    struct ARKAnalytic final : public SystemBase {
     public:
      using ValueType = T;
      // explicit ARKAnalytic(ValueType lambda) : lambda_(lambda){};
      explicit ARKAnalytic(ValueType lambda)
          : SystemBase(0.0, 0.0), lambda_(lambda){};
      ~ARKAnalytic() = default;

      // template <template <typename> class DerivType, typename StateType>
      // DerivType<T> get_initial_derivative(const StateType &state,
      //                                     const ValueType t = 0.0) const {
      //   ValueType temp = lambda_ * state.get_position() + 1.0 / (1.0 + t * t)
      //   -
      //                    lambda_ * std::atan(t);
      //   return DerivType<T>{temp, 0.0};
      // }

      // template <class StateType, class DerivType>
      // void operator()(const StateType &state, DerivType &dxdt, const T t)
      // const {
      //   ValueType pos = state.get_position();
      //   ValueType temp =
      //       lambda_ * pos + 1.0 / (1.0 + t * t) - lambda_ * std::atan(t);
      //   dxdt.set_velocity(temp);
      //   dxdt.set_acceleration(0.0);
      // }

      // T kinematic_rates(double time_v) {
      //   ValueType pos = kinematic_states;
      //   dynamic_states = lambda_ * pos + 1.0 / (1.0 + time_v * time_v) -
      //                    lambda_ * std::atan(time_v);
      //   return dynamic_states;
      // };
      // T dynamic_rates(double /*time_v*/) { return 0.0; };

      double kinematic_rates(double time_v) noexcept override {
        double pos = kinematic_states();
        dynamic_states_ = lambda_ * pos + 1.0 / (1.0 + time_v * time_v) -
                         lambda_ * std::atan(time_v);
        return dynamic_states_;
      };
      double dynamic_rates(double /*time_v*/) noexcept override { return 0.0; };

      T initial_position() const { return 0.0; }
      T initial_velocity() const { return 0.0; };

      std::pair<T, T> analytical(T time) const {
        return std::make_pair(std::atan(time), 0.0);
      };

      // ValueType kinematic_states, dynamic_states;
      ValueType lambda_;
    };
  }  // namespace systems

}  // namespace elastica
