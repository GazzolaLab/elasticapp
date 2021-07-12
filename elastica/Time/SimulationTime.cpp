//==============================================================================
/*!
//  \file
//  \brief Instantiations of Time and TimeDelta
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Time/SimulationTime.hpp"
#include "Utilities/DefineTypes.h"

namespace elastica {

  //============================================================================
  //
  //  EXPLICIT INSTANTIATIONS
  //
  //============================================================================

  /*
  // Explicitly instantiate only the classes we want to be defined.
  https://stackoverflow.com/questions/2351148/explicit-template-instantiation-when-is-it-used
  // Sad but true :
  https://stackoverflow.com/questions/11705407/explicit-template-instantiation-through-a-typedef
  */

  template class impl::SimulationTime<real_t>;
  template class impl::SimulationTimeDelta<real_t>;

}  // namespace elastica
