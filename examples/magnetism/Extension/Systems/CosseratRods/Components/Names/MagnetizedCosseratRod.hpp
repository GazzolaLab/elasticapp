//==============================================================================
/*!
//  \file
//  \brief MagnetizedCosserat rod name
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

#include <elastica/Systems/common/Components/NameAdapter.hpp>
//
#include <string>

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Provides a name to MagnetizedCosseratRod
       * \ingroup cosserat_rod_component
       */
      struct MagnetizedCosseratRodName {
       public:
        //**********************************************************************
        /*!\brief Human-readable name of the current plugin and all derivates
         *
         * \note This is intended to work with pretty_type::name<>
         */
        static std::string name() { return "MagnetizedCosseratRod"; }
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica