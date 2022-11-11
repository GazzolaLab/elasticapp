//==============================================================================
/*!
//  \file
//  \brief Facilities for ramping a profile in time from off (0) to on (1)
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cmath>
#include <utility>

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup rampup Rampup
 * \ingroup utils
 * \brief Ramp profiles from off (0) to on (1)
 */
//******************************************************************************

namespace elastica {

  namespace rampup {

    //    template <class U, class T>
    //    struct is_explicitly_convertible
    //        : public std::integral_constant<bool,
    //                                        (std::is_constructible<T,
    //                                        U>::value &&
    //                                         !std::is_convertible<U,
    //                                         T>::value)> {};

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Implements a step ramp profile
     * \ingroup rampup
     *
     * \warning
     * The activation is not C-0 continuous!
     *
     * \tparam Real_ The real number type
     */
    template <typename Real_>
    struct StepProfile {
      //**Type definitions******************************************************
      using Real = Real_;
      //************************************************************************

      //************************************************************************
      /*!\brief Operator to apply ramping
       *
       * \param val Current value
       * \param start Start value of ramp
       * \param interval Interval of ramp
       */
      Real operator()(Real, Real, Real) const noexcept { return 1.0; }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Implements a linear ramp profile
     * \ingroup rampup
     *
     * \warning
     * The activation is only C-0 continuous!
     *
     * \tparam Real_ The real number type
     */
    template <typename Real_>
    struct LinearProfile {
      //**Type definitions******************************************************
      using Real = Real_;
      //************************************************************************

      //************************************************************************
      /*!\brief Operator to apply ramping
       *
       * \param val Current value
       * \param start Start value of ramp
       * \param interval Interval of ramp
       */
      Real operator()(Real val, Real start, Real interval) const noexcept {
        return (val - start) / interval;
      }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Implements a sinusoidal ramp profile
     * \ingroup rampup
     *
     * \notes
     * See https://doi.org/10.1016/0021-9991(77)90100-0
     * In his convention kernel's width is 4h. That is 4h_paper = h_impl, so
     * the kernel is modified accordingly
     *
     * \tparam Real_ The real number type
     */
    template <typename Real_>
    struct SinProfile {
      using Real = Real_;

      // Discrete Heaviside
      // Expr is 0.5*x + 0.5*sin(pi*x)/pi + 0.5
      inline auto op(Real shifted_scaled_x) const noexcept -> Real {
        return static_cast<Real>(0.5) *
               (shifted_scaled_x +
                static_cast<Real>(1.0 + (std::sin(static_cast<Real>(M_PI) *
                                                  shifted_scaled_x) /
                                         M_PI)));
      };

      //************************************************************************
      /*!\brief Operator to apply ramping
       *
       * \param val Current value
       * \param start Start value of ramp
       * \param interval Interval of ramp
       */
      Real operator()(Real val, Real start, Real interval) const noexcept {
        return op(Real(2.0) * ((val - start) / interval) - Real(1.0));
      }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief A data-structure that implements a ramping profile
     * \ingroup rampup
     *
     * \details
     * Ramps a continuous signal from 0 to 1 based on an input (usually time).
     *
     * \tparam Real_ The real number type
     * \tparam Profile_ Profile policy to rampup. See for example
     * elastica::rampup::LinearProfile, elastica::rampup::SinProfile
     */
    template <typename Real_, typename Profile_>
    struct Rampup {
     public:
      //**Type definitions******************************************************
      //! Real type
      using Real = Real_;
      //! Profile type
      using Profile = Profile_;
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      Rampup()
          : ramp_start_time_(Real(0.0)),
            ramp_interval_(Real(0.0)),
            profile_() {}
      Rampup(Real start, Real interval, Profile profile)
          : ramp_start_time_(std::move(start)),
            ramp_interval_(std::move(interval)),
            profile_(std::move(profile)) {}
      //@}
      //************************************************************************

      //**Access operators******************************************************
      /*!\name Access operators */
      //@{
      inline auto interval() & noexcept -> Real& { return ramp_interval_; }
      inline auto interval() const& noexcept -> Real { return ramp_interval_; }

      inline auto start_time() & noexcept -> Real& { return ramp_start_time_; }
      inline auto start_time() const& noexcept -> Real {
        return ramp_start_time_;
      }
      //@}
      //************************************************************************

      friend void swap(Rampup& first, Rampup& second) noexcept {
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(first.profile_, second.profile_);
        swap(first.ramp_start_time_, second.ramp_start_time_);
        swap(first.ramp_interval_, second.ramp_interval_);
      }

      //**Function methods******************************************************
      /*!\name Function methods */
      //@{
      Real operator()(Real val) const noexcept {
        return (val < start_time())
                   ? 0.0
                   : ((val <= (start_time() + interval()))
                          ? profile_(val, start_time(), interval())
                          : 1.0);
      }
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Start time of ramp
      Real ramp_start_time_;
      //! Interval of ramp
      Real ramp_interval_;
      //! Profile
      Profile profile_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace rampup

  //****************************************************************************
  /*!\brief Idiomatically makes a rampup profile
   * \ingroup rampup
   *
   * \param start Start time of ramp
   * \param interval Interval of ramp
   * \param profile Profile of ramp
   * \note
   * start and interval need to be of the same type, and should be a floating
   * point value or explicitly convertible to floating point value.
   *
   * \see elastica::rampup::Rampup
   */
  template <typename Real_, typename F>
  auto make_rampup(Real_ start, Real_ interval, F profile) noexcept {
    using rampup::Rampup;
    return Rampup<Real_, F>(std::move(start), std::move(interval),
                            std::move(profile));
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Idiomatically makes a step rampup
   * \ingroup rampup
   *
   * \example
   * \snippet Test_Rampup.cpp make_step_rampup_eg
   *
   * \param start Start time of ramp
   * \param interval Interval of ramp
   *
   * \note
   * start and interval need to be of the same type, and should be a floating
   * point value or explicitly convertible to floating point value.
   *
   * \see elastica::make_rampup
   */
  template <typename Real_>
  auto make_step_rampup(Real_ start, Real_ interval) noexcept {
    return make_rampup(std::move(start), std::move(interval),
                       rampup::StepProfile<Real_>{});
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Idiomatically makes a linear rampup
   * \ingroup rampup
   *
   * \example
   * \snippet Test_Rampup.cpp make_linear_rampup_eg
   *
   * \param start Start time of ramp
   * \param interval Interval of ramp
   *
   * \note
   * start and interval need to be of the same type, and should be a floating
   * point value or explicitly convertible to floating point value.
   *
   * \see elastica::make_rampup
   */
  template <typename Real_>
  auto make_linear_rampup(Real_ start, Real_ interval) noexcept {
    return make_rampup(std::move(start), std::move(interval),
                       rampup::LinearProfile<Real_>{});
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Idiomatically makes a sin rampup
   * \ingroup rampup
   *
   * \example
   * \snippet Test_Rampup.cpp make_sin_rampup_eg
   *
   * \param start Start time of ramp
   * \param interval Interval of ramp
   *
   * \note
   * start and interval need to be of the same type, and should be a floating
   * point value or explicitly convertible to floating point value.
   *
   * \see elastica::make_rampup
   */
  template <typename Real_>
  auto make_sin_rampup(Real_ start, Real_ interval) noexcept {
    return make_rampup(std::move(start), std::move(interval),
                       rampup::SinProfile<Real_>{});
  }
  //****************************************************************************

}  // namespace elastica
