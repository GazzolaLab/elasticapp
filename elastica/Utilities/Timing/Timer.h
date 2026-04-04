//==============================================================================
/*!
//  \file
//  \brief Timer class
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <algorithm>
#include <limits>
#include <ostream>

#include "Utilities/TimeStamps.h"

namespace elastica {

  namespace timing {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Progress timer for time and performance measurements.
     * \ingroup timing
     *
     * The Timer class offers timing & benchmarking functionality for all kinds
     * of applications.
     *
     * \example
     * The following example code demonstrates the use of the
     * WcTimer class, which combines the Timer class template with the WcPolicy
     * for wall clock time measurements, for a single time measurement:
     * \snippet Test_Timer.cpp timer_eg
     *
     * The timer class additionally offers the functionality to start several
     * time measurements in order to evaluate minimal, maximal or average times.
     * The next example demonstrates a possible setup for such a series of time
     * measurements:
     * \snippet Test_Timer.cpp timer_loop_eg
     */
    template <typename TP>  // Timing policy
    class Timer {
     public:
      //**Type definitions******************************************************
      using TimingPolicy = TP;  //!< Timing policy of the Timer.
      using TimeStamp = typename TP::TimeStamp;  //!< Time stamp type
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      explicit inline Timer();
      //@}
      //************************************************************************

      //**Destructor************************************************************
      // No explicitly declared destructor.
      //************************************************************************

      //**Timing functions******************************************************
      /*!\name Timing functions */
      //@{
      inline void start();
      inline void stop();
      inline void lap();
      inline void reset();
      //@}
      //************************************************************************

      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{
      inline std::size_t& get_counter() noexcept { return counter_; }
      inline std::size_t const& get_counter() const noexcept {
        return counter_;
      }
      inline double& get_total() noexcept { return time_; }
      inline double const& get_total() const noexcept { return time_; }
      inline double& get_min() noexcept { return min_; }
      inline double const& get_min() const noexcept { return min_; }
      inline double& get_max() noexcept { return max_; }
      inline double const& get_max() const noexcept { return max_; }
      inline double& get_last() noexcept { return last_; }
      inline double const& get_last() const noexcept { return last_; }
      //@}
      //************************************************************************

      //**Time evaluation functions*********************************************
      /*!\name Time evaluation functions */
      //@{
      inline double total() const;
      inline double average() const;
      inline double min() const;
      inline double max() const;
      inline double last() const;
      inline std::size_t counter() const;
      //@}
      //************************************************************************

     private:
      std::size_t counter_;  //!< Number of performed time measurements.
      TimeStamp start_;      //!< Start of the current time measurement.
      TimeStamp stop_;       //!< End of the current time measurement.
      double time_;          //!< The total elapsed time of all measurements.
      double min_;           //!< The minimal time of all measurements.
      double max_;           //!< The maximal time of all measurements.
      double last_;          //!< The last measured time.
    };
    //**************************************************************************

    //==========================================================================
    //
    //  CONSTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor of the Timer class.
     */
    template <typename TP>  // Timing policy
    inline Timer<TP>::Timer()
        : counter_(0),
          start_(),
          stop_(),
          time_(0.0),
          min_(0.0),
          max_(0.0),
          last_(0.0) {}
    //**************************************************************************

    //==========================================================================
    //
    //  TIMING FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Starting a single time measurement.
     *
     * \return void
     *
     * This function starts a single time measurement.
     */
    template <typename TP>  // Timing policy
    inline void Timer<TP>::start() {
      // Starting the time measurement and calculating a time stamp
      start_ = TimingPolicy::get_time_stamp();
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Ending a single time measurement.
     *
     * \return void
     *
     * This function ends the currently running time measurement and performs
     * the necessary statistical calculations.
     */
    template <typename TP>  // Timing policy
    inline void Timer<TP>::stop() {
      // Stopping the time measurement and calculating a time stamp
      stop_ = TimingPolicy::get_time_stamp();

      // Increasing the counter
      ++counter_;

      // Calculating the wallclock and CPU time
      const double diff = TimingPolicy::to_seconds(stop_ - start_);

      // Average time measurement
      time_ += diff;

      // Minimum time measurement, for the first time instance check for
      // non-zero min
      min_ = min_ > 0.0 ? std::min(diff, min_) : diff;

      // Maximum time measurement
      max_ = std::max(diff, max_);

      // Last time measurement
      last_ = diff;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Ending a single time measurement and starting a new one.
     *
     * \return void
     *
     * This function ends the currently running time measurement, performs the
     * necessary statistical calculations and starts the next time measurement
     * without delay.
     */
    template <typename TP>  // Timing policy
    inline void Timer<TP>::lap() {
      stop();
      start_ = stop_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Resetting the timer.
     *
     * \return void
     *
     * This function completely resets the timer and all information on the
     * performed time measurements. In order to start a new time measurement,
     * the start() function has to be used.
     */
    template <typename TP>  // Timing policy
    inline void Timer<TP>::reset() {
      *this = Timer<TP>{};
    }
    //**************************************************************************

    //==========================================================================
    //
    //  TIME EVALUATION FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Returns the total elapsed time of all performed time measurements.
     *
     * \return The total elapsed time of all time measurements.
     */
    template <typename TP>  // Timing policy
    inline double Timer<TP>::total() const {
      return time_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the average time of all performed time measurements.
     *
     * \return The average time.
     */
    template <typename TP>  // Timing policy
    inline double Timer<TP>::average() const {
      return counter_ ? (time_ / static_cast<double>(counter_)) : double(0);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the minimal time of all performed time measurements.
     *
     * \return The minimal time.
     */
    template <typename TP>  // Timing policy
    inline double Timer<TP>::min() const {
      return min_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the maximal time of all performed time measurements.
     *
     * \return The maximal time.
     */
    template <typename TP>  // Timing policy
    inline double Timer<TP>::max() const {
      return max_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the last measured time.
     *
     * \return The last measured time.
     */
    template <typename TP>  // Timing policy
    inline double Timer<TP>::last() const {
      return last_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the total number of time measurements performed by this
     * timer.
     *
     * \return The number of performed time measurements.
     */
    template <typename TP>  // Timing policy
    inline std::size_t Timer<TP>::counter() const {
      return counter_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Stream operator for a timer
     */
    template <typename TP>
    std::ostream& operator<<(std::ostream& os, Timer<TP> const& t) {
      os << "( ";
      os << " total : " << t.total() << " ,";
      os << " counter : " << t.counter() << " ,";
      os << " average : " << t.average() << " ,";
      os << " min : " << t.min() << " ,";
      os << " max : " << t.max() << " ,";
      os << " last : " << t.last();
      os << " )";
      return os;
    }
    //**************************************************************************

  }  // namespace timing

}  // namespace elastica
