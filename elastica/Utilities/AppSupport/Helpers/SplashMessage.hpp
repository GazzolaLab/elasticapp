//==============================================================================
/*!
//  \file
//  \brief Splash messages in applications
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
#include <cstdlib>

#include "Configuration/AppSupport.hpp"

namespace elastica {

  namespace detail {

    inline bool splash_message_enabled() /* noexcept*/ {
      // if set to 0 also return true
      if (const char* env_v = std::getenv(ENV_ELASTICA_NO_SPLASH))
        return not bool(env_v[0]);
      else
        return true;
    }

  }  // namespace detail

}  // namespace elastica