#pragma once

//******************************************************************************
/*!\brief Selection of the default admissible systems
 * \ingroup config
 *
 * This macro selects the default admissible systems. The purpose of this
 * typelist is to add tags indicating the admissible systems in the simulation
 * that comprise the degrees of freedom. The following system tags are
 * available:
 *
 *   - elastica::SphereCollectionTag
 *
 * All available systems are added to the list by default. This can be
 * customized as needed in the client file.
 */
#define ELASTICA_DEFAULT_ADMISSIBLE_SYSTEM_TAGS \
  tmpl::list<::elastica::SphereCollectionTag>
//******************************************************************************
