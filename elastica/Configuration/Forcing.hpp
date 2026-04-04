//==============================================================================
/*!
//  \file
//  \brief Default compile-time options for configuring Forcing
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
/*!\brief Selection of the default execution type for Forcing algorithms
 * \ingroup default_config
 *
 * This macro select the default execution type for Forcing algorithms. The
 * following execution policies are available:
 *
 *   - elastica::configuration::sequential_policy
 *   - elastica::configuration::parallel_policy
 *   - elastica::configuration::hybrid_policy
 *
 * This can be customized if needed in the client file.
 */
#ifndef ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_FORCING
#define ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_FORCING \
  ::elastica::configuration::sequential_policy
#endif
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default registrar types for Forcing
 * \ingroup default_config
 *
 * This macro records the registrar types for Forcing that already have
 * support in the library, as a `tmpl::list`. While this list is NOT useful when
 * natively compiling client code, it finds use while scripting, to provide
 * runtime loading of custom forcing data, or execution of custom
 * forcing algorithms.
 */
#ifndef ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING
#define ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING tmpl::list<>
#endif
//******************************************************************************
