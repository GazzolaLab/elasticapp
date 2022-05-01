//==============================================================================
/*!
//  \file
//  \brief Specialization of a Block slice for Cosserat rods
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
#include "Systems/CosseratRods/Types.hpp"
///
#include "Systems/Block/Block.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Utilities/End.hpp"  // from_end

namespace blocks {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of a slice/view on a Block for Cosserat rods
   * \ingroup blocks
   *
   * \tparam CRT        The traits class for a Cosserat rod
   * \tparam Components Components customizing a Cosserat rod (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename CRT,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  class BlockSlice<
      elastica::cosserat_rod::CosseratRodPlugin<CRT, BlockSlice, Components...>>
      : public detail::BlockSliceFacade<
            elastica::cosserat_rod::CosseratRodPlugin<CRT, BlockSlice,
                                                      Components...>> {
   private:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = CRT;
    //! Type of the parent plugin
    using Parent =
        detail::BlockSliceFacade<elastica::cosserat_rod::CosseratRodPlugin<
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
    //**Type definitions********************************************************
    //! Size type
    using size_type = typename Traits::size_type;
    //! Index type
    using index_type = typename Traits::index_type;
    //**************************************************************************

   public:
    //////////////////////////// start conformance /////////////////////////////

    //**Check functions*********************************************************
    /*!\name Check functions */
    //@{
    /*!\brief Checks whether indices are within range of accessible values
     */
    inline auto check(std::size_t index_to_be_sliced) const -> std::size_t {
      constexpr index_type current_rod_index(0UL);
      std::size_t max_index(PluginType::n_elements(current_rod_index));
      if (index_to_be_sliced < max_index) {
        return index_to_be_sliced;
      } else {
        throw std::out_of_range(
            "Index to be sliced exceeds the number of dofs inside a Cosserat "
            "rod blockslice!");
      }
    }
    inline auto check(elastica::from_end index_to_be_sliced) const
        -> std::size_t {
      // cannot be 0
      const auto fe = index_to_be_sliced.i;
      constexpr index_type current_rod_index(0UL);
      std::size_t max_index(PluginType::n_elements(current_rod_index));
      if (fe <= max_index) {
        return max_index - fe;
      } else {
        throw std::out_of_range(
            "Index to be sliced from the end exceeds the number of dogs inside "
            "a Cosserat rod blocksize!");
      }
    };
    //**************************************************************************

    //@}
    //**************************************************************************

    ///////////////////////////// end conformance //////////////////////////////

    //**Ghosting functions******************************************************
    /*!\name Ghosting functions */
    //@{

    //**************************************************************************
    /*!\brief Fills ghosts for a variable of tag `Tag` in the current Block
     * Needed because some of the kernels use it
     */
    template <typename Tag>
    void fill_ghosts_for() const noexcept {}
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

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of a slice/view on a const Block for Cosserat rods
   * \ingroup blocks
   *
   * \tparam CRT        The traits class for a Cosserat rod
   * \tparam Components Components customizing a Cosserat rod (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename CRT,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  class ConstBlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
      CRT, ConstBlockSlice, Components...>>
      : public detail::ConstBlockSliceFacade<
            elastica::cosserat_rod::CosseratRodPlugin<CRT, ConstBlockSlice,
                                                      Components...>> {
   private:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = CRT;
    //! Type of the parent plugin
    using Parent =
        detail::ConstBlockSliceFacade<elastica::cosserat_rod::CosseratRodPlugin<
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
    //**Type definitions********************************************************
    //! Size type
    using size_type = typename Traits::size_type;
    //! Index type
    using index_type = typename Traits::index_type;
    //**************************************************************************

   public:
    //////////////////////////// start conformance /////////////////////////////

    //**Check functions*********************************************************
    /*!\name Check functions */
    //@{
    /*!\brief Checks whether indices are within range of accessible values
     */
    inline auto check(std::size_t index_to_be_sliced) const -> std::size_t {
      constexpr index_type current_rod_index(0UL);
      std::size_t max_index(PluginType::n_elements(current_rod_index));
      if (index_to_be_sliced < max_index) {
        return index_to_be_sliced;
      } else {
        throw std::out_of_range(
            "Index to be sliced exceeds the number of rods inside a Cosserat "
            "rod block!");
      }
    }
    inline auto check(elastica::from_end index_to_be_sliced) const
        -> std::size_t {
      // cannot be 0
      const auto fe = index_to_be_sliced.i;
      constexpr index_type current_rod_index(0UL);
      std::size_t max_index(PluginType::n_elements(current_rod_index));
      if (fe <= max_index) {
        return max_index - fe;
      } else {
        throw std::out_of_range(
            "Index to be sliced from the end exceeds the number of rods inside "
            "a Cosserat rod block!");
      }
    };
    //**************************************************************************

    //@}
    //**************************************************************************

    ///////////////////////////// end conformance //////////////////////////////

    //**Ghosting functions******************************************************
    /*!\name Ghosting functions */
    //@{

    //**************************************************************************
    /*!\brief Fills ghosts for a variable of tag `Tag` in the current Block
     * Needed because some of the kernels use it
     */
    template <typename Tag>
    void fill_ghosts_for() const noexcept {}
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
  /*!\brief Specialization of a slice/view on a Block for Tagged Cosserat rods
   * \ingroup blocks
   *
   * \tparam CRT        The traits class for a Cosserat rod
   * \tparam Tag        The tag for the Tagged Cosserat plugin
   * \tparam Components Components customizing a Cosserat rod (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename CRT, typename Tag,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  class BlockSlice<elastica::cosserat_rod::TaggedCosseratRodPlugin<
      CRT, BlockSlice, Tag, Components...>>
      : public BlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
            CRT, BlockSlice, Components...>> {
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
    using Traits = CRT;
    //! Type of the parent plugin
    using Parent =
        BlockSlice<elastica::cosserat_rod::CosseratRodPlugin<Traits, BlockSlice,
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
  /*!\brief Specialization of a const slice/view on a Block for Tagged Cosserat
   * rods
   * \ingroup blocks
   *
   * \tparam CRT        The traits class for a Cosserat rod
   * \tparam Tag        The tag for the Tagged Cosserat plugin
   * \tparam Components Components customizing a Cosserat rod (such as geometry,
   *                    elasticity)
   *
   * \see Block
   */
  template <typename CRT, typename Tag,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  class ConstBlockSlice<elastica::cosserat_rod::TaggedCosseratRodPlugin<
      CRT, ConstBlockSlice, Tag, Components...>>
      : public ConstBlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
            CRT, ConstBlockSlice, Components...>> {
   private:
    //**Type definitions********************************************************
    //! Traits type
    using Traits = CRT;
    //! Type of the parent plugin
    using Parent = ConstBlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
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
  /*!\brief Returns the size of the Cosserat Rod.
   * \ingroup cosserat_rod blocks
   *
   * \param cosserat_rod The current cosserat rod.
   * \return The  number of elements in the cosserat rod.
   */
  template <typename CRT,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  inline constexpr auto size(
      BlockSlice<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, BlockSlice, Components...>> const& cosserat_rod) noexcept {
    // it might be not the best idea to have a size() member
    return cosserat_rod.size();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns the size of the Cosserat Rod.
   * \ingroup cosserat_rod blocks
   *
   * \param cosserat_rod The current cosserat rod.
   * \return The  number of elements in the cosserat rod.
   */
  template <typename CRT,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  inline constexpr auto size(
      ConstBlockSlice<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ConstBlockSlice, Components...>> const& cosserat_rod) noexcept {
    // it might be not the best idea to have a size() member
    return cosserat_rod.size();
  }
  //****************************************************************************

}  // namespace blocks