//==============================================================================
/*!
//  \file
//  \brief All symplectic steppers in \elastica
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

    //**************************************************************************
    // promote from symplectic namespace for cleaner interface
    using symplectic::SymplecticStepper;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Position Verlet algorithm
     * \ingroup time
     */
    using PositionVerlet =
        symplectic::SymplecticStepper<symplectic::PositionVerlet>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Velocity Verlet algorithm
     * \ingroup time
     */
    using VelocityVerlet =
        symplectic::SymplecticStepper<symplectic::VelocityVerlet>;
    //**************************************************************************

  }  // namespace timestepper

}  // namespace elastica