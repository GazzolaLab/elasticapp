//
// Created by Tejaswin Parthasarathy on 6/1/21.
//

#pragma once

#include <cmath>
#include <utility>

#include "Time/SimulationTime.hpp"

namespace testing {

  namespace detail {

    template <typename T>
    class SHO {
     public:
      using Type = T;
      using ValueType = typename T::ValueType;
      explicit SHO() : omega_(2.0){};
      explicit SHO(T omega) : omega_(omega){};
      ~SHO() = default;

      template <template <typename> class DerivType, typename StateType>
      DerivType<T> get_initial_derivative(const StateType& st,
                                          const ValueType /*t = 0.0*/) {
        return DerivType<T>{st.get_velocity(),
                            compute_acceleration(st.get_position())};
      }

      ValueType compute_acceleration(elastica::Time, ValueType pos,
                                     ValueType) const noexcept {
        return -omega_ * omega_ * pos;
      }

      template <class StateType, class DerivType>
      void operator()(const StateType& state, DerivType& dxdt, const T /*t*/) {
        T pos = state.get_position();
        dxdt.set_velocity(state.get_velocity());
        dxdt.set_acceleration(compute_acceleration(pos));
      }

      ValueType initial_position() const { return 2.0; }
      ValueType initial_velocity() const { return -2.0 * omega_; };
      ValueType timestep_guess() const {
        auto n_samples = ValueType(1000UL);
        return 2.0 * M_PI / omega_ / n_samples;
      };

      std::pair<ValueType, ValueType> analytical(
          elastica::Time const time_v) const {
        ValueType amplitude =
            std::sqrt(std::pow(initial_position(), 2.0) +
                      std::pow(initial_velocity() / omega_, 2));
        ValueType phase = std::acos(initial_position() / amplitude);
        ValueType ap =
            amplitude *
            std::cos(omega_ * static_cast<ValueType>(time_v) + phase);
        ValueType av =
            -amplitude * omega_ *
            std::sin(omega_ * static_cast<ValueType>(time_v) + phase);
        return std::make_pair(ap, av);
      }

     private:
      ValueType omega_;
    };

    // https://asasi.upm.edu.my/upload/dokumen/20191218103653SOMS2019_Book_Chapters_Asasi_-_UPM_Press.pdf#page=169
    template <typename T>
    class DuffingLike {
     public:
      using Type = T;
      using ValueType = typename T::ValueType;
      explicit DuffingLike(){};
      ~DuffingLike() = default;

      template <template <typename> class DerivType, typename StateType>
      DerivType<T> get_initial_derivative(const StateType& state,
                                          const ValueType t = 0.0) const {
        DerivType<T> deriv{0.0, 0.0};
        operator()(state, deriv, t);
        return deriv;
      }

      template <class StateType, class DerivType>
      void operator()(const StateType& state, DerivType& dxdt,
                      const T t) const {
        ValueType pos = state.get_position();
        ValueType vel = state.get_velocity();
        ValueType cos_t = std::cos(t);
        ValueType acc =
            (2.0 * -cos_t * cos_t) * cos_t - pos * (1.0 + pos * vel) - vel;
        dxdt.set_velocity(vel);
        dxdt.set_acceleration(acc);
      }
      ValueType compute_acceleration(elastica::Time const time_v, ValueType pos,
                                     ValueType vel) const {
        ValueType cos_t = std::cos(time_v.value());
        return (2.0 - cos_t * cos_t) * cos_t - pos * (1.0 + pos * vel) - vel;
      }

      ValueType initial_position() const { return 0.0; }
      ValueType initial_velocity() const { return 1.0; };

      std::pair<ValueType, ValueType> analytical(
          elastica::Time const time_v) const {
        return std::make_pair(std::sin(time_v.value()),
                              std::cos(time_v.value()));
      };

      ValueType timestep_guess() const {
        auto n_samples = ValueType(1000UL);
        return 2.0 * M_PI / n_samples;
      };
    };

    template <typename System>
    class SymplecticWrapper : public System {
     public:
      using Type = typename System::Type;
      Type& kinematic_states() noexcept { return primitive; }
      Type kinematic_rates(elastica::Time const) const noexcept {
        return derivative;
      }
      Type& dynamic_states() noexcept { return derivative; }
      Type dynamic_rates(elastica::Time const t) const noexcept {
        double_derivative =
            System::compute_acceleration(t, primitive, derivative);
        return double_derivative;
      }
      template <typename... Args>
      explicit SymplecticWrapper(Args&&... args)
          : System(std::forward<Args>(args)...),
            primitive(System::initial_position()),
            derivative(System::initial_velocity()),
            double_derivative(System::compute_acceleration(
                elastica::Time(0.0), primitive, derivative)){}

     private:
      Type primitive, derivative;
      mutable Type double_derivative;
    };

  }  // namespace detail

  struct CustomDouble {
    // constexpr here for literal types like pi
    using ValueType = double;
    constexpr CustomDouble() : d_(0.0){};
    constexpr CustomDouble(double d) : d_(d){};
    constexpr CustomDouble(CustomDouble const& other) : d_(other.d_){};

    ~CustomDouble() = default;
    // Overloaded assignment
    // no self-assignment
    CustomDouble& operator=(double d) {
      // do the copy
      d_ = d;
      // return the existing object so we can chain this operator
      return *this;
    }
    CustomDouble& operator*=(double d) {
      // do the copy
      d_ *= d;
      // return the existing object so we can chain this operator
      return *this;
    }
    CustomDouble& operator+=(CustomDouble const& other) {
      // do the copy
      d_ += other.d_;
      // return the existing object so we can chain this operator
      return *this;
    }
    CustomDouble& operator+=(double d) {
      // do the copy
      d_ += d;
      // return the existing object so we can chain this operator
      return *this;
    }

    /* explicit*/ operator double() const { return d_; }

    double d_;
  };

  // multiplication with timedetla
  auto operator*(elastica::TimeDelta const td, CustomDouble cd)
      -> CustomDouble {
    cd *= td.value();
    return cd;
  }

  template <typename T>
  class SymplecticSHO : public detail::SymplecticWrapper<detail::SHO<T>> {};
  template <typename T>
  class SymplecticDuffingLike
      : public detail::SymplecticWrapper<detail::DuffingLike<T>> {};

}  // namespace testing