//==============================================================================
/*!
//  \file
//  \brief Inclusion of all environment primitives
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

// Forward declarations come first
#include "Simulator/Environments/Primitives/Aliases.hpp"
#include "Simulator/Environments/Primitives/TypeTraits.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"
//
#include "Simulator/Environments/Primitives/CapsuleFromRodPrimitive.hpp"
#include "Simulator/Environments/Primitives/CapsulePrimitive.hpp"
#include "Simulator/Environments/Primitives/CylinderPrimitive.hpp"
#include "Simulator/Environments/Primitives/HalfSpacePrimitive.hpp"
#include "Simulator/Environments/Primitives/ImposedPrimitiveAdapter.hpp"
#include "Simulator/Environments/Primitives/PlanePrimitive.hpp"
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup primitives Rigid-body primitives
 * \ingroup environments
 * \brief Implementation of rigid-body geometrical primitives for use in
 * collision detection and other environmental forces
 */
//******************************************************************************
