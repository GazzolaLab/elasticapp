//==============================================================================
/*!
//  \file
//  \brief Get the streamed output of `t` as a `std::string`
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

#include <sstream>
#include <string>

//******************************************************************************
/*!\brief Get the streamed output of `t` as a `std::string`
 * \ingroup utilities
 */
template <typename T>
std::string get_output(const T& t) noexcept {
  std::ostringstream os;
  os << t;
  return os.str();
}
//******************************************************************************
