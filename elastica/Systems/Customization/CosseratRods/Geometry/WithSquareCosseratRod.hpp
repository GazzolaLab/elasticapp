//==============================================================================
/*!
//  \file
//  \brief Primitive trait for square cosserat rods in \elastica
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
#include "Simulator/Environments/Primitives/TypeTraits.hpp"  // refactor
#include "Simulator/Environments/Primitives/Types.hpp"       // refactor
#include "Systems/CosseratRods/Types.hpp"
#include "Utilities/NoSuchType.hpp"
//

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename Traits, typename ComputationalBlock>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::component::WithSquareCosseratRod<
            Traits, ComputationalBlock>> {
      // TODO : future work
      using type = NoSuchType;
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica