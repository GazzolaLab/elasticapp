//==============================================================================
/*!
//  \file
//  \brief CPU-clock policies
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

#include "Utilities/TimeStamps.h"

namespace elastica {

  namespace timing {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Timing policy for the measurement of the CPU time.
     * \ingroup timing
     *
     * The CpuPolicy class represents the timing policy for CPU time
     * measurements that can be used in combination with the Timer class
     * template. This combination is realized with the CpuTimer type definition.
     */
    struct CpuPolicy {
     public:
      //**Type definitions******************************************************
      //! TimeStamp type
      using TimeStamp = decltype(get_cpu_time());
      using Duration =
          decltype(std::declval<TimeStamp>() - std::declval<TimeStamp>());
      //************************************************************************

      //**Timing functions******************************************************
      /*!\name Timing functions */
      //@{

      //************************************************************************
      /*!\brief Returns a timestamp of the current CPU time in seconds.
       *
       * \return CPU timestamp in seconds.
       */
      static inline auto get_time_stamp() -> TimeStamp {
        return get_cpu_time();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Converts a duration into seconds
       */
      static inline auto to_seconds(Duration duration) -> double {
        return duration / static_cast<double>(CLOCKS_PER_SEC);
      }
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace timing

}  // namespace elastica
