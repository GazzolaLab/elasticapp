//==============================================================================
/*!
//  \file
//  \brief Default configuration of simulation environment
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
/*!\brief Selection of the default environment modules
 * \ingroup default_config
 *
 * This macro selects the default environment modules. The purpose of this
 * \a typelist is to add tags indicating the environments in the simulation. The
 * following module tags are available:
 *
 *   - elastica::environments::tags::GravityField
 *   - elastica::environments::tags::CollisionDetection
 *
 * Each module has its own customization entry points in the appropriate
 * configuration files. All available environments are added to the list
 * by default. This can be customized as needed in the client file.
 */
#ifndef ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS
#define ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS \
  tmpl::list<elastica::environments::tags::GravityField>
#endif
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default imposed primitives
 * \ingroup default_config
 *
 * This macro selects the default imposed primitives. The purpose of this
 * \a typelist is to add primitives that comprise the environment in the
 * simulation, which do not comprise the degrees of freedom (being temporally
 * integrated) in the simulator. Usually, but no necessarily, these correspond
 * to infinite objects. The following primitives are available:
 *
 *   - elastica::collision::ImposedPlanePrimitive
 *   - elastica::collision::ImposedSpherePrimitive
 *   - elastica::collision::ImposedCapsulePrimitive
 *
 * This list can be customized as needed in the client file.
 */
#ifndef ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES
#define ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES tmpl::list<>
#endif
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default execution type for Environment algorithms
 * \ingroup default_config
 *
 * This macro select the default execution type for Environment algorithms. The
 * following execution policies are available:
 *
 *   - elastica::configuration::sequential_policy
 *   - elastica::configuration::parallel_policy
 *
 * This can be customized if needed in the client file.
 */
#ifndef ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_ENVIRONMENTS
#define ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_ENVIRONMENTS \
  ::elastica::configuration::sequential_policy
#endif
//******************************************************************************
