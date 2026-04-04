//==============================================================================
/*!
//  \file
//  \brief Wall-clock policies
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
    /*!\brief Timing policy for the measurement of the wall clock time.
     * \ingroup timing
     *
     * The WcPolicy class represents the timing policy for wall clock time
     * measurements that can be used in combination with the Timer class
     * template. This combination is realized with the WcTimer type definition.
     */
    struct WcPolicy {
     public:
      //**Type definitions******************************************************
      //! TimeStamp type
      using TimeStamp = decltype(get_wc_time());
      using Duration = std::chrono::duration<double>;
      // decltype(std::declval<TimeStamp>() - std::declval<TimeStamp>());
      //************************************************************************

      //**Timing functions******************************************************
      /*!\name Timing functions */
      //@{

      //************************************************************************
      /*!\brief Returns a timestamp of the current wall clock time in seconds.
       *
       * \return Wall clock timestamp
       */
      static inline auto get_time_stamp() -> TimeStamp { return get_wc_time(); }
      //************************************************************************

      //************************************************************************
      /*!\brief Converts a duration into seconds
       */
      static inline auto to_seconds(Duration duration) -> double {
        return duration.count();
      }
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace timing

}  // namespace elastica
