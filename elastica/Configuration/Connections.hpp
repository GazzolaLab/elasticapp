//==============================================================================
/*!
//  \file
//  \brief Default compile-time options for configuring Connections
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
/*!\brief Selection of the default execution type for Connection algorithms
 * \ingroup default_config
 *
 * This macro select the default execution type for Connection algorithms. The
 * following execution policies are available:
 *
 *   - elastica::configuration::sequential_policy
 *   - elastica::configuration::parallel_policy
 *   - elastica::configuration::hybrid_policy
 *
 * This can be customized if needed in the client file.
 */
#ifndef ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_CONNECTIONS
#define ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_CONNECTIONS \
  ::elastica::configuration::sequential_policy
#endif
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default registrar types for Connections
 * \ingroup default_config
 *
 * This macro records the registrar types for Connections that already have
 * support in the library, as a `tmpl::list`. While this list is NOT useful when
 * natively compiling client code, it finds use while scripting, to provide
 * runtime loading of custom connectivity data, or execution of custom
 * connectivity algorithms.
 */
#ifndef ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS
#define ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS tmpl::list<>
#endif
//******************************************************************************
