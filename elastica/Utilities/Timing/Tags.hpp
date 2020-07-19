#pragma once

#include "CpuTimer.h"
#include "WcTimer.h"

namespace elastica {

  namespace timing {

    namespace tags {

      template <typename T>
      struct TimerTag {
        using type = T;
      };

      using WcTimerTag = TimerTag<WcTimer>;

      using CpuTimerTag = TimerTag<CpuTimer>;

    }  // namespace tags

  }  // namespace timing

}  // namespace elastica