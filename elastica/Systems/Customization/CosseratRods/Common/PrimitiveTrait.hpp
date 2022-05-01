//==============================================================================
/*!
//  \file
//  \brief Common primitive traits for cosserat rods in \elastica
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
#include "Systems/CosseratRods/Types.hpp"
#include "Systems/common/Components/TypeTraits.hpp"
//

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename RBT,
              template <typename /*RBT*/, typename /* ComputationalBlock */>
              class... Components>
    struct PrimitiveTrait<::elastica::cosserat_rod::CosseratRodPlugin<
        RBT, ::blocks::Block, Components...>>
        : public PrimitiveTrait<typename ::elastica::tt::GeometryComponentTrait<
              ::elastica::cosserat_rod::CosseratRodPlugin<
                  RBT, ::blocks::Block, Components...>>::type> {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename RBT,
              template <typename /*RBT*/, typename /* ComputationalBlock */>
              class... Components>
    struct PrimitiveTrait<
        ::blocks::Block<::elastica::cosserat_rod::CosseratRodPlugin<
            RBT, ::blocks::Block, Components...>>>
        : public PrimitiveTrait<::elastica::cosserat_rod::CosseratRodPlugin<
              RBT, ::blocks::Block, Components...>> {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename RBT,
              template <typename /*RBT*/, typename /* ComputationalBlock */>
              class... Components>
    struct PrimitiveTrait<
        ::blocks::BlockSlice<::elastica::cosserat_rod::CosseratRodPlugin<
            RBT, ::blocks::BlockSlice, Components...>>>
        : public PrimitiveTrait<::elastica::cosserat_rod::CosseratRodPlugin<
              RBT, ::blocks::Block, Components...>> {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename RBT,
              template <typename /*RBT*/, typename /* ComputationalBlock */>
              class... Components>
    struct PrimitiveTrait<
        ::blocks::ConstBlockSlice<::elastica::cosserat_rod::CosseratRodPlugin<
            RBT, ::blocks::ConstBlockSlice, Components...>>>
        : public PrimitiveTrait<::elastica::cosserat_rod::CosseratRodPlugin<
              RBT, ::blocks::Block, Components...>> {};
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica
