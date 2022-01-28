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

#include "Simulator/Interface/Affordances/detail/EmptyBatchingAlgorithm.hpp"
#include "Simulator/Interface/Affordances/detail/EmptyFinalize.hpp"
#include "Simulator/Interface/Affordances/detail/EmptyResetModule.hpp"
#include "Simulator/Interface/Affordances/detail/EmptyUpdateSteps.hpp"

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

    //**************************************************************************
    /*!\brief Module interface with all required (meta)functions left empty
     * \ingroup modules
     *
     * \details
     * For quick assembly of affordance modules, we have an empty interface
     *
     * \example
     * \code
     * template <class SimTraits, class Sim>
     * class EgAffordance : public CRTPHelper<Sim, EgAffordance>,
     *                                 public EmptyInterface<SimTraits>,
     *                                 public NonCopyable {};
     * \endcode
     */
    template <typename SimTraits>
    using EmptyInterface = typename detail::inherit<
        SimTraits, detail::EmptyUpdateSteps, detail::EmptyBatchingAlgorithm,
        detail::EmptyFinalize, detail::EmptyResetModule>::type;
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica