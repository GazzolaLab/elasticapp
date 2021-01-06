//==============================================================================
/*!
//  \file
//  \brief Facilities for demangling compilation symbols
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
// Includes
//******************************************************************************

#include <string>

//******************************************************************************
/*!\brief Returns the demangled name of a compilation symbol
// \ingroup UtilitiesGroup
//
// \usage
   \code
     auto demangled_name = demangle(typeid(int).name());
   \endcode
 */
std::string demangle(char const* name);
//******************************************************************************
