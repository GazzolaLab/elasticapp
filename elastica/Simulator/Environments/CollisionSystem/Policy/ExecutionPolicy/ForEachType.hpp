//==============================================================================
/*!
//  \file
//  \brief ForEach type in collision system
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
#include "Utilities/ForceInline.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Policy class for execution of `for_each_type` kernel
       * \ingroup collision_system
       *
       * \details
       * ForEachTypeOp is a policy class to determine how to iterate and apply a
       * functor `func` on a collection of types within \elastica
       * collision subsystem.
       *
       * \tparam ExecutionPolicyOverTypes_ The execution policy to be
       * followed over while iterating over a collection of pairs, see @ref
       * execution
       */
      template <typename ExecutionPolicyOverTypes_>
      struct ForEachTypeOp;
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization for sequential policy
       * \ingroup collision_system
       */
      template <>
      struct ForEachTypeOp<::elastica::configuration::sequential_policy> {
        template <typename List, typename Functor>
        static ELASTICA_ALWAYS_INLINE void for_each_type(
            Functor f) /*noexcept*/ {
          tmpl::for_each<List>(std::move(f));
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
      struct ForEachTypeOp<::elastica::configuration::parallel_policy> {
       protected:
        template <template <class...> class List, typename... Elements,
                  typename Functor>
        static ELASTICA_ALWAYS_INLINE void for_each_type_args(
            List<Elements...>&&, Functor f) /*noexcept*/ {
          // f is copied here as opposed to being refed, to maintain
          // invariance
          parallel::invoke([f]() { f(tmpl::type_<Elements>{}); }...);
        }

        // TBB restricts ther number of functions to at least two when
        // parallel invoke is involved : to WAR this, we just do a serial
        // invoke when number of elements is 1
        template <template <class...> class List, typename Element,
                  typename Functor>
        static ELASTICA_ALWAYS_INLINE void for_each_type_args(
            List<Element>&&, Functor f) /*noexcept*/ {
          f(tmpl::type_<Element>{});
        }

       public:
        template <typename List, typename Functor>
        static ELASTICA_ALWAYS_INLINE void for_each_type(
            Functor f) /*noexcept*/ {
          for_each_type_args(List{}, std::move(f));
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
      struct ForEachTypeOp<::elastica::configuration::hybrid_policy> {
        using Serial =
            ForEachTypeOp<::elastica::configuration::sequential_policy>;
        using Parallel =
            ForEachTypeOp<::elastica::configuration::parallel_policy>;

        // for now, do two for_each. Later on this can be modified
        template <typename List, typename Func>
        static ELASTICA_ALWAYS_INLINE auto for_each_type(Func f)
        /*noexcept*/ {
          constexpr std::size_t n_types = tmpl::size<List>::value;
          if (n_types > for_each_type_hybrid_threshold()) {
            Parallel::for_each_type<List>(std::move(f));
          } else {
            Serial::for_each_type<List>(std::move(f));
          }
        }
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
