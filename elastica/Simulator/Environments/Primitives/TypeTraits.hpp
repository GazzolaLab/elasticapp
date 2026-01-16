//==============================================================================
/*!
//  \file
//  \brief Type traits for primitives
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <type_traits>
//
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*!\brief Customization point of primitives in \elastica
     * \ingroup customization
     *
     * \details
     * The PrimitiveTraitCustomization type trait is a customization point for
     * obtaining the primitive trait for any type `T` in \elastica. It enables
     * evaluation of the primitive trait of `T`
     *
     * \usage
     * For any type `T`, one can specialize PrimitiveTrait and provide a nested
     * `type` typedef, as shown below.
     *
     * \example
     * \snippet Primitives/Test_TypeTraits.cpp primitive_trait_custom_eg
     *
     * \see PrimitiveTrait
     * \tparam T Type to customize
     */
    template <typename T>
    struct PrimitiveTrait;
    //**************************************************************************

  }  // namespace customization

  namespace collision {

    namespace tt {

      //************************************************************************
      /*!\brief Check if type `T` is a \elastica rigid body primitive
       * \ingroup primitives
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
       * For the type `F`
       * \code
       * using result = ::elastica::collision::tt::IsRigidPrimitive<F>;
       * \endcode
       *
       * \metareturns
       * std::bool_constant
       *
       * \semantics
       * If the type `F` is derived from `RigidBodyPrimitive`, then
       * \code
       * typename result::type = std::true_type;
       * \endcode
       * otherwise
       * \code
       * typename result::type = std::false_type;
       * \endcode
       *
       * \example
       * \snippet Primitives/Test_TypeTraits.cpp is_rigid_primitive_eg
       *
       * \tparam T The type to check
       */
      template <typename T>
      struct IsRigidPrimitive : std::is_base_of<RigidBodyPrimitive, T> {};
      //************************************************************************

      //************************************************************************
      /// \see elastica::collision::tt::IsRigidPrimitive
      /// \ingroup primitives
      template <typename T>
      constexpr bool IsRigidPrimitive_v = IsRigidPrimitive<T>::value;
      //************************************************************************

      namespace detail {
        template <typename T, typename = typename IsRigidPrimitive<T>::type>
        struct PrimitiveTraitEval;

        // if rigid body, use PrimitiveType
        template <typename R>
        struct PrimitiveTraitEval<GeomPrimitive, R> {};

        template <typename T>
        struct PrimitiveTraitEval<T, std::true_type> {
          using type = T;
        };

        template <typename T>
        struct PrimitiveTraitEval<ImposedPrimitiveAdapter<T>, std::true_type> {
          using type = T;
        };

        template <typename T>
        struct PrimitiveTraitEval<T, std::false_type>
            : public ::elastica::customization::PrimitiveTrait<T> {};

      }  // namespace detail

      //************************************************************************
      /*!\brief Get primitive type of type `T`
       * \ingroup primitives
       *
       * \details
       * The PrimitiveTrait type trait has a nested member `type` when it is
       * either
       * 1. an internal \elastica primitive type
       * 2. or customized using ::elastica::customization::PrimitiveTrait
       *
       * \usage
       * For any type `T`
       * \code
       * using result = elastica::collision::tt::PrimitiveTrait<T>;
       * \endcode
       * \metareturns
       * the type `T`
       *
       * \example
       * \snippet Primitives/Test_TypeTraits.cpp primitive_trait_eg
       *
       * \tparam T The type to obtain primitive from
       */
      template <typename T>
      struct PrimitiveTrait
          : public detail::PrimitiveTraitEval<std::remove_cvref_t<T>> {};
      //************************************************************************

      //************************************************************************
      /// \see elastica::collision::tt::PrimitiveTrait
      /// \ingroup primitives
      template <typename T>
      using PrimitiveTrait_t = typename PrimitiveTrait<T>::type;
      //************************************************************************

    }  // namespace tt

  }  // namespace collision

}  // namespace elastica
