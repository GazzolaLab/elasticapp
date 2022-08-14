//==============================================================================
/*!
//  \file
//  \brief Simulation time and increments
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <limits>
#include <ostream>

#include "Time/Types.hpp"
//
#include "Utilities/Approx.hpp"

namespace elastica {

  namespace impl {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename Real>
    class SimulationTimeDelta;
    /*! \endcond */
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Simulation time
     * \ingroup time
     *
     * \details
     * The time in a simulation.  Times can be safely compared for equality and
     * provides more meaningful program semantics
     */
    template <typename Real_>
    class SimulationTime {
     private:
      //**Type definitions******************************************************
      using TimeDelta = SimulationTimeDelta<Real_>;
      using This = SimulationTime<Real_>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Real type
      using Real = Real_;
      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Default constructor gives an invalid time
       */
      SimulationTime() noexcept = default;
      //************************************************************************

      //************************************************************************
      /*!\brief Constructor for the SimulationTime class.
       *
       * \param restarted_time Time for initializing simulation
       */
      explicit SimulationTime(Real restarted_time) noexcept
          : value_(restarted_time){};
      //************************************************************************

      //************************************************************************
      /*!\brief Default copy constructor
       */
      SimulationTime(const SimulationTime&) = default;
      //************************************************************************

      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~SimulationTime() = default;
      //@}
      //************************************************************************

      //**Access operators******************************************************
      /*!\name Access operators */
      //@{

      //************************************************************************
      /*!\brief Access to the time value.
       */
      inline constexpr auto value() const& noexcept -> Real { return value_; };
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the time value.
       */
      inline constexpr auto value() & noexcept -> Real& { return value_; };
      //************************************************************************

      //************************************************************************
      /*!\brief Explicit conversion operator to the underlying time
       */
      /*explicit*/ inline constexpr operator Real() const noexcept {
        return value();
      }
      //************************************************************************
      //@}
      //************************************************************************

      //**Increment/decrement operators*****************************************
      /*!\name Increment/decrement operators */
      //@{
      SimulationTime& operator+=(const TimeDelta& inc) noexcept;
      SimulationTime& operator-=(const TimeDelta& dec) noexcept;
      //@}
      //************************************************************************

     private:
      //**Friendships***********************************************************
      //! Friendship with increments
      friend TimeDelta;
      // to gain access to approx?
      template <typename U>
      friend bool operator==(SimulationTime<U> const&,
                             SimulationTime<U> const&) noexcept;
      template <typename U>
      friend bool operator==(SimulationTimeDelta<U> const&,
                             SimulationTimeDelta<U> const&) noexcept;
      //************************************************************************

      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! approximate operator to compare times
      static const Approx approx;
      //! Value of time
      Real value_{std::numeric_limits<Real>::signaling_NaN()};
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    // Declaration of a static approx
    template <typename Real>
    const Approx SimulationTime<Real>::approx =
        Approx::custom()
            .epsilon(std::numeric_limits<Real>::epsilon() * 100)
            .scale(1.0);
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Interval of time change
     * \ingroup time
     *
     * \details
     * Represents an interval of time between two Times.
     */
    template <typename Real_>
    class SimulationTimeDelta {
     private:
      //**Type definitions******************************************************
      using Time = SimulationTime<Real_>;
      using This = SimulationTimeDelta<Real_>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Real type
      using Real = Real_;
      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Default constructor, gives an invalid time-delta
       */
      SimulationTimeDelta() noexcept = default;
      //************************************************************************

      //************************************************************************
      /*!\brief Value constructor
       *
       * \param value Time-increment value
       */
      explicit SimulationTimeDelta(Real value) noexcept : value_(value) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Default copy constructor
       */
      SimulationTimeDelta(const SimulationTimeDelta&) = default;
      //************************************************************************

      //@}
      //************************************************************************

      //**Access operators******************************************************
      /*!\name Access operators */
      //@{

      //************************************************************************
      /*!\brief Access to the time value.
       */
      inline constexpr auto value() const& noexcept -> Real { return value_; };
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the time value.
       */
      inline constexpr auto value() & noexcept -> Real& { return value_; };
      //************************************************************************

      //************************************************************************
      /*!\brief Explicit conversion operator to the underlying time
       */
      explicit inline constexpr operator Real() const noexcept {
        return value();
      }
      //************************************************************************
      //@}
      //************************************************************************

      //**Assignment operators**************************************************
      /*!\name Assignment operators */
      //@{

      //************************************************************************
      /*!\brief Addition assignment operator.
       *
       * \param inc The time increment
       * \return Reference to the incremented time-delta.
       */
      SimulationTimeDelta& operator+=(const SimulationTimeDelta& inc) noexcept {
        value_ += inc.value_;
        return *this;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Subtraction assignment operator.
       *
       * \param inc The time decrement
       * \return Reference to the decremented time-delta.
       */
      SimulationTimeDelta& operator-=(const SimulationTimeDelta& dec) noexcept {
        value_ -= dec.value_;
        return *this;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Multiplication assignment operator.
       *
       * \param mult The multiplicative factor
       * \return Reference to the multiplied time-delta.
       */
      SimulationTimeDelta& operator*=(Real mult) noexcept {
        value_ *= mult;
        return *this;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Division assignment operator.
       *
       * \param div The dividing factor
       * \return Reference to the divided time-delta.
       */
      SimulationTimeDelta& operator/=(double div) noexcept {
        value_ /= div;
        return *this;
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Friendships***********************************************************
      //! Friendship with time
      friend Time;
      //************************************************************************

      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      Real value_{std::numeric_limits<Real>::signaling_NaN()};
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  ASSIGNMENT OPERATORS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Addition assignment operator.
     *
     * \param inc The time increment
     * \return Reference to the incremented time.
     */
    template <typename Real>
    SimulationTime<Real>& SimulationTime<Real>::operator+=(
        SimulationTimeDelta<Real> const& inc) noexcept {
      value() += inc.value();
      return *this;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Subtraction assignment operator.
     *
     * \param inc The time decrement
     * \return Reference to the decrement time.
     */
    template <typename Real>
    SimulationTime<Real>& SimulationTime<Real>::operator-=(
        SimulationTimeDelta<Real> const& dec) noexcept {
      value() -= dec.value();
      return *this;
    }
    //**************************************************************************

    //==========================================================================
    //
    //  GLOBAL OPERATORS
    //
    //==========================================================================

    //**************************************************************************
    /*!\name Binary logical Time operators */
    //@{

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
    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\name Binary logical TimeDelta operators */
    //@{
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
    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\name Binary arithmetic Time operators */
    //@{
    template <typename Real>
    auto operator-(SimulationTime<Real> const& a,
                   SimulationTime<Real> const& b) noexcept
        -> SimulationTimeDelta<Real> {
      ELASTICA_ASSERT(a > b, "Cannot subtract earlier times from later times!");
      return SimulationTimeDelta<Real>{a.value() - b.value()};
    }
    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\name Binary arithmetic TimeDelta operators */
    //@{
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
    //@}
    //**************************************************************************

    // SimulationTimeDelta<Real> <op> Scalar
    // useful in time-stepping algorithms
    //**************************************************************************
    /*!\name Binary arithmetic TimeDelta-scalar operators */
    //@{
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
    //@}
    //**************************************************************************

    // SimulationTime <op> SimulationTimeDelta<Real>
    // SimulationTimeDelta<Real> <op> SimulationTime
    //**************************************************************************
    /*!\name Binary arithmetic Time-TimeDelta operators */
    //@{
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
    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\name Stream operators */
    //@{
    //! Stream operator for SimulationTime
    //! \ingroup time
    template <typename Real>
    std::ostream& operator<<(std::ostream& os,
                             const SimulationTime<Real>& t) noexcept {
      return os << t.value();
    }

    //! Stream operator for SimulationTimeDelta
    //! \ingroup time
    template <typename Real>
    std::ostream& operator<<(std::ostream& os,
                             const SimulationTimeDelta<Real>& dt) noexcept {
      return os << dt.value();
    }
    //@}
    //**************************************************************************

  }  // namespace impl

}  // namespace elastica
