//==============================================================================
/*!
//  \file
//  \brief Types belonging to Name copmonents
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
#include "Systems/common/Components/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of name types in the interface
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      struct CosseratRodName;
      struct CosseratRodWithoutDampingName;

      template <typename Traits, typename Block>
      using CosseratRodNameAdapted =
          ::elastica::detail::NameAdapter<Traits, Block, CosseratRodName>;

      template <typename Traits, typename Block>
      using CosseratRodWithoutDampingNameAdapted =
          ::elastica::detail::NameAdapter<Traits, Block,
                                          CosseratRodWithoutDampingName>;
      /*! \endcond */
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
