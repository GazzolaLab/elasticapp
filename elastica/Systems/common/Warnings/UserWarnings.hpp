//==============================================================================
/*!
//  \file
//  \brief Warnings for Systems
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
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
      print_to(std::cerr);
      std::cerr << "\n"
                << "To disable this warning, you can set the environment "
                   "variable "
                << ENV_ELASTICA_NO_SYSTEM_WARN << " to 1\n";
    }
  }
  /*! \endcond */
  //****************************************************************************

}  // namespace elastica