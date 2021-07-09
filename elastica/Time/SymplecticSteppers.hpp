//==============================================================================
/*!
//  \file
//  \brief All symplectic steppers in \elastica
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

#include "Time/Helpers.hpp"
///
#include "Time/SymplecticSteppers/Types.hpp"
///
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/StepperTags.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"

namespace elastica {

  namespace timestepper {

    //**************************************************************************
    /*!\brief Symplectic time-steppers and methods
     * \ingroup time
     */
    namespace symplectic {}
    //**************************************************************************

    /// Promote namespaces
    using symplectic::PositionVerlet;
    using symplectic::SymplecticStepper;
    using symplectic::VelocityVerlet;

  }  // namespace timestepper

}  // namespace elastica