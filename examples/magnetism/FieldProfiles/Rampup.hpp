//==============================================================================
/*!
//  \file
//  \brief Facilities to implement rampup
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

// TODO : export to rampup

#pragma once

#include <cmath>

namespace elastica {

  namespace detail {

    template <typename Real_>
    struct LinearProfile {
      using Real = Real_;

      Real operator()(Real val, Real start, Real interval) const noexcept {
        return (val < start)
                   ? 0.0
                   : ((val <= (start + interval)) ? ((val - start) / interval)
                                                  : 1.0);
      }
    };

    template <typename Real_>
    struct SinProfile {
      using Real = Real_;

      // See https://doi.org/10.1016/0021-9991(77)90100-0
      // As reference from
      // https://scihub.bban.top/https://doi.org/10.1137/0729022
      // In his convention kernel's width is 4h. That is 4h_paper = h_impl, so
      // the kernel is modified accordingly

      // Discrete Heaviside
      // Expr is 0.5*x + 0.5*sin(pi*x)/pi + 0.5
      inline auto op(Real shifted_scaled_x) const noexcept -> Real {
        return (static_cast<Real>(0.5) *
                (1.0 + shifted_scaled_x +
                 std::sin(static_cast<Real>(M_PI) * shifted_scaled_x) / M_PI));
      };

      Real operator()(Real val, Real start, Real interval) const noexcept {
        return (val < start)
                   ? 0.0
                   : ((val <= (start + interval))
                          ? op(Real(2.0) * ((val - start) / interval) -
                               Real(1.0))
                          : 1.0);
      }
    };

  }  // namespace detail

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief A linear ramping profile
   * \ingroup extension
   */
  template <typename Real_, typename Profile_>
  struct Rampup {
   public:
    //**Type definitions********************************************************
    //! Real type
    using Real = Real_;
    //! Profile type
    using Profile = Profile_;
    //**************************************************************************

    //**Constructor***********************************************************
    /*!\name Constructor */
    //@{
    Rampup()
        : ramp_start_time_(Real(0.0)), ramp_interval_(Real(0.0)), profile_() {}
    Rampup(Real start, Real interval, Profile profile)
        : ramp_start_time_(std::move(start)),
          ramp_interval_(std::move(interval)),
          profile_(std::move(profile)) {}
    //@}
    //************************************************************************

    //**Access operators********************************************************
    /*!\name Access operators */
    //@{
    inline auto interval() & noexcept -> Real& { return ramp_interval_; }
    inline auto interval() const& noexcept -> Real { return ramp_interval_; }

    inline auto start_time() & noexcept -> Real& { return ramp_start_time_; }
    inline auto start_time() const& noexcept -> Real {
      return ramp_start_time_;
    }
    //@}
    //**************************************************************************

    friend void swap(Rampup& first, Rampup& second) noexcept {
      using std::swap;

      // by swapping the members of two objects,
      // the two objects are effectively swapped
      swap(first.profile_, second.profile_);
      swap(first.ramp_start_time_, second.ramp_start_time_);
      swap(first.ramp_interval_, second.ramp_interval_);
    }

    //**Function methods********************************************************
    /*!\name Function methods */
    //@{
    Real operator()(Real val) const noexcept {
      return profile_(val, start_time(), interval());
    }
    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Start time of ramp
    Real ramp_start_time_;
    //! Interval of ramp
    Real ramp_interval_;
    //! Profile
    Profile profile_;
    //@}
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Idiomatically makes a rampup
   * \ingroup extension
   */
  template <typename Real_, typename F>
  auto make_rampup(Real_ start, Real_ interval, F func) noexcept {
    return Rampup<Real_, F>(std::move(start), std::move(interval),
                            std::move(func));
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Idiomatically makes a linear rampup
   * \ingroup extension
   */
  template <typename Real_>
  auto make_linear_rampup(Real_ start, Real_ interval) {
    return make_rampup(std::move(start), std::move(interval),
                       detail::LinearProfile<Real_>{});
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Idiomatically makes a sin rampup
   * \ingroup extension
   */
  template <typename Real_>
  auto make_sin_rampup(Real_ start, Real_ interval) {
    return make_rampup(std::move(start), std::move(interval),
                       detail::SinProfile<Real_>{});
  }
  //****************************************************************************

}  // namespace elastica
