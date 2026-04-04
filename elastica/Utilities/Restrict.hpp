//==============================================================================
/*!
//  \file
//  \brief Declaration of the restrict macro in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <blaze/system/Restrict.h>

//******************************************************************************
/*!\brief Declares a macro for restriction of pointers and references
 * \ingroup utils
 *
 * \details
 * For some reason, clang also uses declares __GNUC__ so blaze's Restrict
 * mechanism works. Here we just alias it
 */
#define ELASTICA_RESTRICT BLAZE_RESTRICT
//******************************************************************************
