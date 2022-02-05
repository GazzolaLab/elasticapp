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
#include <elastica/Time/TimeStepID.hpp>
#include <elastica/Utilities/AppSupport/Helpers/MultipleJustGreaterThan.hpp>
#include <elastica/Utilities/CRTP.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Requires.hpp>
#include <elastica/Utilities/TypeTraits/IsDetected.hpp>

namespace elastica {

  namespace detail {
    // reset should be called in a non-const context
    template <class T>
    using reset_t = decltype(std::declval<T&>().reset());
  }  // namespace detail

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
    //! Number of substep update // TODO : assumes position verlet
    static constexpr std::size_t n_time_updates_per_step = 2UL;
    //**************************************************************************

   public:
    //**************************************************************************
    /*!\name Run methods */
    //@{

    //**************************************************************************
    /*!\brief Runs the case-study simulation for given number of
     * simulation iterations
     */
    void run_for_n_iterations(std::size_t n_iterations) /* noexcept */ {
      /* todo : only makes sense for a constant time stepper */
      for (std::size_t i = 0UL; i < n_iterations; ++i) {
        // step() updates the current iteration
        self().step();
      }
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Runs the case-study simulation for given number of
     * simulation seconds
     */
    void run_for(::elastica::real_t seconds) /* noexcept */ {
      /* todo : replace with integrate */
      const std::size_t n_additional_iterations = multiple_just_greater_than(
          seconds, static_cast<::elastica::real_t>(dt_));
      run_for_n_iterations(n_additional_iterations);
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
    std::size_t current_iteration() const noexcept {
      return current_iter_.step();
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**************************************************************************
    /*!\name Reset methods */
    //@{

    //**************************************************************************
    /*!\brief Resets the objects in the current scope
     */
    void reset_this() noexcept {
      // by default, a simulation which resets calls this function. The user
      // can override it in the derived class
      time_ = ::elastica::Time{0.0};
      //! The current iteration of the simulation
      current_iter_ = ::elastica::TimeStepID{n_time_updates_per_step};
    }
    //**************************************************************************

   protected:
    //**************************************************************************
    /*!\brief Dispatch for CaseStudy having a reset method
     */
    template <typename U = CaseStudy,
              Requires<::tt::is_detected<detail::reset_t, U>{}> = nullptr>
    void reset_dispatch() {
      self().reset();
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Dispatch for CaseStudy not having a reset method
     */
    template <typename U = CaseStudy,
              Requires<not ::tt::is_detected<detail::reset_t, U>{}> = nullptr>
    void reset_dispatch() const noexcept {};
    //**************************************************************************

   public:
    //**************************************************************************
    /*!\brief Resets the case study
     */
    void reset_case_study() {
      reset_this();
      reset_dispatch();
    }
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
    /*!\brief Sets the case-study simulation iterate to a given value
     */
    void set_current_iteration(std::size_t iter) noexcept {
      current_iter_ = TimeStepID{n_time_updates_per_step};

      // Forward it to the value needed
      for (std::size_t i = 0UL; i < iter; ++i) {
        ++current_iter_;
      }
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

    //**************************************************************************
    /*!\brief Gets the case-study iteration
     */
    inline auto get_current_iteration() -> TimeStepID& {
      return current_iter_;
    };
    //**************************************************************************

    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Default constructor of the CaseStudyInterface class.
     */
    CaseStudyInterface() noexcept { reset_this(); };
    //**************************************************************************

   private:
    //**Member variables******************************************************
    //! The current time of the simulation
    elastica::Time time_{0.0};
    //! The current time-step of the simulation
    elastica::TimeDelta dt_{};
    //! The current iteration of the simulation
    elastica::TimeStepID current_iter_{n_time_updates_per_step};
    //**************************************************************************
  };
  //****************************************************************************

}  // namespace elastica
