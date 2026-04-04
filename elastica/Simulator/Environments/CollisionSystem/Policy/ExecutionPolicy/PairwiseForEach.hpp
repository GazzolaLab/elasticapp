//==============================================================================
/*!
//  \file
//  \brief Pairwise ForEach in collision system
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

#include <utility>  // move
//
#include "ModuleSettings/DEMSolver.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
//
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy/Types.hpp"
///
#include "Parallel/invoke.hpp"  // uses elastica's, not GLIBCXX
///
#include "Simulator/Environments/CollisionSystem/Detection/PairwiseTMPL.hpp"
///
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
      /*!\brief Policy class for execution of `pairwise_for_each` kernel
       * \ingroup collision_system
       *
       * \details
       * OverPairsOp is a policy class to determine how to iterate and apply a
       * functor `func` on a collection of pairs within \elastica
       * collision subsystem. It finds application in applying a functor to a
       * pair of colliding types.
       *
       * \tparam ExecutionPolicyOverPairs_ The execution policy to be
       * followed over while iterating over a collection of pairs, see @ref
       * execution
       */
      template <typename ExecutionPolicyOverPairs_>
      struct OverPairsOp;
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization for sequential policy
       * \ingroup collision_system
       */
      template <>
      struct OverPairsOp<::elastica::configuration::sequential_policy> {
        template <typename List, typename Functor>
        static ELASTICA_ALWAYS_INLINE void pairwise_for_each(
            Functor f) /*noexcept*/ {
          elastica::pairwise_for_each<List>(std::move(f));
        }
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization for parallel policy
       * \ingroup collision_system
       */
      template <>
      struct OverPairsOp<::elastica::configuration::parallel_policy> {
       protected:
        template <template <class...> class List, typename... Elements,
                  typename Functor>
        static ELASTICA_ALWAYS_INLINE void pairwise_for_each_args(
            List<Elements...>&&, Functor f) /*noexcept*/ {
          // f is copied here as opposed to being refed, to maintain
          // invariance
          parallel::invoke([f]() {
            f(tmpl::type_<typename Elements::first_type>{},
              tmpl::type_<typename Elements::second_type>{});
          }...);
        }

       public:
        template <typename List, typename Functor>
        static ELASTICA_ALWAYS_INLINE void pairwise_for_each(
            Functor f) /*noexcept*/ {
          pairwise_for_each_args(List{}, std::move(f));
        }
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization for hybrid policy
       * \ingroup collision_system
       */
      template <>
      struct OverPairsOp<::elastica::configuration::hybrid_policy> {
        using Serial =
            OverPairsOp<::elastica::configuration::sequential_policy>;
        using Parallel =
            OverPairsOp<::elastica::configuration::parallel_policy>;

        // for now, do two for_each. Later on this can be modified
        template <typename List, typename Func>
        static ELASTICA_ALWAYS_INLINE auto pairwise_for_each(Func f)
        /*noexcept*/ {
          constexpr std::size_t n_pairs = tmpl::size<List>::value;
          if (n_pairs > pairwise_for_each_hybrid_threshold()) {
            Parallel::pairwise_for_each<List>(std::move(f));
          } else {
            Serial::pairwise_for_each<List>(std::move(f));
          }
        }
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
