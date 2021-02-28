//==============================================================================
/*!
//  \file
//  \brief Block type traits
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

#include "Types.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/IsA.hpp"

namespace blocks {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Check whether a given type `B` is a Block
  // \ingroup blocks
  //
  // \details
  // Inherits from std::true_type if `B` is a template specialization of a
  // blocks::Block, otherwise inherits from std::false_type.
  //
  // \usage
  // For any type `B`,
     \code
     using result = IsBlock<B>;
     \endcode
  //
  // \metareturns
  // cpp17::bool_constant
  //
  // \semantics
  // If the type `B` is an instantiation of elastica::Block, then
     \code
     typename result::type = std::true_type;
     \endcode
  // otherwise
     \code
     typename result::type = std::false_type;
     \endcode
  //
  // \example
  // \snippet Block/Test_TypeTraits.cpp is_block_example
  //
  // \tparam B : the type to check
  //
  // \see Block
  */
  template <typename B>
  struct IsBlock : ::tt::is_a<Block, B> {};
  //****************************************************************************

  //****************************************************************************
  /*!\brief Auxiliary variable template for the IsBlock type trait.
  // \ingroup blocks
  //
  // The is_block_v variable template provides a convenient
  // shortcut to access the nested \a value of the IsBlock
  // class template. For instance, given the type \a T the following two
  // statements are identical:
  // \example
     \code
       constexpr bool value1 = IsBlock<T>::value;
       constexpr bool value2 = is_block_v<T>;
     \endcode
  //
  // \tparam B : the type to check
  //
  // \see IsBlock
  */
  template <typename B>
  constexpr bool is_block_v = IsBlock<B>::value;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Check whether a given type `B` is a BlockSlice
  // \ingroup blocks
  //
  // \details
  // Inherits from std::true_type if `B` is a template specialization of a
  // blocks::BlockSlice, otherwise inherits from std::false_type.
  //
  // \usage
  // For any type `B`,
     \code
     using result = IsBlockSlice<B>;
     \endcode
  //
  // \metareturns
  // cpp17::bool_constant
  //
  // \semantics
  // If the type `B` is an instantiation of elastica::BlockSlice, then
     \code
     typename result::type = std::true_type;
     \endcode
  // otherwise
     \code
     typename result::type = std::false_type;
     \endcode
  //
  // \example
  // \snippet Block/Test_TypeTraits.cpp is_blockslice_example
  //
  // \tparam B : the type to check
  //
  // \see BlockSlice
  */
  template <typename B>
  struct IsBlockSlice : ::tt::is_a<BlockSlice, B> {};
  //****************************************************************************

  //****************************************************************************
  /*!\brief Auxiliary variable template for the IsBlockSlice type trait.
  // \ingroup blocks
  //
  // The is_block_slice_v variable template provides a convenient
  // shortcut to access the nested \a value of the IsBlockSlice
  // class template. For instance, given the type \a T the following two
  // statements are identical:
  // \example
     \code
       constexpr bool value1 = IsBlockSlice<T>::value;
       constexpr bool value2 = is_block_slice_v<T>;
     \endcode
  //
  // \tparam B : the type to check
  //
  // \see IsBlockSlice
  */
  template <typename B>
  constexpr bool is_block_slice_v = IsBlockSlice<B>::value;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Check whether a given type `B` is a ConstBlockSlice
  // \ingroup blocks
  //
  // \details
  // Inherits from std::true_type if `B` is a template specialization of a
  // blocks::ConstBlockSlice, otherwise inherits from std::false_type.
  //
  // \usage
  // For any type `B`,
     \code
     using result = IsConstBlockSlice<B>;
     \endcode
  //
  // \metareturns
  // cpp17::bool_constant
  //
  // \semantics
  // If the type `B` is an instantiation of elastica::ConstBlockSlice, then
     \code
     typename result::type = std::true_type;
     \endcode
  // otherwise
     \code
     typename result::type = std::false_type;
     \endcode
  //
  // \example
  // \snippet Block/Test_TypeTraits.cpp is_const_blockslice_example
  //
  // \tparam B : the type to check
  //
  // \see ConstBlockSlice
  */
  template <typename B>
  struct IsConstBlockSlice : ::tt::is_a<ConstBlockSlice, B> {};
  //****************************************************************************

  //****************************************************************************
  /*!\brief Auxiliary variable template for the IsConstBlockSlice type trait.
  // \ingroup blocks
  //
  // The is_const_block_slice_v variable template provides a convenient
  // shortcut to access the nested \a value of the IsConstBlockSlice
  // class template. For instance, given the type \a T the following two
  // statements are identical:
  // \example
     \code
       constexpr bool value1 = IsConstBlockSlice<T>::value;
       constexpr bool value2 = is_const_block_slice_v<T>;
     \endcode
  //
  // \tparam B : the type to check
  //
  // \see IsBlockSlice
  */
  template <typename B>
  constexpr bool is_const_block_slice_v = IsConstBlockSlice<B>::value;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Check whether a given type `B` models the \a Block concept
  // \ingroup blocks
  //
  // \details
  // Inherits from std::true_type if `B` models the \a Block concept, that is
  // `B` is one of blocks::Block, blocks::BlockSlice, otherwise inherits from
  // std::false_type.
  //
  // \usage
  // For any type `B`,
     \code
     using result = ModelsBlockConcept<B>;
     \endcode
  //
  // \metareturns
  // cpp17::bool_constant
  //
  // \semantics
  // If the type `B` is an instantiation of either a blocks::Block or a blocks
  // ::BlockSlice, then
     \code
     typename result::type = std::true_type;
     \endcode
  // otherwise
     \code
     typename result::type = std::false_type;
     \endcode
  //
  // \example
  // \snippet Block/Test_TypeTraits.cpp models_block_concept_example
  //
  // \tparam B : the type to check
  //
  // \see Block, BlockSlice, ConstBlockSlice
  */
  template <typename B>
  struct ModelsBlockConcept : ::cpp17::disjunction<IsBlock<B>, IsBlockSlice<B>,
                                                   IsConstBlockSlice<B>> {};
  //****************************************************************************

  //****************************************************************************
  /*!\brief Get the plugin type `P` of a block type `B`
  // \ingroup blocks
  //
  // \details
  // The PluginTrait type trait has a nested member `type` representing the
  // plugin `P` when the meta-argument `B` models block concept
  //
  // \usage
  // For any type `B` such that ModelsBlockConcept<B>::type = std::true_type,
     \code
     using result = PluginTrait<B>;
     \endcode
  // \metareturns
  // the type `T = P` such that `B = X<P>` where `X` is the type modeling block
  // concept
  //
  // \example
  // \snippet Block/Test_TypeTraits.cpp plugin_trait_example
  //
  // \tparam B Type (modeling block concept) whose `PluginTrait` is to be
  // retrieved
  //
  // \see ModelsBlockConcept
  */
  template <typename B>
  struct PluginTrait;
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of PluginTrait for a Block
  // \ingroup blocks
  */
  template <typename P>
  struct PluginTrait<Block<P>> {
    using type = P;
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of PluginTrait for a BlockSlice
  // \ingroup blocks
  */
  template <typename P>
  struct PluginTrait<BlockSlice<P>> {
    using type = P;
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of PluginTrait for a ConstBlockSlice
  // \ingroup blocks
  */
  template <typename P>
  struct PluginTrait<ConstBlockSlice<P>> {
    using type = P;
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*!\brief Eager result for obtaining plugin type `P` of a block model `B`
  // \ingroup blocks
  //
  // \details
  // The plugin_t type trait is a helper template to obtain the nested
  // type `type` of PluginTrait, used as follows.
  //
  // \example
  // \snippet Block/Test_TypeTraits.cpp plugin_t_example
  //
  // \see PluginTrait
  */
  /// [plugin_t]
  template <typename B>
  using plugin_t = typename PluginTrait<B>::type;
  /// [plugin_t]
  //****************************************************************************

}  // namespace blocks