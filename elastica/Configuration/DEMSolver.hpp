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
