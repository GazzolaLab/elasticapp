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

  namespace impl { // document, so don't use detail

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename /* Real */>
    class SimulationTime;
    template <typename /* Real */>
    class SimulationTimeDelta;
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //****************************************************************************
  /*!\brief The simulation time used in \elastica
   * \ingroup time
   *
   * \details
   * The time in a simulation. Times can be safely compared for equality and
   * provides more meaningful program semantics.
   */
  using Time = impl::SimulationTime<real_t>;
  //****************************************************************************

  //****************************************************************************
  /*!\brief The interval of change in time in \elastica
   * \ingroup time
   *
   * \details
   * Represents an interval of time between two Times.
   */
  using TimeDelta = impl::SimulationTimeDelta<real_t>;
  //****************************************************************************

}  // namespace elastica