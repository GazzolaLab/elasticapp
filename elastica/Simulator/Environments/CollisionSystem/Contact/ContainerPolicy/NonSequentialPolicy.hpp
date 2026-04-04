//==============================================================================
/*!
//  \file
//  \brief Parallel and hybrid container policy
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
#include <vector>
//
#include "ModuleSettings/Parallel.hpp"
#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/concurrent_vector.h>
#endif
//
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/Types.hpp"
//

namespace elastica {

  namespace collision {

    namespace detail {

      //==========================================================================
      //
      //  CLASS DEFINITION
      //
      //==========================================================================

      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of ContactContainerPolicy for sequential policy
       * \ingroup collision_system
       */
      template <>
      struct ContactContainerPolicy<
          ::elastica::configuration::parallel_policy> {
        template <typename C>
        using type =
#if ELASTICA_TBB_PARALLEL_MODE
            tbb::concurrent_vector<C>  // cache_aligned_allocator is okay for
                                       // now
#else
            std::vector<C>
#endif
            ;
      };
      /*! \endcond */
      //**************************************************************************

      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of PairwiseCacheContainerPolicy for parallel
       * policy \ingroup collision_system
       */
      template <>
      struct ContactContainerPolicy<::elastica::configuration::hybrid_policy>
          : public ContactContainerPolicy<
                ::elastica::configuration::parallel_policy> {};
      /*! \endcond */
      //**************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
