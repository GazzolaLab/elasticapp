//==============================================================================
/*!
//  \file
//  \brief Type traits for Transfer
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
#include <utility>

#include "TransferTraits.hpp"
#include "Types.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    namespace tt {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Compile time check for valid default System transfer operations
       * \ingroup transfer
       *
       * IsSystemDefaultTransferable type trait class tests whether or not
       * the given \system type \c T can have its relevant information
       * transferred by default, without user specialization, to the \elastica
       * collision detection module. In order to qualify to be
       * \a DefaultTransferable the given type \c T needs to have certain public
       * member functions listed below. In case the given type \c T is
       * \a DefaultTransferable, the \a value member constant is set to \a true
       * , the nested type definition \a type is \a std::true_type, and the
       * class derives from \a std::true_type. Otherwise \a value is set to
       * \a false, \a type is \a std::false_type, and the class derives
       * from \a std::false_type.
       *
       * \usage
       * For any \system type \c T,
       * \code
       * using result = IsSystemDefaultTransferable<T>;
       * \endcode
       *
       * \metareturns
       * cpp17::bool_constant
       *
       * \semantics
       * If the type \c T has the following signature
       * \snippet this expected_system_signatures
       * then
       * \code
       * typename result::type = std::true_type;
       * \endcode
       * otherwise
       * \code
       * typename result::type = std::false_type;
       * \endcode
       *
       * \example
       * \snippet Transfer/Test_TypeTraits.cpp IsSystemDefaultTransferable_eg
       *
       * \tparam T the \system type to check
       */
      template <typename T,                  // SystemType
                typename = cpp17::void_t<>>  // Restricting condition
      struct IsSystemDefaultTransferable : std::false_type {};
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of IsSystemDefaultTransferable for default
       * transferable types.
       * \ingroup transfer
       */
      template <typename T>  // SystemType
      struct IsSystemDefaultTransferable<
          T, cpp17::void_t<
                 /// [expected_system_signatures]
                 decltype(std::declval<T const&>().get_dimensions(
                     std::declval<std::size_t>())),
                 decltype(std::declval<T const&>().get_position(
                     std::declval<std::size_t>())),
                 decltype(std::declval<T const&>().get_director(
                     std::declval<std::size_t>())),
                 decltype(std::declval<T const&>().get_velocity(
                     std::declval<std::size_t>())),
                 decltype(std::declval<T const&>().get_angular_velocity(
                     std::declval<std::size_t>())),
                 decltype(std::declval<T const&>().get_force(
                     std::declval<std::size_t>())),
                 decltype(std::declval<T const&>().get_torque(
                     std::declval<std::size_t>()))>
          /// [expected_system_signatures]
          > : std::true_type {};
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*!\brief Compile time check for valid default Primitive transfer
       * operations
       * \ingroup transfer type_traits
       *
       * IsPrimitiveDefaultTransferable type trait class tests whether or not
       * the given \primitive type \c T can have its relevant information
       * transferred by default, without user specialization, from the \elastica
       * collision detection module. In order to qualify to be
       * \a DefaultTransferable the given type \c T needs to be a geometrical
       * primitive and have certain public
       * member functions listed below. In case the given type \c T is
       * \a DefaultTransferable, the \a value member constant is set to \a true
       * , the nested type definition \a type is \a std::true_type, and the
       * class derives from \a std::true_type. Otherwise \a value is set to
       * \a false, \a type is \a std::false_type, and the class derives
       * from \a std::false_type.
       *
       * \usage
       * For any \primitive type \c T,
       * \code
       * using result = IsPrimitiveDefaultTransferable<T>;
       * \endcode
       *
       * \metareturns
       * cpp17::bool_constant
       *
       * \semantics
       * If the type \c T is a rigid primitive and has the following member
       * functions
       * \snippet this primitive_transfer_signature
       * then
       * \code
       * typename result::type = std::true_type;
       * \endcode
       * otherwise
       * \code
       * typename result::type = std::false_type;
       * \endcode
       *
       * \example
       * \snippet Transfer/Test_TypeTraits.cpp IsPrimitiveDefaultTransferable_eg
       *
       * \tparam T the \primitive type to check
       */
      template <typename T,                  // PrimitiveType
                typename = cpp17::void_t<>>  // Restricting condition
      struct IsPrimitiveDefaultTransferable : std::false_type {};
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of IsPrimitiveDefaultTransferable for default
       * transferable types.
       * \ingroup transfer
       *
       * \note
       * https://stackoverflow.com/a/57132084
       * but do not need to check for signatures at this point
       */
      template <typename T>
      struct IsPrimitiveDefaultTransferable<
          T, cpp17::void_t<
                 /// [primitive_transfer_signature]
                 decltype(&T::transfer)
                 /// [primitive_transfer_signature]
                 >> : IsRigidPrimitive<T> {};
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*!\brief Compile time check for valid default transferable
       * operations
       * \ingroup transfer
       *
       * IsDefaultTransferable type trait class tests whether or not a system
       * and its primitive type can be transferred by default.
       *
       * \usage
       * For any system type `Sys`,
       * \code
       * using result = IsPrimitiveDefaultTransferable<Sys>;
       * \endcode
       *
       * \metareturns
       * cpp17::bool_constant
       *
       * \semantics
       * If the type `Sys` satisfies IsSystemDefaultTransferable and its
       * primitive type satisfies IsPrimitiveDefaultTransferable
       * \code
       * typename result::type = std::true_type;
       * \endcode
       * otherwise
       * \code
       * typename result::type = std::false_type;
       * \endcode
       *
       * \example
       * \snippet Transfer/Test_TypeTraits.cpp IsDefaultTransferable_eg
       *
       * \see IsSystemDefaultTransferable, IsPrimitiveDefaultTransferable
       *
       * \tparam Sys the type to check
       */
      template <typename Sys>
      struct IsDefaultTransferable
          : public cpp17::conjunction<
                tt::IsSystemDefaultTransferable<
                    typename TransferTraits<Sys>::SystemType>,
                tt::IsPrimitiveDefaultTransferable<
                    typename TransferTraits<Sys>::PrimitiveType>> {};
      //************************************************************************

      namespace {
        //**********************************************************************
        /*!\brief Compile time switch between TransferTags
        // \ingroup transfer type_traits
        //
        // The TagSwitch type trait class switches the given tag type
        // \c FromOrToTag to its converse type \c type. If type \c FromOrToTag
        // is not a valid tag type, a compiler error is generated.
        //
        // \usage
        // For any tag type \c T,
           \code
             using result = TagSwitch<T>;
           \endcode
        //
        // \metareturns
        // cpp17::bool_constant
        //
        // \semantics
        // For a tag type \c T
           \code
             typename result::type = from_t, to_t;
           \endcode
        //
        // \example
           \code
             using tag_t = tags::from_t;
             using opposite_t = typename TagSwitch<tag_t>::type;
             static_assert(std::is_same<opposite_t, tags::to_t>::value, "");
           \endcode
        //
        // \see from_t, to_t
        //
        // \tparam FromOrToTag the tag type to check
        */
        template <typename FromOrToTag>
        struct TagSwitch {
          static constexpr tags::from_t switch_tag(tags::to_t);  // NO LINT
          static constexpr tags::to_t switch_tag(tags::from_t);
          // All other cases should return a compiler error
          static constexpr void switch_tag(...);
          using type = decltype(switch_tag(std::declval<FromOrToTag>()));
        };
        //**********************************************************************
      }  // namespace

      //========================================================================
      //
      //  ALIAS DECLARATIONS
      //
      //========================================================================

      //************************************************************************
      /*!\brief Auxiliary alias template for the TagSwitch type trait.
       * \ingroup transfer
       *
       * The tag_switch_t alias template provides a convenient shortcut to
       * access the nested \c type definition from the result of applying
       * TagSwitch to the input type \a FromOrToTag. The following code example
       * shows both ways to access the nested type definition:
       *
       * \example
       * \code
       *   using Type1 = typename T::TagType;
       *   using Type2 = tag_type_t<T>;
       *   static_assert(std::is_same<Type1, Type2>::value, "Always passes");
       * \endcode
       *
       * \see TagSwitch
       *
       * \tparam FromOrToTag
       */
      template <typename FromOrToTag>
      using tag_switch_t = typename TagSwitch<FromOrToTag>::type;
      //************************************************************************

    }  // namespace tt

  }  // namespace collision

}  // namespace elastica
