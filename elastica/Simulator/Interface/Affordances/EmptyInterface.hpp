//==============================================================================
/*!
//  \file
//  \brief An all empty interface to use with affordance modules
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include "Simulator/Interface/Affordances/EmptyBatchingAlgorithm.hpp"
#include "Simulator/Interface/Affordances/EmptyFinalize.hpp"
#include "Simulator/Interface/Affordances/EmptyResetModule.hpp"
#include "Simulator/Interface/Affordances/EmptyUpdateSteps.hpp"

namespace elastica {

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename SimTraits, template <typename> class... Affordances>
      struct inherit {
        // needs a name for [-Wnon-c-typedef-for-linkage]
        using type = struct foo : public Affordances<SimTraits>... {};
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    template <typename SimTraits>
    using EmptyInterface = typename detail::inherit<
        SimTraits, detail::EmptyUpdateSteps, detail::EmptyBatchingAlgorithm,
        detail::EmptyFinalize, detail::EmptyResetModule>::type;

  }  // namespace modules

}  // namespace elastica