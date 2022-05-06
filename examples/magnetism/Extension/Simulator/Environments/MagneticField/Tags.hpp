//==============================================================================
/*!
//  \file
//  \brief Tag to create a uniform magnetic field over all systems
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <elastica/Simulator/Environments/Protocols.hpp>
#include <elastica/Utilities/ProtocolHelpers.hpp>
#include <utility>  // false type

#include "Extension/Simulator/Environments/MagneticField/Types.hpp"

namespace elastica {

  namespace environments {

    namespace tags {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Tag for marking a uniform magnetic field over systems
       * \ingroup environments
       */
      struct MagneticFieldTag
          : public ::tt::ConformsTo<
                ::elastica::environments::protocols::EnvironmentTag> {
        //! Does environment require primitives?
        using requires_primitives = std::false_type;

        template <typename Traits>
        using type = MagneticField<Traits>;
      };
      //************************************************************************

    }  // namespace tags

  }  // namespace environments

}  // namespace elastica
