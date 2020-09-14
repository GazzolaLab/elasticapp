//==============================================================================
/*!
//  \file
//  \brief Default configuration of the DEM solver
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
/*!\brief Selection of the normal force model for the discrete element solver.
 * \ingroup config
 *
 * The \a forceModel setting switches between different models for the
 * evaluation of the normal force within the discrete element solver. For a
 * detailed explanation of the different force models see the ForceModel class
 * description. Currently, the following models are available:
 *
 *   - LinearSpringDashpot
 *   - Hertzian
 */
#define ELASTICA_DEFAULT_DEM_RESPONSE_MODEL \
  elastica::collision::response::LinearSpringDashpot
//******************************************************************************
