//==============================================================================
/*!
//  \file
//  \brief Specialization of a Block for Rigid bodys
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

#include <vector>

/// Forward declarations
#include "Systems/RigidBody/Types.hpp"
///

#include "ErrorHandling/Assert.hpp"
#include "Systems/Block/Block.hpp"
#include "Systems/RigidBody/BlockSlice.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
#include "Systems/common/SymplecticStepperAdapter.hpp"
///
#include "Utilities/NonCopyable.hpp"
//#include "Utilities/StdHelpers.hpp"
#include "Utilities/End.hpp"
#include "Utilities/TMPL.hpp"

// serialization support, feels strange coming from simulator
#include "Simulator/IO/Serialization/Serialize.hpp"

namespace blocks {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of blocks::Block for RigidBodyPlugin
   * \ingroup blocks
   *
   * \tparam RBT        The traits class for a Rigid body
   * \tparam Components Components customizing a Rigid body (such as geometry,
   *                    elasticity)
   *
   * \see blocks::Block
   */
  template <typename RBT,  // Cosserat Rod Traits
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  class Block<elastica::rigid_body::RigidBodyPlugin<RBT, Block, Components...>>
      // final (not final because testing requires deriving)
      : private elastica::NonCopyable,
        // public ::elastica::SymplecticPolicy<
        //     BlockFacade<elastica::rigid_body::RigidBodyPlugin<
        //        RBT, Block, Components...>>>
        public BlockFacade<
            elastica::rigid_body::RigidBodyPlugin<RBT, Block, Components...>> {
   private:
    //**Type definitions********************************************************
    //! Facade type
    using Facade = BlockFacade<
        elastica::rigid_body::RigidBodyPlugin<RBT, Block, Components...>>;
    //! Parent type
    // using Parent = ::elastica::SymplecticPolicy<Facade>;
    using Parent = Facade;
    //! Plugin type
    using Plugin = typename Parent::PluginType;
    //! This type
    using This = Block<Plugin>;
    //! This slice type
    using ThisSlice = BlockSlice<
        elastica::rigid_body::RigidBodyPlugin<RBT, BlockSlice, Components...>>;
    //! Plugin type for the slice
    using SlicePlugin = typename ThisSlice::PluginType;

   public:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = RBT;
    //! List of variables
    using typename Parent::Variables;
    //**************************************************************************

   protected:
    //! Template type of expected initializer
    template <typename... Initializers>
    using RigidBodyInitializer =
        // note : here it is not templated on the slice of the plugin but rather
        // the plugin itself
        ::blocks::BlockInitializer<Plugin, Initializers...>;
    //! Type of index
    using index_type = typename Traits::index_type;
    //! Type of size
    using size_type = typename Traits::size_type;
    //! Type of total element count
    using TotalElementCount = size_type;
    //**************************************************************************

   private:
    //**Friendships*************************************************************
    // Friend the Facade (for what?)
    friend Parent;
    // Friend the serialization struct
    friend elastica::io::Serialize<This>;
    //**************************************************************************

    //**Parent methods**********************************************************
    //! Methods inherited from parent class
   public:
    using Plugin::n_elements;
    using Plugin::total_n_elements;
    // for n_units() free function
    using Parent::n_units;
    //**************************************************************************

   public:
    //**************************************************************************
    /*!\name Start of slice */
    //@{
    //**************************************************************************
    /*!\brief Gets starting location of new slice, based on the stagger type
     */
    inline auto get_start_of_slice(index_type const unit) const noexcept
        -> size_type {
      return unit;
    }
    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets starting location of new slice, based on the stagger type
     */
    inline auto get_size_of_slice(index_type const) const noexcept
        -> size_type {
      return 1UL;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets starting location of new slice, based on the stagger type
     */
    inline auto get_size_of_slice(index_type const,
                                  size_type const slice_size) const noexcept
        -> size_type {
      return slice_size;
    }
    //**************************************************************************

   public:
    //**************************************************************************
    /*
     * \note
     * Most of the non-default public interface should more or less come from
     * the Cosserat Rod plugin class per design
     */
    //**************************************************************************

    //////////////////////////// start conformance /////////////////////////////
    // To work with block facade, implement customization point for generating
    // a slice.
    //**Slice functions*********************************************************
    /*!\name Slice functions */
    //@{
    /*!\brief Takes slice of each individual variable
     */
    template <typename Var>
    inline constexpr typename Var::slice_type slice(
        index_type index_to_be_sliced) & {
      return Var::slice(blocks::get<Var>(*this),
                        get_start_of_slice(index_to_be_sliced),
                        get_size_of_slice(index_to_be_sliced));
    }
    template <typename Var>
    inline constexpr typename Var::const_slice_type slice(
        index_type index_to_be_sliced) const& {
      return Var::slice(blocks::get<Var>(*this),
                        get_start_of_slice(index_to_be_sliced),
                        get_size_of_slice(index_to_be_sliced));
    }
    //**************************************************************************

    //**************************************************************************
    //@{
    /*!\brief Takes slice of each individual variable in a range
     */
    template <typename Var>
    inline constexpr typename Var::slice_type slice(index_type start_index,
                                                    size_type slice_size) & {
      return Var::slice(blocks::get<Var>(*this),
                        get_start_of_slice(start_index),
                        get_size_of_slice(start_index, slice_size));
    }
    template <typename Var>
    inline constexpr typename Var::const_slice_type slice(
        index_type start_index, size_type slice_size) const& {
      return Var::slice(blocks::get<Var>(*this),
                        get_start_of_slice(start_index),
                        get_size_of_slice(start_index, slice_size));
    }
    //**************************************************************************

    //@}
    //**************************************************************************

    //**Check functions*********************************************************
    /*!\name Check functions */
    //@{
    /*!\brief Checks whether indices are within range of accessible values
     */
    inline auto check(std::size_t index_to_be_sliced) const -> std::size_t {
      if (index_to_be_sliced < n_units()) {
        return index_to_be_sliced;
      } else {
        throw std::out_of_range(
            "Index to be sliced exceeds the number of slices inside a Rigid "
            "body block!");
      }
    }
    inline auto check(elastica::from_end index_to_be_sliced) const
        -> std::size_t {
      // cannot be 0
      const auto fe = index_to_be_sliced.i;
      if (fe <= n_units()) {
        return n_units() - fe;
      } else {
        throw std::out_of_range(
            "Index to be sliced from the end exceeds the number of slices "
            "inside a Rigid body block!");
      }
    };
    //**************************************************************************

    //@}
    //**************************************************************************
    ///////////////////////////// end conformance //////////////////////////////

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     */
    Block() : Parent(){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other Other block to move from
     */
    Block(Block&& other) noexcept : Parent(std::move(other)) {}
    //**************************************************************************

    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~Block() = default;
    //@}
    //**************************************************************************

   protected:
    //**Resizing functions******************************************************
    /*!\name Resizing functions */
    //@{

    //**************************************************************************
    /*!\brief Resizes all variables in the current Block
     *
     * \param total_number_of_units The total number of units/rods
     */
    void resize_internal(size_type const total_number_of_units) {
      tmpl::for_each<Variables>(
          [this, new_size = total_number_of_units](auto v) {
            using Variable = tmpl::type_from<decltype(v)>;
            auto& var = blocks::get<Variable>(*this);
            Variable::resize(var, new_size);
          });
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    //**Element buffer function************************************************
    /*!\brief Initialize the element buffer at a given location
     *
     * \param loc_n_units Fill location
     *
     * \note
     * This is mainly useful in testing
     */
    inline void initialize_elements_buffer(
        size_type const loc_n_units) noexcept {
      n_elements(loc_n_units) = get_size_of_slice(loc_n_units);
    }
    //**************************************************************************

   public:
    //**************************************************************************
    /*!\brief Emplaces a new rod to the block
     *
     * \details
     * This is the main entry-point for the Simulator to add new rods to the
     * current Block.
     *
     * \param block_initializer The initializer to fill in all elements of the
     * new block
     */
    template <typename... FirstBlockInitializers>
    auto emplace(
        RigidBodyInitializer<FirstBlockInitializers...>&& block_initializer)
        -> ThisSlice {
      // 0. number of preexisting rods
      const size_type curr_n_units = n_units();
      const size_type new_n_units = curr_n_units + 1UL;

      // 2. Resize the block to fit the new rod
      // 2.1 resize dofs on nodes, elements and voronoi by total_n_elements_
      // 2.2 resize dofs on rods by new_n_units
      resize_internal(new_n_units);

      // 3. The slicing logic (which calls check() to see if a buffer overflows)
      // requires that we know the number of elements that
      // needs to be sliced before making the slice itself, which is taken from
      // the elements buffer. So here we fill in the total_n_elements
      initialize_elements_buffer(curr_n_units);

      // after resizing the units should match
      ELASTICA_ASSERT(n_units() == new_n_units,
                      "Invariant violation, contact the developers");

      // after this step, we should have enough capacity for the n'th rod

      // 5. make a slice for the n'th unit (0 based indexing with checks)
      ThisSlice latest_slice(blocks::slice(*this, curr_n_units));

      // ... and initialize it ...
      Parent::initialize(latest_slice, std::move(block_initializer));

      // copied to the output buffer, NRVO should kick in
      return latest_slice;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Emplaces (back) a new rod to the block
     *
     * \param block_initializer The initializer to fill in all elements of the
     * new block
     *
     * \see emplace
     */
    template <typename... FirstBlockInitializers>
    inline decltype(auto) emplace_back(
        RigidBodyInitializer<FirstBlockInitializers...>&& block_initializer) {
      return emplace(std::move(block_initializer));
    }
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

}  // namespace blocks

namespace blocks {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of blocks::Block for a TaggedRigidBodyPlugin
   * \ingroup blocks
   *
   * \tparam RBT        The traits class for a Tagged Cosserat plugin
   * \tparam Tag        The tag for the Tagged Cosserat plugin
   * \tparam Components Components customizing a Rigid body (such as geometry,
   *                    elasticity)
   *
   * \see blocks::Block
   */
  template <typename RBT, typename Tag,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  class Block<elastica::rigid_body::TaggedRigidBodyPlugin<RBT, Block, Tag,
                                                          Components...>>
      : public Block<
            elastica::rigid_body::RigidBodyPlugin<RBT, Block, Components...>> {
   private:
    //**Type definitions********************************************************
    //! This type
    using This =
        Block<elastica::rigid_body::TaggedRigidBodyPlugin<RBT, Block, Tag,
                                                          Components...>>;
    //! This slice type
    using ThisSlice = BlockSlice<elastica::rigid_body::TaggedRigidBodyPlugin<
        RBT, BlockSlice, Tag, Components...>>;
    //! Parent type
    using Parent =
        Block<elastica::rigid_body::RigidBodyPlugin<RBT, Block, Components...>>;

   protected:
    //! Template type of expected initializer
    template <typename... Initializers>
    using RigidBodyInitializer =
        typename Parent::template RigidBodyInitializer<Initializers...>;
    //**************************************************************************

   public:
    //**************************************************************************
    /*!\brief Emplaces a new rod to the block
     *
     * \details
     * This is the main entry-point for the Simulator to add new rods to the
     * current Block.
     *
     * \param block_initializer The initializer to fill in all elements of the
     * new block
     */
    template <typename... FirstBlockInitializers>
    auto emplace(
        RigidBodyInitializer<FirstBlockInitializers...>&& block_initializer)
        -> ThisSlice {
      // converts to a tagged slice
      return Parent::emplace(std::move(block_initializer));
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Emplaces (back) a new rod to the block
     *
     * \param block_initializer The initializer to fill in all elements of the
     * new block
     *
     * \see emplace
     */
    template <typename... FirstBlockInitializers>
    inline decltype(auto) emplace_back(
        RigidBodyInitializer<FirstBlockInitializers...>&& block_initializer) {
      return This::emplace(std::move(block_initializer));
    }
    //**************************************************************************
  };
  //****************************************************************************

}  // namespace blocks

namespace blocks {

  //============================================================================
  //
  //  FREE FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Returns the size of the Cosserat Rod.
   * \ingroup rigid_body blocks
   *
   * \param rigid_body The current cosserat rod.
   * \return The  number of elements in the cosserat rod.
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline constexpr auto size(
      Block<::elastica::rigid_body::RigidBodyPlugin<
          RBT, Block, Components...>> const& rigid_body) noexcept {
    // it might be not the best idea to have a size() member in the first place
    return rigid_body.size();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Get number of units within a RigidBody
   * \ingroup rigid_body blocks
   *
   * \details
   * Get number of units from a Block. By 'units' we mean the number of
   * individual BlockSlice(s) composing the Block.
   *
   * \usage
   * \code
     Block<...> b;
     std::size_t n_units = blocks::n_units(b);
   * \endcode
   *
   * \param rigid_body The cosserat rod whose n_units is to be extracted
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline auto n_units(
      Block<::elastica::rigid_body::RigidBodyPlugin<
          RBT, Block, Components...>> const& rigid_body) noexcept
      -> std::size_t {
    return rigid_body.n_units();
  }
  //****************************************************************************

}  // namespace blocks
