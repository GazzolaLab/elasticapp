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
 * \ingroup default_collision_config
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

//******************************************************************************
/*!\brief The threshold determining hybrid execution of `for_each` in DEMSolver
 * \ingroup default_collision_config
 *
 * \details
 * The value returned determines the behavior of the hybrid execution model of
 * `for_each` used inside the DEMSolver. If less than the current threshold,
 * `for_each` is performed in sequence, else it is performed in parallel. See
 * \ref execution for more details.
 */
constexpr std::size_t for_each_hybrid_threshold() { return 32UL; }
//******************************************************************************

//******************************************************************************
/*!\brief The threshold determining hybrid execution of `for_each_type` in
 * DEMSolver
 * \ingroup default_collision_config
 *
 * \details
 * The value returned determines the behavior of the hybrid execution model of
 * `for_each_type` used inside the DEMSolver. If less than the current
 * threshold, `for_each_type` is performed in sequence, else it is performed in
 * parallel. See \ref execution for more details.
 */
constexpr std::size_t for_each_type_hybrid_threshold() { return 2UL; }
//******************************************************************************

//******************************************************************************
/*!\brief The threshold determining hybrid execution of `pairwise_for_each` in
 * DEMSolver
 * \ingroup default_collision_config
 *
 * \details
 * The value returned determines the behavior of the hybrid execution model of
 * `pairwise_for_each` used inside the DEMSolver. If less than the current
 * threshold, execution of kernels in `pairwise_for_each` is performed in
 * sequence, else it can be performed in parallel. See \ref execution for more
 * details.
 */
constexpr std::size_t pairwise_for_each_hybrid_threshold() { return 4UL; }
//******************************************************************************
