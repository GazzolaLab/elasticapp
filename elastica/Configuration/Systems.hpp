//==============================================================================
/*!
//  \file
//  \brief Default configuration of systems in the simulation
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
/*!\brief Selection of the default admissible systems
 * \ingroup default_config
 *
 * This macro selects the default admissible \systems. The purpose of this
 * \a typelist is to add tags indicating the admissible \systems in the
 * simulation that comprise the degrees of freedom. The following \system tags
 * are available:
 *
 *   - elastica::SphereCollectionTag
 *
 * All available systems are added to the list by default. This can be
 * customized as needed in the client file.
 */
#define ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS tmpl::list<>
// tmpl::list<::elastica::SphereCollectionTag>
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default policy for adding new systems to Blocks
 * within the simulator
 * \ingroup default_config
 *
 * This macro select the default Blocking policy for adding Systems within
 * \elastica. The following (simple) blocking policies are available:
 *
 *   - elastica::configuration::RestrictSizeAcrossBlockTypesPolicy
 *   - elastica::configuration::LooselyRestrictSizeAcrossBlockTypesPolicy
 *   - elastica::configuration::AlwaysNewAcrossBlockTypesPolicy
 *   - elastica::configuration::AlwaysSameAcrossBlockTypesPolicy
 *
 * This can be customized if needed in the client file. More complicated
 * policies are also possible by composition
 */
#define ELASTICA_DEFAULT_BLOCKING_POLICY_FOR_SYSTEMS \
  elastica::configuration::RestrictSizeAcrossBlockTypesPolicy
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the size parameter for the default policy choosen in
 * ELASTICA_DEFAULT_BLOCKING_CRITERIA_FOR_SYSTEMS()
 * \ingroup default_config
 *
 * This value specifies the default size parameter for specifying the blocking
 * policy.
 */
constexpr std::size_t default_blocking_policy_size() { return 1024UL; }
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default iteration behavior over multiple types of
 * \elastica systems
 * \ingroup default_config
 *
 * This macro select the default behavior while iterating over multiple types
 * of systems (such as a CosseratRod, Sphere, Capsule) etc. The following
 * execution policies are available:
 *
 *   - elastica::configuration::sequential_policy
 *   - elastica::configuration::parallel_policy
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_ITERATION_POLICY_ACROSS_SYSTEM_TYPES \
  ::elastica::configuration::sequential_policy
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default iteration behavior over a container of
 * \elastica systems
 * \ingroup default_config
 *
 * This macro select the default behavior while iterating over multiple systems
 * of the same type. The following execution policies are available:
 *
 *   - elastica::configuration::sequential_policy
 *   - elastica::configuration::parallel_policy
 *   - elastica::configuration::hybrid_policy
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_ITERATION_POLICY_FOR_EACH_SYSTEM_TYPE \
  ::elastica::configuration::sequential_policy
//******************************************************************************
