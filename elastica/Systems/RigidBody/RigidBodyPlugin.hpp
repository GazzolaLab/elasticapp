//==============================================================================
/*!
//  \file
//  \brief Plugin for Cosserat Rods within \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//
#include "Systems/common/Cardinality.hpp"
//#include "Systems/common/IndexCheck.hpp"
/// Forward declarations
#include "Systems/RigidBody/Types.hpp"
///
#include "Systems/RigidBody/RigidBodyTraits.hpp"
///
#include "Systems/RigidBody/Tags.hpp"
///
#include "Utilities/TMPL.hpp"
//
#include <algorithm>

namespace elastica {

  namespace rigid_body {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITIONS
      //
      //========================================================================

      //************************************************************************
      /*!\brief Additional Variables corresponding to a Cosserat rod plugin
       * \ingroup rigid_body
       *
       * \details
       * RigidBodyPluginVariables contains the definitions of variables
       * used within the Blocks framework for a Cosserat rod, that do not belong
       * to any other component (such as geometry, elasticity).
       *
       * \tparam RBT A valid Cosserat Rod Traits class
       *
       * \see elastica::rigid_body::RigidBodyPlugin
       */
      template <typename RBT>
      class RigidBodyPluginVariables {
       private:
        //**Type definitions****************************************************
        //! Traits type
        using Traits = RBT;
        //**********************************************************************

       protected:
        //**Variable definitions************************************************
        /*!\name Variable definitions*/
        //@{

        //**********************************************************************
        /*!\brief Variable marking number of elements within the Cosserat rod
         * hierarchy
         */
        struct NElement : public Traits::template RigidBodyVariable<
                              tags::NElement,                   //
                              typename Traits::DataType::Index  //
                              > {};
        //**********************************************************************

        //**Type definitions****************************************************
        //! List of computed variables
        using ComputedVariables = tmpl::list<NElement>;
        //! List of initialized variables
        using InitializedVariables = tmpl::list<>;
        //! List of all variables
        using Variables = tmpl::append<InitializedVariables, ComputedVariables>;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Plugin for implementing Rigid bodys within @ref blocks
     * in \elastica
     * \ingroup rigid_body
     *
     * \details
     * RigidBodyPlugin is the computational plugin modeling a Rigid body
     * for use with template types modeling a \c ComputationalBlock concept
     * (blocks::Block, blocks::BlockSlice) in \elastica.
     *
     * As with other Plugins, RigidBodyPlugin is templated for customization.
     * The three template parameters serve different purposes.
     *
     * The first template parameter is a Traits class `RBT` for customizing the
     * data -structures, placement and algorithms used. For example, see
     * elastica::rigid_body::RigidBodyTraits.
     *
     * The second template parameter is the Block-like entity which derives
     * from the RigidBodyPlugin using the CRTP pattern. For examples of this
     * pattern, refer to examples in @ref blocks.
     *
     * The third template parameter is reserved for multiple `Components`. These
     * are templated types controlling customizing orthogonal aspects of a
     * Rigid body. For examples, see @ref rigid_body_component
     *
     * \tparam RBT A valid Cosserat Rod Traits class
     * \tparam ComputationalBlock A template type modeling the
     * `ComputationalBlock` concept
     * \tparam Components Variadic components for customizing behavior
     *
     * \see blocks::Block, blocks::BlockSlice
     */
    template <typename RBT, template <typename> class ComputationalBlock,
              template <typename /*RBT*/, typename /* ComputationalBlock */>
              class... Components>
    class RigidBodyPlugin
        : public detail::RigidBodyPluginVariables<RBT>,
          public HasUnitCardinality<
              RigidBodyPlugin<RBT, ComputationalBlock, Components...>>,
          public Components<
              RBT, ComputationalBlock<RigidBodyPlugin<RBT, ComputationalBlock,
                                                      Components...>>>...,
          public ::tt::ConformsTo<blocks::protocols::Plugin> {
     private:
      //**Type definitions******************************************************
      //! Traits type
      using Traits = RBT;
      //! This type
      using This = RigidBodyPlugin<Traits, ComputationalBlock, Components...>;
      //! Variable definitions
      using VariableDefinitions = detail::RigidBodyPluginVariables<Traits>;
      //! Size type
      using size_type = typename Traits::size_type;
      //! Index type
      using index_type = typename Traits::index_type;
      //************************************************************************

     private:
      //**CRTP section**********************************************************
      /*!\name CRTP section */
      //@{

      //**Type definitions******************************************************
      //! Type of the bottom level derived class
      using Self = ComputationalBlock<This>;
      //! Reference type of the bottom level derived class
      using Reference = Self&;
      //! const reference type of the bottom level derived class
      using ConstReference = Self const&;
      //************************************************************************

     public:
      //**Self method***********************************************************
      /*!\brief Access to the underlying derived
      //
      // \return Mutable reference to the underlying derived
      //
      // Safely down-casts this module to the underlying derived type, using
      // the Curiously Recurring Template Pattern (CRTP).
      */
      inline constexpr auto self() & noexcept -> Reference {
        return static_cast<Reference>(*this);
      }
      //************************************************************************

      //**Self method***********************************************************
      /*!\brief Access to the underlying derived
      //
      // \return Const reference to the underlying derived
      //
      // Safely down-casts this module to the underlying derived type, using
      // the Curiously Recurring Template Pattern (CRTP).
      */
      inline constexpr auto self() const& noexcept -> ConstReference {
        return static_cast<ConstReference>(*this);
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Useful aliases********************************************************
      using NElement = typename VariableDefinitions::NElement;
      //************************************************************************

     public:
      // These are promoted to public scope for protocols to confirm they work
      // as well as any type traits based on the Plugin (for example, to check
      // whether to have a default initializer). Additionally this simplifies
      // and honors the contract with BlockFacade.

      //**Block type definitions************************************************
      //! List of computed variables
      using ComputedVariables =
          tmpl::append<typename VariableDefinitions::ComputedVariables,
                       typename Components<RBT, Self>::ComputedVariables...>;
      //! List of initialized variables
      using InitializedVariables =
          tmpl::append<typename VariableDefinitions::InitializedVariables,
                       typename Components<RBT, Self>::InitializedVariables...>;
      //! List of all variables
      using Variables =
          tmpl::append<typename VariableDefinitions::Variables,
                       typename Components<RBT, Self>::Variables...>;
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Initialize method for the entire cosserat rod
       *
       * \details
       * initialize() is responsible for initializing the variables present
       * in this plugin and all its components in the final block of data. For
       * InitializedVariables, it fetches the corresponding initializer from
       * the set of `initializers` to fill in the data. For ComputedVariables
       * the default value is usually set. The plugin then calls the
       * corresponding `initialize` method for each component and hence fills in
       * all component Variables as well.
       *
       * \tparam DownstreamBlock The final block-like object which is
       * derived from the current component
       * \tparam CosseratInitializer An Initializer corresponding to the
       * Rigid body hierarchy
       */
      template <template <typename> class BlockLike, class CosseratInitializer>
      static void initialize(
          RigidBodyPlugin<Traits, BlockLike, Components...>& this_component,
          CosseratInitializer&& initializers) {
        // initialize the elements boi
        {
          auto& variable(blocks::get<tags::NElement>(this_component.self()));
          // this automatically is deduced to 1
          variable = 1UL;
        }

        // Pass self here to make minimal changes to the tests.
        EXPAND_PACK_LEFT_TO_RIGHT(Components<RBT, Self>::initialize(
            this_component.self(),
            std::forward<CosseratInitializer>(initializers)));
      }
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
     protected:
      //************************************************************************
      /*!\name Element methods*/
      //@{
      /*!\brief Gets the n_elements buffer of the current rod
       */
      inline constexpr decltype(auto) n_elements() & noexcept {
        // this version is only for testing
        return blocks::get<tags::NElement>(self());
      }
      inline auto n_elements(index_type const unit) & noexcept -> size_type& {
        // Zero-based indexing
        return NElement::slice(n_elements(), unit);
        // return n_elements()[unit];
      }

     public:
      inline constexpr decltype(auto) n_elements() const& noexcept {
        return blocks::get<tags::NElement>(self());
      }
      // Zero-based indexing
      inline auto n_elements(index_type const unit) const& noexcept
          -> size_type {
        return NElement::slice(n_elements(), unit);
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\brief Returns number of units (unique, individual cosserat rods)
       * stored in the block
       */
      inline constexpr auto n_units() const noexcept -> size_type {
        return n_elements().size();
      }
      //************************************************************************

      //************************************************************************
      /*!\name Total methods*/
      //@{
      /*!\brief Gets the total number of elements/nodes/voronois for the
       * Rigid body, for each placement type.
       */
      inline auto total_n_elements() const noexcept -> size_type {
        // may need to change based on elements() structure
        return std::accumulate(n_elements().begin(), n_elements().end(), 0UL);
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\brief Checks if the current Block/BlockSlice is empty
       */
      inline auto empty() const noexcept -> bool {
        return not n_elements()
                       .size();  // n_elements().empty() doesn't work with blaze
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Returns the size of the current Block/BlockSlice
       */
      inline auto size() const noexcept { return total_n_elements(); }
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace rigid_body

  //  //**Check
  //  functions*********************************************************
  //  /*!\name Index check functions
  //   * \ingroup rigid_body
  //   * \brief Checks whether indices are within range of accessible values
  //   * */
  //  template <typename RBT, template <typename> class ComputationalBlock,
  //            template <typename /*RBT*/, typename /*InitializedBlock*/>
  //            class... Components,
  //            typename IndexType>
  //  inline auto index_check(
  //      ::elastica::rigid_body::RigidBodyPlugin<RBT, ComputationalBlock,
  //                                              Components...> const&
  //                                              block_like,
  //      IndexType index_to_be_sliced) -> std::size_t {
  //    return index_check_helper(size(block_like.self()),
  //                              std::move(index_to_be_sliced));
  //  }

}  // namespace elastica

namespace elastica {

  namespace rigid_body {

    //**************************************************************************
    /*!\brief The default tag parameter for a TaggedRigidBodyPlugin
     * \ingroup rigid_body
     *
     * \details
     * Default tag parameter for the third template parameter of the
     * TaggedRigidBodyPlugin
     * \see TaggedRigidBodyPlugin
     */
    struct DefaultRigidBodyPluginTag;
    //**************************************************************************

    //**************************************************************************
    /*!\brief A cosserat rod plugin with a tag parameter
     * \ingroup rigid_body
     *
     * \tparam RBT        The traits class for a Rigid body
     * \tparam ComputationalBlock A template type modeling the
     * `ComputationalBlock` concept
     * \tparam Tag        The tag for the Tagged Cosserat plugin
     * \tparam Components Components customizing a Rigid body (such as
     * geometry, elasticity)
     *
     * \see RigidBodyPlugin
     */
    template <typename RBT, template <typename> class ComputationalBlock,
              typename Tag = DefaultRigidBodyPluginTag,
              template <typename /*RBT*/, typename /* ComputationalBlock */>
              class... Components>
    class TaggedRigidBodyPlugin
        : public RigidBodyPlugin<RBT, ComputationalBlock, Components...> {};
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica
