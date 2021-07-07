//
// Created by Tejaswin Parthasarathy on 5/25/21.
//

#pragma once

#include <cmath>
#include <utility>

#include "ErrorHandling/Assert.hpp"

namespace elastica {

  namespace detail {

    static inline bool marginComparison(double lhs, double rhs, double margin) {
      return (lhs + margin >= rhs) && (rhs + margin >= lhs);
    }

  }  // namespace detail

  // TODO : Add docs
  // TODO : Add tests
  class Approx {
   private:
    bool equalityComparisonImpl(double const other) const {
      return detail::marginComparison(m_value, other, m_margin) ||
             detail::marginComparison(
                 m_value, other, m_epsilon * (m_scale + std::fabs(m_value)));
    }

    // Validates the new margin (margin >= 0)
    // out-of-line to avoid including stdexcept in the header
    void setMargin(double new_margin) {
      ELASTICA_ASSERT(new_margin >= 0,
                      "Invalid Approx::margin"
                          << new_margin
                          << ", Approx::Margin has to be non-negative.");
      m_margin = new_margin;
    };

    // Validates the new epsilon (0 < epsilon < 1)
    // out-of-line to avoid including stdexcept in the header
    void setEpsilon(double new_epsilon) {
      ELASTICA_ASSERT(new_epsilon >= 0 && new_epsilon <= 1.0,
                      "Invalid Approx::epsilon: "
                          << new_epsilon
                          << ", which is only allowed to be in [0, 1]");
      m_epsilon = new_epsilon;
    };

   public:
    Approx(double value)
        : m_epsilon(std::numeric_limits<double>::epsilon() * 100),
          m_margin(0.0),
          m_scale(0.0),
          m_value(value) {}

    static Approx custom() { return Approx(0); }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    Approx operator()(T const& value) const {
      Approx approx(static_cast<double>(value));
      approx.m_epsilon = m_epsilon;
      approx.m_margin = m_margin;
      approx.m_scale = m_scale;
      return approx;
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    explicit Approx(T const& value) : Approx(static_cast<double>(value)) {}

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator==(const T& lhs, Approx const& rhs) {
      auto lhs_v = static_cast<double>(lhs);
      return rhs.equalityComparisonImpl(lhs_v);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator==(Approx const& lhs, const T& rhs) {
      return operator==(rhs, lhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator!=(T const& lhs, Approx const& rhs) {
      return !operator==(lhs, rhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator!=(Approx const& lhs, T const& rhs) {
      return !operator==(rhs, lhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator<=(T const& lhs, Approx const& rhs) {
      return static_cast<double>(lhs) < rhs.m_value || lhs == rhs;
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator<=(Approx const& lhs, T const& rhs) {
      return lhs.m_value < static_cast<double>(rhs) || lhs == rhs;
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator>=(T const& lhs, Approx const& rhs) {
      return static_cast<double>(lhs) > rhs.m_value || lhs == rhs;
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    friend bool operator>=(Approx const& lhs, T const& rhs) {
      return lhs.m_value > static_cast<double>(rhs) || lhs == rhs;
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    Approx& epsilon(T const& newEpsilon) {
      double epsilonAsDouble = static_cast<double>(newEpsilon);
      setEpsilon(epsilonAsDouble);
      return *this;
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    Approx& margin(T const& newMargin) {
      double marginAsDouble = static_cast<double>(newMargin);
      setMargin(marginAsDouble);
      return *this;
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<double, T>::value>::type>
    Approx& scale(T const& newScale) {
      m_scale = static_cast<double>(newScale);
      return *this;
    }

   private:
    double m_epsilon;
    double m_margin;
    double m_scale;
    double m_value;
  };

}  // namespace elastica
