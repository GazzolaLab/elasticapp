//==============================================================================
/*!
//  \file
//  \brief pretty_type library to write types in a "pretty" format
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

// Reused from SpECTRE : https://spectre-code.org/
// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "Utilities/PrettyType.hpp"

#include <cstddef>
#include <regex>

namespace pretty_type {

  namespace detail {

    std::string extract_short_name(std::string name) {
      // Remove all template arguments
      const std::regex template_pattern("<[^<>]*>");
      size_t previous_size = 0;
      while (name.size() != previous_size) {
        previous_size = name.size();
        name = std::regex_replace(name, template_pattern, "");
      }

      // Remove namespaces, etc.
      const size_t last_colon = name.rfind(':');
      if (last_colon != std::string::npos) {
        name.replace(0, last_colon + 1, "");
      }

      return name;
    }

  }  // namespace detail

}  // namespace pretty_type
