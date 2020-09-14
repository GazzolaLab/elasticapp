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
