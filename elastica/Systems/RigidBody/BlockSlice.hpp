//==============================================================================
/*!
//  \file
//  \brief Specialization of a Block slice for Rigid bodys
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

/// Forward declarations
#include "Systems/RigidBody/Types.hpp"
///
#include "Systems/Block/Block.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
#include "Utilities/End.hpp"  // from_end

namespace blocks {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of a slice/view on a Block for Rigid bodys
   * \ingroup blocks
   *
   * \tparam RBT        The traits class for a Rigid body
   * \tparam Components Components customizing a Rigid body (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  class BlockSlice<
      elastica::rigid_body::RigidBodyPlugin<RBT, BlockSlice, Components...>>
      : public detail::BlockSliceFacade<elastica::rigid_body::RigidBodyPlugin<
            RBT, BlockSlice, Components...>> {
   private:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = RBT;
    //! Type of the parent plugin
    using Parent =
        detail::BlockSliceFacade<elastica::rigid_body::RigidBodyPlugin<
            Traits, BlockSlice, Components...>>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using typename Parent::PluginType;
    //! Type of the slice
    using This = BlockSlice<PluginType>;
    //! Type of the parent block
    using ParentBlock = typename PluginFrom<This>::template to<Block>::type;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the main block
    friend ParentBlock;
    //    //! Friend the equality operator
    //     operator==(This const&, This const&);
    template <class Plugin>
    friend constexpr bool operator==(BlockSlice<Plugin> const& lhs,
                                     BlockSlice<Plugin> const& rhs);
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param args... slice types to be transferred to the underlying container
     */
    template <typename... Args>
    explicit BlockSlice(ParentBlock& parent_block, Args&&... args) noexcept(
        noexcept(Parent(std::forward<Args>(args)...)))
        : Parent(std::forward<Args>(args)...), parent_block_(parent_block) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    BlockSlice(BlockSlice const& other)
        : Parent(static_cast<Parent const&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    BlockSlice(BlockSlice& other)
        : Parent(static_cast<Parent&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other slice to move from
     */
    BlockSlice(BlockSlice&& other) noexcept
        : Parent(static_cast<Parent&&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //@}
    //**************************************************************************

    //**Utility methods*********************************************************
    /*!\name Utility methods*/
    //@{

    //**************************************************************************
    /*!\brief Gets the parent block
     */
    inline constexpr auto parent() const noexcept -> ParentBlock const& {
      return parent_block_;
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~BlockSlice() = default;
    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Reference to the parent block
    ParentBlock const& parent_block_;
    //@}
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of a slice/view on a const Block for Rigid bodys
   * \ingroup blocks
   *
   * \tparam RBT        The traits class for a Rigid body
   * \tparam Components Components customizing a Rigid body (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  class ConstBlockSlice<elastica::rigid_body::RigidBodyPlugin<
      RBT, ConstBlockSlice, Components...>>
      : public detail::ConstBlockSliceFacade<
            elastica::rigid_body::RigidBodyPlugin<RBT, ConstBlockSlice,
                                                  Components...>> {
   private:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = RBT;
    //! Type of the parent plugin
    using Parent =
        detail::ConstBlockSliceFacade<elastica::rigid_body::RigidBodyPlugin<
            Traits, ConstBlockSlice, Components...>>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using typename Parent::PluginType;
    //! Type of the slice
    using This = ConstBlockSlice<PluginType>;
    //! Type of the parent block
    using ParentBlock = typename PluginFrom<This>::template to<Block>::type;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the main block
    friend ParentBlock;
    //! Friend the equality operator
    template <class Plugin>
    friend constexpr bool operator==(ConstBlockSlice<Plugin> const& lhs,
                                     ConstBlockSlice<Plugin> const& rhs);
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param args... slice types to be transferred to the underlying container
     */
    template <typename... Args>
    explicit ConstBlockSlice(
        ParentBlock const& parent_block,
        Args&&... args) noexcept(noexcept(Parent(std::forward<Args>(args)...)))
        : Parent(std::forward<Args>(args)...), parent_block_(parent_block) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    ConstBlockSlice(ConstBlockSlice const& other)
        : Parent(static_cast<Parent const&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    ConstBlockSlice(ConstBlockSlice& other)
        : Parent(static_cast<Parent&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other slice to move from
     */
    ConstBlockSlice(ConstBlockSlice&& other) noexcept
        : Parent(static_cast<Parent&&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~ConstBlockSlice() = default;
    //@}
    //**************************************************************************

    //**Utility methods*********************************************************
    /*!\name Utility methods*/
    //@{

    //**************************************************************************
    /*!\brief Gets the parent block
     */
    inline constexpr auto parent() const noexcept -> ParentBlock const& {
      return parent_block_;
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Reference to the parent block
    ParentBlock const& parent_block_;
    //@}
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of a slice/view on a Block for Tagged Rigid bodys
   * \ingroup blocks
   *
   * \tparam RBT        The traits class for a Rigid body
   * \tparam Tag        The tag for the Tagged RigidBody plugin
   * \tparam Components Components customizing a Rigid body (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename RBT, typename Tag,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  class BlockSlice<elastica::rigid_body::TaggedRigidBodyPlugin<
      RBT, BlockSlice, Tag, Components...>>
      : public BlockSlice<elastica::rigid_body::RigidBodyPlugin<
            RBT, BlockSlice, Components...>> {
    // only conceptually "new" constructor that we need to allow is conversion
    // of a normal block slice to a tagged block slice.
    // but even this conversion is facilitated by the inherited constructors.
    //
    // All other constructors should follow through, by virtue of slicing. The
    // "reference" stored is actually a reference to a non-tagged block, but we
    // can always convert it back to a tagged block if needed.
   private:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = RBT;
    //! Type of the parent plugin
    using Parent =
        BlockSlice<elastica::rigid_body::RigidBodyPlugin<Traits, BlockSlice,
                                                         Components...>>;
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    BlockSlice() = delete;
    // constructor usde in slice() methods
    using Parent::Parent;
    // these are constructors used in emplace() of block
    BlockSlice(Parent const& other) : Parent(other){};
    BlockSlice(Parent&& other) noexcept : Parent(std::move(other)){};
    //@}
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of a const slice/view on a Block for Tagged RigidBody
   * rods
   * \ingroup blocks
   *
   * \tparam RBT        The traits class for a Rigid body
   * \tparam Tag        The tag for the Tagged RigidBody plugin
   * \tparam Components Components customizing a Rigid body (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename RBT, typename Tag,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  class ConstBlockSlice<elastica::rigid_body::TaggedRigidBodyPlugin<
      RBT, ConstBlockSlice, Tag, Components...>>
      : public ConstBlockSlice<elastica::rigid_body::RigidBodyPlugin<
            RBT, ConstBlockSlice, Components...>> {
   private:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = RBT;
    //! Type of the parent plugin
    using Parent = ConstBlockSlice<elastica::rigid_body::RigidBodyPlugin<
        Traits, ConstBlockSlice, Components...>>;
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    using Parent::Parent;
    //@}
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  FREE FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Returns the size of the Rigid body.
   * \ingroup rigid_body blocks
   *
   * \param rigid_body The current rigid body.
   * \return The  number of elements in the rigid body.
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline constexpr auto size(
      BlockSlice<::elastica::rigid_body::RigidBodyPlugin<
          RBT, BlockSlice, Components...>> const& rigid_body) noexcept {
    // it might be not the best idea to have a size() member
    return rigid_body.size();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns the size of the Rigid body.
   * \ingroup rigid_body blocks
   *
   * \param rigid_body The current rigid body.
   * \return The  number of elements in the rigid body.
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline constexpr auto size(
      ConstBlockSlice<::elastica::rigid_body::RigidBodyPlugin<
          RBT, ConstBlockSlice, Components...>> const& rigid_body) noexcept {
    // it might be not the best idea to have a size() member
    return rigid_body.size();
  }
  //****************************************************************************

}  // namespace blocks
