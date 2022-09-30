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
