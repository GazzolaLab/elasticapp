//==============================================================================
/*!
//  \file
//  \brief Default configuration of simulation domain
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
/*!\brief Selection of the default domain boundary conditions
 * \ingroup config
 *
 * This macro select the default domain boundary condition in the
 * X direction. The following boundary types are available:
 *
 *   - elastica::UnboundedBoundary
 *   - elastica::PeriodicBoundary
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_X elastica::UnboundedBoundary

//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default domain boundary conditions
 * \ingroup config
 *
 * This macro select the default domain boundary condition in the
 * Y direction. The following boundary types are available:
 *
 *   - elastica::UnboundedBoundary
 *   - elastica::PeriodicBoundary
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Y elastica::UnboundedBoundary

//******************************************************************************

//******************************************************************************
/*!\brief Selection of the default domain boundary conditions
 * \ingroup config
 *
 * This macro select the default domain boundary condition in the
 * Z direction. The following boundary types are available:
 *
 *
 *   - elastica::UnboundedBoundary
 *   - elastica::PeriodicBoundary
 *
 * This can be customized if needed in the client file.
 */
#define ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Z elastica::UnboundedBoundary

//******************************************************************************
