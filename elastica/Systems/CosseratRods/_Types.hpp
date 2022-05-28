//==============================================================================
/*!
//  \file
//  \brief All types belonging to @ref cosserat_rod
//  \details This header is "protected" for exposing CosseratRodAliases as types
//  to the outside world
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include "Systems/Block/Types.hpp"
#include "Systems/CosseratRods/Components/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of cosserat rod customization types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename /*DataType*/, typename /*PlacementTraits*/,
              typename /*AllocatorTraits*/>
    struct CosseratRodTraits;

    struct DefaultCosseratRodTraits;

    template <typename /*CRT*/,
              template <typename /*Plugin*/> class /*ComputationalBlock*/,
              template <typename /*CRT*/, typename /*Instantiated Block*/>
              class... /*Components*/>
    class CosseratRodPlugin;

    template <typename /*CRT*/,
              template <typename /*Plugin*/> class /*ComputationalBlock*/,
              typename /*Tag*/,
              template <typename /*CRT*/, typename /*Instantiated Block*/>
              class... /*Components*/>
    class TaggedCosseratRodPlugin;

    template <class /*Plugin*/, class... /*Initializers*/>
    struct CosseratInitializer;
    /*! \endcond */
    //**************************************************************************

  }  // namespace cosserat_rod

  namespace cosserat_rod {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of rod types
    //
    ////////////////////////////////////////////////////////////////////////////

  }  // namespace cosserat_rod

}  // namespace elastica
