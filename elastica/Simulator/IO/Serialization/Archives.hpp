//==============================================================================
/*!
//  \file
//  \brief Inclusion of all archives
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
/// Types first
#include "Simulator/IO/Serialization/Archives/Types.hpp"
///
#include "Simulator/IO/Serialization/Archives/Binary.hpp"
#if defined(ELASTICA_USE_YAMLCPP)
#include "Simulator/IO/Serialization/Archives/YAML.hpp"
#endif  // defined(ELASTICA_USE_YAMLCPP)