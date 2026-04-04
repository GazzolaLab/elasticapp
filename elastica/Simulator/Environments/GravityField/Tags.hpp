//==============================================================================
/*!
//  \file
//  \brief Tag to create a uniform gravity field over all systems
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <utility>  // false type

#include "Simulator/Environments/GravityField/Types.hpp"
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
      /*!\brief Tag for marking a uniform gravity field over systems
       * \ingroup environments
       */
      struct GravityField
          : public ::tt::ConformsTo<
                ::elastica::environments::protocols::EnvironmentTag> {
        //! Does environment require primitives?
        using requires_primitives = std::false_type;

        template <typename Traits>
        using type = ::elastica::environments::GravityField<Traits>;
      };
      //************************************************************************

      //**Type definitions******************************************************
      using GravityTag [[deprecated(R"error(
       tags::GravityTag is deprecated and will be removed in a future
       release. Use tags::GravityField instead. )error")]] = GravityField;
      //************************************************************************

    }  // namespace tags

  }  // namespace environments

}  // namespace elastica