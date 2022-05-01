//==============================================================================
/*!
//  \file
//  \brief ForEach in collision system
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

#include <utility>  // move
//
#include "ModuleSettings/DEMSolver.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
//
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy/Types.hpp"
//
#include "Parallel/for_each.hpp"  // uses elastica's, not GLIBCXX
//
#include "Utilities/ForceInline.hpp"

namespace elastica {

  namespace collision {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Policy class for execution of `for_each` kernel
       * \ingroup collision_system
       *
       * \details
       * ForEachOp is a policy class to determine how to iterate and apply a
       * functor `func` on a collection of systems within the \elastica
       * collision subsystem.
       *
       * \tparam ExecutionPolicyForEach_ The execution policy to be
       * followed over while iterating over a collection, see @ref execution
       */
      template <typename ExecutionPolicyForEach_>
      struct ForEachOp;
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of ForEachOp for sequential policy
       * \ingroup collision_system
       */
      template <>
      struct ForEachOp<::elastica::configuration::sequential_policy> {
        // for now, do two for_each. Later on this can be modified
        template <typename... Args>
        static ELASTICA_ALWAYS_INLINE auto for_each(Args&&... args)
        /*noexcept*/ {
          return std::for_each(std::forward<Args>(args)...);
        }
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of ForEachOp for parallel policy
       * \ingroup collision_system
       */
      template <>
      struct ForEachOp<::elastica::configuration::parallel_policy> {
        // for now, do two for_each. Later on this can be modified
        template <typename... Args>
        static ELASTICA_ALWAYS_INLINE auto for_each(Args&&... args)
        /*noexcept*/ {
          return parallel::for_each(std::forward<Args>(args)...);
        }
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of ForEachOp for hybrid policy
       * \ingroup collision_system
       */
      template <>
      struct ForEachOp<::elastica::configuration::hybrid_policy> {
        using Serial = ForEachOp<::elastica::configuration::sequential_policy>;
        using Parallel = ForEachOp<::elastica::configuration::parallel_policy>;

        // for now, do two for_each. Later on this can be modified
        template <typename InputIterator, typename Func>
        static ELASTICA_ALWAYS_INLINE auto for_each(InputIterator first,
                                                    InputIterator last, Func f)
        /*noexcept*/ {
          // static_cast for -Wsign-compare
          if (static_cast<std::size_t>(std::distance(first, last)) >
              for_each_hybrid_threshold()) {
            Parallel::for_each(first, last, std::move(f));
          } else {
            Serial::for_each(first, last, std::move(f));
          }
        }
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica