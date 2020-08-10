#pragma once

//*************************************************************************************************
/*!\brief Selection of the default environment modules
 * \ingroup config
 *
 * This macro selects the default environment modules. The purpose of this
 * typelist is to add tags indicating the environments in the simulation. The
 * following module tags are available:
 *
 *   - elastica::environments::tags::CollisionDetectionTag
 *
 * Each module has its own customization entry points in the appropriate
 * configuration files. All available environments are added to the list
 * by default. This can be customized as needed in the client file.
 */
#define ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS \
  tmpl::list<elastica::environments::tags::CollisionDetectionTag>
//*************************************************************************************************

//*************************************************************************************************
/*!\brief Selection of the default imposed primitives
 * \ingroup config
 *
 * This macro selects the default imposed primitives. The purpose of this
 * typelist is to add primitives that comprise the environment in the
 * simulation, which do not comprise the degrees of freedom in the
 * simulator. Usually, but no necessarily, these correspond to infinite
 * objects. The following primitives are avaiable:
 *
 *   - elastica::collision::PlanePrimitive
 *   - elastica::collision::SpherePrimitive
 *
 * Only the plane primitive is added to the list by default.
 * This list can be customized as needed in the client file.
 */
#define ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES \
  tmpl::list<elastica::collision::PlanePrimitive>
//*************************************************************************************************
