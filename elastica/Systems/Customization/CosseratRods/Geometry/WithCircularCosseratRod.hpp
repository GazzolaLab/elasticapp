//==============================================================================
/*!
//  \file
//  \brief Primitive trait for circular cosserat rods in \elastica
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
#include "Simulator/Environments/Primitives/TypeTraits.hpp"  // refactor
#include "Simulator/Environments/Primitives/Types.hpp"  // refactor
#include "Systems/CosseratRods/Types.hpp"
//

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename Traits, typename ComputationalBlock>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::component::WithCircularCosseratRod<
            Traits, ComputationalBlock>> {
      using type = ::elastica::collision::CapsuleFromRodPrimitive;
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica