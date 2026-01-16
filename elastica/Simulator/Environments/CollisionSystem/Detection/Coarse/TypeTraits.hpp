//==============================================================================
/*!
//  \file
//  \brief Type traits for collision detection algorithms
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>
//
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //**********************************************************************
        /*!\brief Namespace for coarse detection type traits
        // \ingroup coarse_collision_detection
        */
        namespace tt {}
        //**********************************************************************

        namespace tt {

          //********************************************************************
          /*!\name Tags*/
          //@{

          //********************************************************************
          /*!\brief Tag to mark same primitive types
           * \ingroup coarse_collision_detection
           */
          struct SamePrimitivesTag : public std::true_type {
          };  // needs to have bool
          //********************************************************************

          //********************************************************************
          /*!\brief Tag to mark different primitive types
           * \ingroup coarse_collision_detection
           */
          struct DifferentPrimitivesTag : public std::false_type {};
          //********************************************************************

          //@}
          //********************************************************************

          namespace detail {
            // need this to name, else use primitive
            static constexpr auto downcast(std::true_type) -> SamePrimitivesTag;
            static constexpr auto downcast(std::false_type)
                -> DifferentPrimitivesTag;
          }  // namespace detail

          //********************************************************************
          /*!\brief Compile time dispatch of primitives
           * \ingroup coarse_collision_detection
           *
           * \details
           * DispatchBasedOnPrimitiveTypes determines if the given pair
           * are the same primitives.
           *
           * \semantics
           * In case they are the same primitives, it evaluates to
           * SamePrimitivesTag. Otherwise it evaluates to DifferentPrimitivesTag
           *
           * \usage
           * For the types `F`, `S`
           * \code
           * using result = tt::IsSamePrimitives<F, S>;
           * \endcode
           *
           * \metareturns
           * either `SamePrimitivesTag` if the pair has the same types else
           * returns `DifferentPrimitivesTag`
           *
           * \example
           * \snippet Coarse/Test_TypeTraits.cpp dispatch_eg
           *
           * \tparam FCP The first colliding primitive
           * \tparam SCP The second colliding primitive
           */
          template <typename FCP, typename SCP>
          struct IsSamePrimitive
              : public decltype(detail::downcast(std::is_same<FCP, SCP>{})) {};
          //********************************************************************

          //********************************************************************
          /// \see IsSamePrimitive
          template <typename FCP, typename SCP>
          constexpr bool IsSamePrimitive_v = IsSamePrimitive<FCP, SCP>::value;
          //********************************************************************

        }  // namespace tt

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica

/*Helps extract the second primitives from a matrix of primitives
 *  using FCP = int;
    using PrimitivePrimitiveMatrix =
        tmpl::list<tmpl::list<int, float>, tmpl::list<int, char>,
                   tmpl::list<char, float>, tmpl::list<char, double>>;

using ListOfSecondPrimitives =
    tmpl::fold<PrimitivePrimitiveMatrix, tmpl::list<>,
               SecondPrimtive<FCP>::Extractor<tmpl::_state,
                                              tmpl::_element>>;

ListOfSecondPrimitives <=> tmpl::list<float, char>;
*/
// template <
//     typename FirstPrimitiveType>  // Primitive whose second
//     primitives
//                                   // are to be extracted
//                                   // from a typelist
// struct ExtractSecondPrimitive {
//   template <typename S, typename E>
//   struct apply;

//   template <typename S, typename FP, typename SP>
//   struct apply<S, tmpl::list<FP, SP>> {
//     using type = typename std::conditional<
//         std::is_same<FirstPrimitiveType, FP>::value,
//         tmpl::push_back<S, SP>, S>::type;
//   };
// };
