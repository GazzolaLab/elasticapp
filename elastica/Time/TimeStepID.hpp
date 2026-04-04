//==============================================================================
/*!
//  \file
//  \brief A unique ID type for current simulation time-step
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

#include <cstdint>
#include <iosfwd>

#include "Time/Types.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Simulation time-step ID
   * \ingroup time
   *
   * \details
   * A unique identifier for the temporal state of an integrated
   * system.
   */
  class TimeStepID {
   public:
    //**Type definitions********************************************************
    //! The step type
    using Step = std::uint64_t;
    //! The step type
    using SubStep = std::uint8_t;
    //**************************************************************************

    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief Constructor for the TimeStepID class.
     *
     * \param n_substeps Number of substeps of the current time-stepping alg.
     *
     * \note
     * Takes in a Step for preventing constexpr errors (downcast bad).
     */
    explicit constexpr TimeStepID(Step n_substeps)
        : to_step_threshold(static_cast<SubStep>(n_substeps)){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Copy Constructor for the TimeStepID class.
     *
     */
    TimeStepID(TimeStepID const& other) = default;
    //**************************************************************************

    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~TimeStepID() = default;
    //@}
    //**************************************************************************

    //**Access operators********************************************************
    /*!\name Access operators */
    //@{

    //************************************************************************
    /*!\brief Access to the time value.
     */
    inline constexpr auto value() const& noexcept -> Step { return step_; };
    //************************************************************************

    //************************************************************************
    /*!\brief Access to the time value.
     */
    inline constexpr auto value() & noexcept -> Step& { return step_; };
    //************************************************************************

    //**************************************************************************
    /*!\brief Access to the time-step.
     */
    constexpr auto step() const& noexcept -> Step { return step_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the time-step.
     */
    constexpr auto step() & noexcept -> Step& { return step_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the time sub-step.
     */
    constexpr auto substep() const& noexcept -> SubStep { return substep_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the time sub-step.
     */
    constexpr auto substep() & noexcept -> SubStep& { return substep_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the step threshold
     */
    constexpr auto step_threshold() const& noexcept -> SubStep {
      return to_step_threshold;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the step threshold
     */
    constexpr auto step_threshold() & noexcept -> SubStep& {
      return to_step_threshold;
    }
    //**************************************************************************

    //@}
    //**************************************************************************

    //**Increment/decrement operators*******************************************
    /*!\name Increment/decrement operators */
    //@{

    //**************************************************************************
    /*!\brief Pre-increment operator.
     *
     * \return Reference to the incremented iterator.
     */
    inline constexpr TimeStepID& operator++() noexcept {
      if (++substep_ == to_step_threshold) {
        ++step_;
        substep_ = 0;
      }
      return *this;
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Post-increment operator.
     *
     * \return The previous position of the iterator.
     */
    inline constexpr const TimeStepID operator++(int) noexcept {
      TimeStepID copied(*this);
      operator++();
      return copied;
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Pre-decrement operator.
     *
     * \return Reference to the decremented iterator.
     *
     * \note
     * This is useful in contexts of adaptive-time stepping simulations where
     * time temporarily flows backwards
     */
    inline constexpr TimeStepID& operator--() noexcept {
      if (substep_) {
        --substep_;
      } else {
        substep_ = to_step_threshold - 1;
        --step_;
      }
      return *this;
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Post-decrement operator.
     *
     * \return The previous position of the iterator.
     */
    inline constexpr const TimeStepID operator--(int) noexcept {
      TimeStepID copied(*this);
      operator--();
      return copied;
    };
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Count of step
    Step step_{0};
    //! Count of substep
    SubStep substep_{0};
    //! Threshold
    SubStep to_step_threshold;
    //@}
    //**************************************************************************
  };
  //****************************************************************************

  //============================================================================
  //
  //  GLOBAL OPERATORS
  //
  //============================================================================

  //****************************************************************************
  /*!\name Binary logical TimeStepID operators */
  //@{
  bool operator==(const TimeStepID& a, const TimeStepID& b) noexcept;
  bool operator!=(const TimeStepID& a, const TimeStepID& b) noexcept;
  bool operator<(const TimeStepID& a, const TimeStepID& b) noexcept;
  bool operator<=(const TimeStepID& a, const TimeStepID& b) noexcept;
  bool operator>(const TimeStepID& a, const TimeStepID& b) noexcept;
  bool operator>=(const TimeStepID& a, const TimeStepID& b) noexcept;
  //@}
  //****************************************************************************

  //****************************************************************************
  /*!\name Stream operators */
  //@{
  //! Stream operator for TimeStepID
  //! \ingroup time
  std::ostream& operator<<(std::ostream& s, const TimeStepID& id) noexcept;
  //@}
  //****************************************************************************

}  // namespace elastica