//==============================================================================
/*!
//  \file
//  \brief Customization of primitive trait
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
#include "Simulator/Environments/Primitives/TypeTraits.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
#include "Systems/RigidBody/Sphere/Types.hpp"  // we only need the type, but this includes everything

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename T, typename B>
    struct PrimitiveTrait<::elastica::rigid_body::WithSphereGeometry<T, B>> {
      using type = ::elastica::collision::SpherePrimitive;
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica
