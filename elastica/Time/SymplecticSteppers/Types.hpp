//==============================================================================
/*!
//  \file
//  \brief Types of symplectic time-steppers
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
#include "Time/SymplecticSteppers/Algorithms/Types.hpp"
#include "Time/SymplecticSteppers/StepperTags.hpp"

namespace elastica {

  namespace timestepper {

    namespace symplectic {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <class SymplecticAlgorithm>
      class SymplecticStepper;
      /*! \endcond */
      //************************************************************************

    }  // namespace symplectic

  }  // namespace timestepper

}  // namespace elastica