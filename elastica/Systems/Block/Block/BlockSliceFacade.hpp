//==============================================================================
/*!
//  \file
//  \brief BlockSlice facade for making slices and views
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
#include "Systems/Block/Block/Types.hpp"
//
#include "Systems/Block/Block/AsVariables.hpp"
#include "Systems/Block/Block/TypeTraits.hpp"
#include "Systems/Block/BlockVariables/TypeTraits.hpp"

namespace blocks {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Implementation of a slice/view on an \elastica Block
     * \ingroup blocks
     *
     * \details
     * The BlockSliceFacade template class provides a slice (or) view into a
     * Block of `data` and `operations`. It also models the `ComputationalBlock`
     * concept and hence can be used across places where a \elastica Block is
     * used as a template parameter.
     *
     * Similar to Block, BlockSliceFacade can also be customized (specialized)
     * for any Lagrangian entity, but experience suggests that extensive
     * customization is not needed. For a concrete example, please see
     * specializations of BlockSliceFacade in @ref cosserat_rod
     *
     * \usage
     * The intended usage of a BlockSliceFacade is when a view is required into
     the data
     * held by a block---this is frequently the case when a user either
     * 1. adds an entity into the Simulator, or
     * 2. requires access (for e.g. reading/writing to disk) to only a portion
     of
     * the block.
     * The pattern that is most commonly seen in the use case (1) is for a
     * BlockSliceFacade templated on some `Plugin` type to be only used with a
     Block
     * of the same `Plugin` type, when adding new entities to the Block.
     * For use case (2) we suggest the user to the blocks::slice() function,
     which
     * has an intuitive, explicit slice syntax, or even the subscript
     operator[].
     * We note that we might explicitly disable the subscript operator [] for
     * slicing in the future, as the semantics are potentially unclear.
     *
     * Finally, with no additional coding effort, the BlockSliceFacade has
     exactly the
     * same operations as the mother Block (aka Block of the same `Plugin`
     type),
     * but now it operates only on that slice of the data. This means that a
     * BlockSliceFacade can be used as a small Block in itself which greatly
     simplifies
     * interfacing different components of \elastica---the user need not care or
     * even know about whether the data that she has is a Block or a
     BlockSliceFacade!
     * For example,
       \code
           // ... make simulator etc ...
           auto my_rod = simulator.emplace_back<CosseratRod>( * ...args... *);
           // the args go and form a Block
           // which in turn returns a BlockSliceFacade
           // which the user gets as my_rod

           // use my_rod like a regular cosserat rod
           simulator->constrain(my_rod)->using<SomeConstraint>( *...args... *);
       \endcode
     * This abstraction helped us constrain \c my_rod, embedded in a Block of
     * data, using \c SomeConstraint just like any non-Blocked item of the
     * \elastica library.
     *
     * \tparam Plugin The computational plugin modeling a Lagrangian entity
     *
     * \see Block, blocks::slice
     */
    template <class Plugin>
    class BlockSliceFacade : public Plugin {
     protected:
      //**Type definitions******************************************************
      //! Type of the parent plugin
      using Parent = Plugin;
      //! Type of the slice
      using This = BlockSliceFacade<Plugin>;
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the plugin
      friend Parent;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! List of variables
      using typename Parent::Variables;
      //! Typelist of all variable slices
      using VariableSlicesList = typename as_slices<Variables>::slices;
      //! Values of all variable slices
      using BlockVariableSlices = as_block_variables<VariableSlicesList>;
      //! Conformant mapping between tags and variables
      using VariableMap = VariableMapping<VariableSlicesList>;
      //! Type of the plugin
      using PluginType = Plugin;
      //************************************************************************

      // ideally we make the constructors protected, but this interferes with
      // nothrow specifications
      // https://stackoverflow.com/questions/40441994/usage-of-noexcept-in-derived-classes
     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief The default constructor.
       *
       * \note Not always valid.
       */
      explicit BlockSliceFacade() noexcept(noexcept(BlockVariableSlices{}))
          : slices_() {}
      //************************************************************************

      //************************************************************************
      /*!\brief The default constructor.
       *
       * \param views view to be transferred to the underlying container
       */
      explicit BlockSliceFacade(BlockVariableSlices slices) noexcept
          : slices_(std::move(slices)) {}
      //************************************************************************

      //************************************************************************
      /*!\brief The copy constructor.
       *
       * \param other slice to copy
       */
      BlockSliceFacade(BlockSliceFacade const& other)
          : slices_(other.slices_){};
      //************************************************************************

      //************************************************************************
      /*!\brief The move constructor.
       *
       * \param other slice to move from
       */
      BlockSliceFacade(BlockSliceFacade&& other) noexcept
          : slices_(std::move(other.slices_)){};
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~BlockSliceFacade() = default;
      //@}
      //************************************************************************

      //**Data access***********************************************************
      /*!\name Data access */
      //@{

      //************************************************************************
      /*!\brief Access to the underlying data
      //
      // \return Mutable lvalue reference to the underlying data
      */
      inline constexpr BlockVariableSlices& data() & noexcept {
        return slices_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the underlying data
      //
      // \return Constant lvalue reference to the underlying data
      */
      inline constexpr BlockVariableSlices const& data() const& noexcept {
        return slices_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the underlying data
      //
      // \return Mutable rvalue reference to the underlying data
      */
      inline constexpr BlockVariableSlices&& data() && noexcept {
        return static_cast<BlockVariableSlices&&>(slices_);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the underlying data
      //
      // \return Const rvalue reference to the underlying data
      */
      inline constexpr BlockVariableSlices const&& data() const&& noexcept {
        return static_cast<BlockVariableSlices const&&>(slices_);
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! All slices of Block variables
      BlockVariableSlices slices_;
      //@}
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Implementation of a slice/view on a const \elastica Block
     * \ingroup blocks
     *
     * \details
     * The ConstBlockSliceFacade template class provides a slice (or) view into
     * a constant Block of \a data and \a operations. It also models the
     * `ComputationalBlock` concept and hence can be used across places where a
     * \elastica Block is used as a template parameter.
     *
     * Notably, it differs from BlockSliceFacade in one key aspect : the
     * underlying data/view is always constant and cannot be modified (it is
     * only read only). Hence ConstBlock is useful for propagating
     * const-correctness throughout the code. It can be used in places where one
     * needs to pass (const)-data to the user which she can then copy and use it
     * for her own purposes.
     *
     * \tparam Plugin The computational plugin modeling a Lagrangian entity
     *
     * \see BlockSliceFacade
     */
    template <class Plugin>  // The computational plugin type
    class ConstBlockSliceFacade : public Plugin {
     protected:
      //**Type definitions******************************************************
      //! Type of the parent plugin
      using Parent = Plugin;
      //! Type of the slice
      using This = ConstBlockSliceFacade<Parent>;
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the plugin
      friend Parent;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! List of variables
      using typename Parent::Variables;
      //! Typelist of all variable slices
      using VariableSlicesList = typename as_slices<Variables>::const_slices;
      //! Values of all variable slices
      using BlockVariableSlices = as_block_variables<VariableSlicesList>;
      //! Conformant mapping between tags and variables
      using VariableMap = VariableMapping<VariableSlicesList>;
      //! Type of the plugin
      using PluginType = Plugin;
      //************************************************************************

      // ideally we make the constructors protected, but this interferes with
      // nothrow specifications
      // https://stackoverflow.com/questions/40441994/usage-of-noexcept-in-derived-classes
     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief The default constructor.
       *
       * \note Not always valid.
       */
      explicit ConstBlockSliceFacade() noexcept(noexcept(BlockVariableSlices{}))
          : slices_{} {}
      //************************************************************************

      //************************************************************************
      /*!\brief The default constructor.
       *
       * \param views view to be transferred to the underlying container
       */
      explicit ConstBlockSliceFacade(BlockVariableSlices slices) noexcept
          : slices_(std::move(slices)) {}
      //************************************************************************

      //************************************************************************
      /*!\brief The copy constructor.
       *
       * \param other slice to copy
       */
      ConstBlockSliceFacade(ConstBlockSliceFacade const& other)
          : slices_(other.slices_){};
      //************************************************************************

      //************************************************************************
      /*!\brief The move constructor.
       *
       * \param other slice to move from
       */
      ConstBlockSliceFacade(ConstBlockSliceFacade&& other) noexcept
          : slices_(std::move(other.slices_)){};
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~ConstBlockSliceFacade() = default;
      //@}
      //************************************************************************

      //**Data access***********************************************************
      /*!\name Data access */
      //@{

      //************************************************************************
      /*!\brief Access to the underlying data
      //
      // \return Constant lvalue reference to the underlying data
      */
      inline constexpr BlockVariableSlices const& data() const& noexcept {
        return slices_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the underlying data
      //
      // \return Const rvalue reference to the underlying data
      */
      inline constexpr BlockVariableSlices const&& data() const&& noexcept {
        return static_cast<BlockVariableSlices const&&>(slices_);
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! All slices of constant Block variables
      BlockVariableSlices const slices_;
      //@}
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //==========================================================================
    //
    //  GLOBAL OPERATORS
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <class Plugin>
    inline constexpr auto blocks_equality_impl(
        BlockSliceFacade<Plugin> const& lhs,
        BlockSliceFacade<Plugin> const& rhs) noexcept(noexcept(lhs.data() ==
                                                               rhs.data()))
        -> bool {
      return (lhs.data() == rhs.data());
    }
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <class Plugin>
    inline constexpr auto blocks_equality_impl(
        ConstBlockSliceFacade<Plugin> const& lhs,
        ConstBlockSliceFacade<Plugin> const&
            rhs) noexcept(noexcept(lhs.data() == rhs.data())) -> bool {
      return (lhs.data() == rhs.data());
    }
    /*! \endcond */
    //**************************************************************************

    //**Equality operator*******************************************************
    /*!\brief Equality comparison between two BlockSliceFacade objects.
     *
     * \param lhs The left-hand side slice.
     * \param rhs The right-hand side slice.
     * \return \a true if the slices are same, else \a false
     */
    template <class Plugin>
    inline constexpr auto operator==(
        BlockSliceFacade<Plugin> const& lhs,
        BlockSliceFacade<Plugin> const&
            rhs) noexcept(noexcept(detail::blocks_equality_impl(lhs, rhs)))
        -> bool {
      return blocks_equality_impl(lhs, rhs);
    }
    //**************************************************************************

    //**Equality operator*******************************************************
    /*!\brief Equality comparison between two ConstBlockSliceFacade objects.
     *
     * \param lhs The left-hand side const slice.
     * \param rhs The right-hand side const slice.
     * \return \a true if the const slices are same, else \a false
     */
    template <class Plugin>
    inline constexpr auto operator==(
        ConstBlockSliceFacade<Plugin> const& lhs,
        ConstBlockSliceFacade<Plugin> const&
            rhs) noexcept(noexcept(detail::blocks_equality_impl(lhs, rhs)))
        -> bool {
      return blocks_equality_impl(lhs, rhs);
    }
    //**************************************************************************

  }  // namespace detail

}  // namespace blocks