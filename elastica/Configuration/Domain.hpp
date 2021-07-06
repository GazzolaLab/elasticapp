//==============================================================================
/*!
//  \file
//  \brief Default compile-time options for configuring simulation domain
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
/*!\brief Selection of the default domain boundary condition in X direction
 * \ingroup default_config
 *
 * This macro select the default domain boundary condition in the X direction.
 * The following boundary types are available:
 *
 *   - elastica::IsUnbounded
 *   - elastica::IsPeriodic
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_X ::elastica::IsUnbounded
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default domain boundary condition in Y direction
 * \ingroup default_config
 *
 * This macro select the default domain boundary condition in the Y direction.
 * The following boundary types are available:
 *
 *   - elastica::IsUnbounded
 *   - elastica::IsPeriodic
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Y ::elastica::IsUnbounded
//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default domain boundary condition in Z direction
 * \ingroup default_config
 *
 * This macro select the default domain boundary condition in the Z direction.
 * The following boundary types are available:
 *
 *   - elastica::IsUnbounded
 *   - elastica::IsPeriodic
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Z ::elastica::IsUnbounded
//******************************************************************************
