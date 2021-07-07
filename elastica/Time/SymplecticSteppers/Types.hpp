//==============================================================================
/*!
//  \file
//  \brief Types of symplectic time-steppers
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