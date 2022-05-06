//==============================================================================
/*!
//  \file
//  \brief Helper to mark a valid component within @ref cosserat_rod_component
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
#include "Extension/Systems/CosseratRods/Components/helpers/Types.hpp"
//
#include <elastica/Systems/CosseratRods/Components/helpers/Component.hpp>

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      template <typename DerivedMagnetismComponent>
      struct MagnetismComponent : public Component<DerivedMagnetismComponent> {
        // for ease in programming, ideally we shouldnt need it
        //**Type definitions****************************************************
        //! Type of magnetism component
        using magnetism_component = DerivedMagnetismComponent;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica