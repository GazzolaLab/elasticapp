//==============================================================================
/*!
//  \file
//  \brief Implementation details of TimeMetadata
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "Time/TimeMetadata.hpp"

#include <ostream>
#include <tuple>

namespace elastica {

  bool operator==(TimeMetadata const& a, TimeMetadata const& b) noexcept {
    return std::tie(a.time_v, a.dt, a.id) == std::tie(b.time_v, b.dt, b.id);
  }

  bool operator!=(TimeMetadata const& a, TimeMetadata const& b) noexcept {
    return not(a == b);
  }

  std::ostream& operator<<(std::ostream& s, TimeMetadata const& data) {
    return s << "Time : " << data.time_v << ", dt : " << data.dt
             << ", step : " << data.id;
  }

}  // namespace elastica