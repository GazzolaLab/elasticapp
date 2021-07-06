//==============================================================================
/*!
//  \file
//  \brief Serialization component and documentation
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

// Types first
#include "Simulator/IO/Serialization/Types.hpp"
///
#include "Simulator/IO/Serialization/Serialization.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/SerializeWizard.hpp"
///
#include "Simulator/IO/Serialization/Archives.hpp"
///
#include "Simulator/IO/Serialization/STL.hpp"
#include "Simulator/IO/Serialization/Custom.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup serialization Serialization
 * \ingroup simulator
 * \brief Machinery to serialize and deserialize data
 *
 * \details
 * The serialization module component implements machinery to serialize and
 * deserialize types in \elastica, mainly for checkpointing.
 */
//******************************************************************************
