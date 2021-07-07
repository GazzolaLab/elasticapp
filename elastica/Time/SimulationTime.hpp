#pragma once

#include <limits>
#include <ostream>

#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Time/Types.hpp"
#include "Utilities/Approx.hpp"

namespace elastica {

  namespace detail {

    template <typename Real>
    class SimulationTimeDelta;

    template <typename Real>
    class SimulationTime {
     private:
      using TimeDelta = SimulationTimeDelta<Real>;
      using This = SimulationTime<Real>;

     public:
      SimulationTime() noexcept : value_(0.0){};
      explicit SimulationTime(Real restarted_time) noexcept
          : value_(restarted_time){};
      inline constexpr auto value() const& noexcept -> Real { return value_; };
      explicit inline constexpr operator Real() const noexcept {
        return value();
      }

      SimulationTime& operator+=(const TimeDelta& delta) noexcept;
      SimulationTime& operator-=(const TimeDelta& delta) noexcept;

     private:
      friend TimeDelta;
      // to gain access to approx?
      template <typename U>
      friend bool operator==(SimulationTime<U> const&,
                             SimulationTime<U> const&) noexcept;
      template <typename U>
      friend bool operator==(SimulationTimeDelta<U> const&,
                             SimulationTimeDelta<U> const&) noexcept;
      friend ::elastica::io::Serialize<This>;
      // for serialization purposes
      inline constexpr auto value() & noexcept -> Real& { return value_; };

      static const Approx approx;

      Real value_{std::numeric_limits<Real>::signaling_NaN()};
    };

    template <typename Real>
    const Approx SimulationTime<Real>::approx =
        Approx::custom()
            .epsilon(std::numeric_limits<double>::epsilon() * 100)
            .scale(1.0);

    template <typename Real>
    class SimulationTimeDelta {
     private:
      using Time = SimulationTime<Real>;
      using This = SimulationTimeDelta<Real>;

     public:
      /// Default constructor gives an invalid TimeDelta.
      SimulationTimeDelta() noexcept = default;
      explicit SimulationTimeDelta(double value) noexcept : value_(value){}

      inline constexpr auto value() const& noexcept -> Real { return value_; }
      explicit inline constexpr operator Real() const noexcept {
        return value();
      }

      SimulationTimeDelta& operator+=(
          const SimulationTimeDelta& other) noexcept {
        value_ += other.value_;
        return *this;
      }

      SimulationTimeDelta& operator-=(
          const SimulationTimeDelta& other) noexcept {
        value_ -= other.value_;
        return *this;
      }

      SimulationTimeDelta& operator*=(double mult) noexcept {
        value_ *= mult;
        return *this;
      }

      SimulationTimeDelta& operator/=(double div) noexcept {
        value_ /= div;
        return *this;
      }

     private:
      friend Time;
      friend ::elastica::io::Serialize<This>;
      // for serialization purposes
      inline constexpr auto value() & noexcept -> Real& { return value_; };

      Real value_{std::numeric_limits<Real>::signaling_NaN()};
    };

    template <typename Real>
    SimulationTime<Real>& SimulationTime<Real>::operator+=(
        SimulationTimeDelta<Real> const& delta) noexcept {
      value_ += delta.value();
      return *this;
    }

    template <typename Real>
    SimulationTime<Real>& SimulationTime<Real>::operator-=(
        SimulationTimeDelta<Real> const& delta) noexcept {
      value_ -= delta.value();
      return *this;
    }

    template <typename Real>
    bool operator==(SimulationTime<Real> const& a,
                    SimulationTime<Real> const& b) noexcept {
      // compare equality with some tolerance t
      return a.value() == SimulationTime<Real>::approx(b.value());
    }

    template <typename Real>
    bool operator!=(const SimulationTime<Real>& a,
                    const SimulationTime<Real>& b) noexcept {
      return not(a == b);
    }

    template <typename Real>
    bool operator<(const SimulationTime<Real>& lhs,
                   const SimulationTime<Real>& rhs) noexcept {
      return (lhs).value() < rhs.value() || lhs == rhs;
    }

    template <typename Real>
    bool operator>(const SimulationTime<Real>& a,
                   const SimulationTime<Real>& b) noexcept {
      return b < a;
    }

    template <typename Real>
    bool operator<=(const SimulationTime<Real>& a,
                    const SimulationTime<Real>& b) noexcept {
      return not(a > b);
    }

    template <typename Real>
    bool operator>=(const SimulationTime<Real>& a,
                    const SimulationTime<Real>& b) noexcept {
      return not(a < b);
    }

