//==============================================================================
/*!
//  \file
//  \brief Implementation backends interfacing states with common \elastica data
//  structures
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
///
#include "Systems/States/Expressions/backends/Declarations.hpp"
///

#define ELASTICA_USE_BLAZE 1  // todo : from CMAKE

#if defined(ELASTICA_USE_BLAZE)
#include "Systems/States/Expressions/backends/blaze.hpp"
#endif  // ELASTICA_USE_BLAZE
