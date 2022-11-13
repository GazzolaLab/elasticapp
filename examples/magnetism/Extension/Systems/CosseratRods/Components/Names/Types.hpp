//==============================================================================
/*!
//  \file
//  \brief Types belonging to Name copmonents
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
#include <elastica/Systems/common/Components/Types.hpp>

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
      struct MagnetizedCosseratRodName;
      struct MagnetizedCosseratRodWithoutDampingName;

      template <typename Traits, typename Block>
      using MagnetizedCosseratRodNameAdapted =
          ::elastica::detail::NameAdapter<Traits, Block,
                                          MagnetizedCosseratRodName>;

      template <typename Traits, typename Block>
      using MagnetizedCosseratRodWithoutDampingNameAdapted =
          ::elastica::detail::NameAdapter<
              Traits, Block, MagnetizedCosseratRodWithoutDampingName>;
      /*! \endcond */
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