    // TimeDelta <cmp> TimeDelta

    template <typename Real>
    bool operator==(const SimulationTimeDelta<Real>& a,
                    const SimulationTimeDelta<Real>& b) noexcept {
      // compare equality with some tolerance t
      return a.value() == SimulationTime<Real>::approx(b.value());
    }

    template <typename Real>
    bool operator!=(const SimulationTimeDelta<Real>& a,
                    const SimulationTimeDelta<Real>& b) noexcept {
      return not(a == b);
    }

    template <typename Real>
    bool operator<(const SimulationTimeDelta<Real>& lhs,
                   const SimulationTimeDelta<Real>& rhs) noexcept {
      return (lhs).value() < rhs.value() || lhs == rhs;
    }

    template <typename Real>
    bool operator>(const SimulationTimeDelta<Real>& a,
                   const SimulationTimeDelta<Real>& b) noexcept {
      return b < a;
    }

    template <typename Real>
    bool operator<=(const SimulationTimeDelta<Real>& a,
                    const SimulationTimeDelta<Real>& b) noexcept {
      return not(a > b);
    }

    template <typename Real>
    bool operator>=(const SimulationTimeDelta<Real>& a,
                    const SimulationTimeDelta<Real>& b) noexcept {
      return not(a < b);
    }

    // Time <op> Time
    template <typename Real>
    auto operator-(SimulationTime<Real> const& a,
                   SimulationTime<Real> const& b) noexcept
        -> SimulationTimeDelta<Real> {
      ELASTICA_ASSERT(a > b, "Cannot subtract earlier times from later times!");
      return SimulationTimeDelta<Real>{a.value() - b.value()};
    }

    // SimulationTime <op> SimulationTimeDelta<Real>
    // SimulationTimeDelta<Real> <op> SimulationTime
    template <typename Real>
    SimulationTime<Real> operator+(
        SimulationTime<Real> a, const SimulationTimeDelta<Real>& b) noexcept {
      a += b;
      return a;
    }

    template <typename Real>
    SimulationTime<Real> operator+(const SimulationTimeDelta<Real>& a,
                                   SimulationTime<Real> b) noexcept {
      b += a;
      return b;
    }

    template <typename Real>
    SimulationTime<Real> operator-(
        SimulationTime<Real> a, const SimulationTimeDelta<Real>& b) noexcept {
      a -= b;
      return a;
    }

    // SimulationTimeDelta<Real> <op> Scalar
    // useful in time-stepping algorithms
    template <typename Real>
    SimulationTimeDelta<Real> operator*(SimulationTimeDelta<Real> lhs,
                                        const double mult) noexcept {
      lhs *= mult;
      return lhs;
    }

    template <typename Real>
    SimulationTimeDelta<Real> operator*(
        const double mult, SimulationTimeDelta<Real> rhs) noexcept {
      rhs *= mult;
      return rhs;
    }

    template <typename Real>
    SimulationTimeDelta<Real> operator/(SimulationTimeDelta<Real> lhs,
                                        const double div) noexcept {
      lhs /= div;
      return lhs;
    }

    // SimulationTimeDelta<Real> <op> SimulationTimeDelta<Real>

    template <typename Real>
    SimulationTimeDelta<Real> operator+(
        SimulationTimeDelta<Real> a,
        const SimulationTimeDelta<Real>& b) noexcept {
      a += b;
      return a;
    }

    template <typename Real>
    SimulationTimeDelta<Real> operator-(
        SimulationTimeDelta<Real> a,
        const SimulationTimeDelta<Real>& b) noexcept {
      a -= b;
      return a;
    }

    template <typename Real>
    std::ostream& operator<<(std::ostream& os,
                             const SimulationTime<Real>& t) noexcept {
      return os << t.value();
    }

    template <typename Real>
    std::ostream& operator<<(std::ostream& os,
                             const SimulationTimeDelta<Real>& dt) noexcept {
      return os << dt.value();
    }

  }  // namespace detail

  namespace io {

    // specialize serialization
    template <typename Real>
    struct Serialize<::elastica::detail::SimulationTime<Real>> {
      using type = ::elastica::detail::SimulationTime<Real>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& time) {
        ar(time.value());
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& time) {
        ar(time.value());
      }
    };

    template <typename Real>
    struct Serialize<::elastica::detail::SimulationTimeDelta<Real>> {
      using type = ::elastica::detail::SimulationTimeDelta<Real>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& time_delta) {
        ar(time_delta.value());
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& time_delta) {
        ar(time_delta.value());
      }
    };

  }  // namespace io

}  // namespace elastica
