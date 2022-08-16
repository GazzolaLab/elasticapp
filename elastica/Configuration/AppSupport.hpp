//==============================================================================
/*!
//  \file
//  \brief Configuration of \elastica applications
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\brief Environment variable to control \elastica splash messages
 * \ingroup systems
 *
 * The macro defines the environment variable that is searched for controlling
 * the behavior of splash messages in Elastica++.
 * By default, in case-studies and applications, a splash message is always
 * enabled to give context to the user. To disable splash messages, the
 * the environment variable can be set to 1, i.e.
 * \code
 * ELASTICA_NO_SPLASH=1 ./<your_application> [options]
 * \endcode
 */
#define ENV_ELASTICA_NO_SPLASH "ELASTICA_NO_SPLASH"
//****************************************************************************
