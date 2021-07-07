//==============================================================================
/*!
//  \file
//  \brief Time types
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Time/SymplecticSteppers/Types.hpp"
//
#include "Utilities/DefineTypes.h"

namespace elastica {

  namespace detail {

    template <typename /* Real */>
    class SimulationTime;

    template <typename /* Real */>
    class SimulationTimeDelta;

  }  // namespace detail

  using Time = detail::SimulationTime<real_t>;
  using TimeDelta = detail::SimulationTimeDelta<real_t>;

}  // namespace elastica