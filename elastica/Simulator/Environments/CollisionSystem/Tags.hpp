//==============================================================================
/*!
//  \file
//  \brief Tag to create a collision engine
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
#include <utility>  // true type

#include "Simulator/Environments/CollisionSystem/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "Simulator/Environments/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace environments {

    namespace tags {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Tag for marking a collision engine over systems
       * \ingroup environments
       */
      struct CollisionSystem
          : public ::tt::ConformsTo<
                ::elastica::environments::protocols::EnvironmentTag> {
        //! Does environment need primitives?
        using requires_primitives = std::true_type;

        template <typename SimulatorConfig>
        using type = elastica::collision::CollisionSystem<
            SimulatorConfig, elastica::collision::CollisionConfig>;
      };
      //************************************************************************

      //**Type definitions******************************************************
      using CollisionSystemTag [[deprecated(R"error(
       tags::CollisionSystemTag is deprecated and will be removed in a future
       release. Use tags::CollisionSystem instead. )error")]] = CollisionSystem;
      //************************************************************************

    }  // namespace tags

  }  // namespace environments

}  // namespace elastica