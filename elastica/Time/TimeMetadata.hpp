//==============================================================================
/*!
//  \file
//  \brief Composite containing all information for current simulation time-step
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

#include "Time/Types.hpp"
//
#include "Time/SimulationTime.hpp"
#include "Time/TimeStepID.hpp"
//
#include <iosfwd>

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Simulation time metadata
   * \ingroup time
   *
   * \details
   * Provides all metadata about a single time-step in \elastica.
   */
  struct TimeMetadata {
    Time time_v;
    TimeDelta dt;
    TimeStepID id;
  };
  //****************************************************************************

  //****************************************************************************
  //! Equality operator for TimeMetadata
  //! \ingroup time
  bool operator==(TimeMetadata const& a, TimeMetadata const& b) noexcept;
  //****************************************************************************

  //****************************************************************************
  //! Inequality operator for TimeMetadata
  //! \ingroup time
  bool operator!=(TimeMetadata const& a, TimeMetadata const& b) noexcept;
  //****************************************************************************

  //****************************************************************************
  /*!\name Stream operators */
  //@{
  //! Stream operator for TimeMetadata
  //! \ingroup time
  std::ostream& operator<<(std::ostream& s, TimeMetadata const& id);
  //@}
  //****************************************************************************

}  // namespace elastica
