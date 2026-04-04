//==============================================================================
/*!
//  \file
//  \brief Helpers for formatting filenames
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************
#include "Helpers.hpp"
//
#include <algorithm>  // min
#include <utility>    // move

namespace elastica {

  namespace io {

    auto format_id(std::size_t n_zeros, std::string idstr, char pad)
        -> std::string {
      return std::string(n_zeros - std::min(n_zeros, idstr.length()), pad) +
             std::move(idstr);
    }

    auto format_suffix(std::string const& suffix, char pad) -> std::string {
      return (suffix.empty() ? "" : pad + suffix);
    }

  }  // namespace io

}  // namespace elastica