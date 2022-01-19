//==============================================================================
/*!
//  \file
//  \brief An interface for simulation case studies in \elastica
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
#include <elastica/Time/Time.hpp>
#include <elastica/Utilities/CRTP.hpp>
#include <elastica/Utilities/DefineTypes.h>

namespace elastica {

  //****************************************************************************
  /*!\brief Interface for \elastica case studies
   * \ingroup application_support
   *
   * \details
   * Sets up a default interface for case studies with time, time-step and
   * time iterate. A case-study needs to derive this interface using the CRTP
   * pattern as shown below
   *
   * \example
   * \snippet tutorial_intermediate.hpp case_study_interface_eg
   *
   * \tparam CaseStudy A user defined case-study whose parameters have to be
   * parsed
   */
  template <typename CaseStudy>
  class CaseStudyInterface : public CRTPHelper<CaseStudy, CaseStudyInterface> {
   private:
    //**Type definitions********************************************************
    //! CRTP Type
    using CRTP = ::elastica::CRTPHelper<CaseStudy, CaseStudyInterface>;
    //! CRTP methods
    using CRTP::self;
    //**************************************************************************

   public:
    //**************************************************************************
    /*!\name Run methods */
    //@{

    //**************************************************************************
    /*!\brief Runs the case-study simulation for given number of
     * simulation seconds
     */
    void run_for(::elastica::real_t seconds) /* noexcept */ {
      /* todo : replace with integrate */
      const std::size_t n_steps =
          current_iter_ + (seconds / static_cast<::elastica::real_t>(dt_));
      for (std::size_t i_step = current_iter_; i_step < n_steps; ++i_step) {
        self().step();
      }
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Runs the case-study simulation till the given time is reached
     */
    void run_till(::elastica::real_t seconds) /* noexcept */ {
      /* todo : replace with integrate */
      if (static_cast<::elastica::real_t>(time_) > seconds)
        return;
      else {
        const ::elastica::real_t time_to_run =
            seconds - static_cast<::elastica::real_t>(time_);
        const std::size_t n_steps =
            current_iter_ +
            (time_to_run / static_cast<::elastica::real_t>(dt_));
        for (std::size_t i_step = current_iter_; i_step < n_steps; ++i_step) {
          self().step();
        }
      }
    }
    //**************************************************************************

    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\name Query methods */
    //@{

    //**************************************************************************
    /*!\brief Gets the current time of the case-study simulation
     */
    ::elastica::real_t current_time() const noexcept {
      return static_cast<::elastica::real_t>(time_);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the current time-step of the case-study simulation
     */
    ::elastica::real_t current_dt() const noexcept {
      return static_cast<::elastica::real_t>(dt_);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the current iteration of the case-study simulation
     */
    std::size_t current_iteration() const noexcept { return current_iter_; }
    //**************************************************************************

    //@}
    //**************************************************************************

   protected:
    //**************************************************************************
    /*!\name Internal inteface methods */
    //@{

    //**************************************************************************
    /*!\brief Sets the case-study simulation time-step to a given value
     */
    void set_dt(::elastica::real_t value) {
      dt_ = ::elastica::TimeDelta{value};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Sets the case-study simulation time to a given value
     */
    void set_current_time(::elastica::real_t value) {
      time_ = ::elastica::Time{value};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the case-study simulation time-step
     */
    inline auto get_dt() -> TimeDelta& { return dt_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the case-study simulation time
     */
    inline auto get_current_time() -> Time& { return time_; };
    //**************************************************************************

    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Default constructor of the CaseStudyInterface class.
     */
    CaseStudyInterface() = default;
    //**************************************************************************

   private:
    //**Member variables******************************************************
    //! The current time of the simulation
    elastica::Time time_{0.0};
    //! The current time-step of the simulation
    elastica::TimeDelta dt_{};
    //! The current iteration of the simulation
    std::size_t current_iter_{0UL};
    //**************************************************************************
  };
  //****************************************************************************

}  // namespace elastica
