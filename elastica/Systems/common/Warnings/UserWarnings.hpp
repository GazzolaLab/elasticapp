//==============================================================================
/*!
//  \file
//  \brief Warnings for Systems
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

#include <iostream>
#include <string>

#include "ModuleSettings/Systems.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename Crit, typename PrintTo>
  void systems_warning_if(Crit crit, PrintTo print_to) {
    if (detail::systems_warnings_enabled() and crit()) {
      std::cerr << "[Systems Warning] ";
      print_to(std::cerr);
      std::cerr << "\n"
                << "To disable this warning, you can set the environment "
                   "variable "
                << ENV_ELASTICA_NO_SYSTEM_WARN << " to 1" << std::endl;
      // Flush the buffer to print the entire message when used from the python
      // side
    }
  }
  /*! \endcond */
  //****************************************************************************

}  // namespace elastica