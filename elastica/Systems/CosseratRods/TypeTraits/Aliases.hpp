//==============================================================================
/*!
//  \file
//  \brief Alias templates for Cosserat rod types
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace detail_cosserat_rod {

    namespace tt {

      //========================================================================
      //
      //  ALIAS DECLARATIONS
      //
      //========================================================================

      //************************************************************************
      /*!\brief Alias declaration for nested \c InitializerType type
      // definitions.
      // \ingroup cosserat_rod_type_traits
      //
      // The initializer_tag_t alias declaration provides a convenient shortcut
      // to access the nested \a InitializerTag type definition of the given
      // type \a T. The following code example shows both ways to access the
      // nested type definition:
      //
      // \example
         \code
         using Type1 = typename T::InitializerTag;
         using Type2 = initializer_tag_t<T>;
         \endcode
      */
      template <typename T>
      using initializer_tag_t = typename T::InitializerTag;
      //************************************************************************

      //************************************************************************
      /*!\brief Alias declaration for nested \c ParameterType type definitions.
      // \ingroup cosserat_rod_type_traits
      //
      // The parameter_type_t alias declaration provides a convenient shortcut
      // to access the nested \a ParameterType type definition of the given type
      // \a T. The following code example shows both ways to access the nested
      // type definition:
      //
      // \example
         \code
         using Type1 = typename T::ParameterType;
         using Type2 = parameter_type_t<T>;
         \endcode
      */
      // TODO : shouldn' be here, should be part of strong types
      template <typename T>
      using parameter_type_t = typename T::ParameterType;
      //************************************************************************

      //************************************************************************
      /*!\brief Alias declaration for nested \c SliceType type definitions.
      // \ingroup cosserat_rod_type_traits
      //
      // The slice_type_t alias declaration provides a convenient shortcut to
      // access the nested \a SliceType type definition of the given type \a T.
      // The following code example shows both ways to access the nested type
      // definition:
      //
      // \example
         \code
         using Type1 = typename T::SliceType;
         using Type2 = slice_type_t<T>;
         \endcode
      */
      template <typename T>
      using slice_type_t = typename T::SliceType;
      //************************************************************************

      //************************************************************************
      /*!\brief Alias declaration for nested \c TagType type definitions.
      // \ingroup cosserat_rod_type_traits
      //
      // The tag_type_t alias declaration provides a convenient shortcut to
      // access the nested \a TagType type definition of the given type \a T.
      // The following code example shows both ways to access the nested type
      // definition:
      //
      // \example
         \code
         using Type1 = typename T::TagType;
         using Type2 = tag_type_t<T>;
         \endcode
      */
      template <typename T>
      using tag_type_t = typename T::TagType;
      //************************************************************************

    }  // namespace tt

  }  // namespace detail_cosserat_rod

}  // namespace elastica
