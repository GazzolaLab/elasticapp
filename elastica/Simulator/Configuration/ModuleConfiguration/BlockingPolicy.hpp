//==============================================================================
/*!
//  \file
//  \brief Configuration for customizing blocking policy of Simulator
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
#include <algorithm>
#include <cstddef>
#include <vector>

#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
#include "Simulator/Configuration/ModuleConfiguration/common/common.hpp"
#include "Utilities/Size.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup blocking Blocking Policies
 * \ingroup config
 * \brief Blocking policies
 *
 * \details
 * Compile-time configuration for blocking strategies (for e.g how many rods
 * should a block have) for systems within a ::elastica::interface::Simulator
 */
//******************************************************************************

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Policy class for blocking systems
     * \ingroup blocking
     *
     * \details
     * BlockingPolicy is the policy class to determine how systems added to an
     * \elastica simulator are to be distributed in blocks. Its intended use is
     * as a template parameter in configuration::SystemsConfiguration.
     *
     * \tparam Criterion The criterion used to select a new block
     *
     * \see make_blocking_policy() for idiomatic construction
     */
    template <typename Criterion>
    struct BlockingPolicy {
      //************************************************************************
      /*!\brief Policy constructor.
       */
      explicit BlockingPolicy(Criterion c) : criterion_(std::move(c)) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       */
      template <typename... Args>
      BlockingPolicy(Args&&... args)
          : BlockingPolicy(Criterion{std::forward<Args>(args)...}) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor.
       */
      BlockingPolicy(BlockingPolicy&&) noexcept = default;
      //************************************************************************

      //************************************************************************
      /*!\brief Determine which block to add new systems to
       *
       * \param collection The collection of blocks::Block
       * \param new_dofs The degrees of freedom of the new block to be added
       * \return Mutable reference to the block to which a new system is to be
       * added
       */
      template <typename T>
      auto operator()(std::vector<T>& collection, std::size_t new_dofs) const
          -> T& {
        static_assert(std::is_default_constructible<T>::value,
                      R"error(
The vector type passed in does not meet requirements of a Block!
)error");
        const auto it = std::find_if(collection.cbegin(), collection.cend(),
                                     [this, new_dofs](T const& t) -> bool {
                                       return criterion_(t, new_dofs);
                                     });

        // cannot use emplace_back because before cpp17 emplace_back() returns
        // void https://en.cppreference.com/w/cpp/container/vector/emplace_back
        // *collection.emplace(it) needs move/copy asssignable
        return (it == std::cend(collection))
                   ? (collection.emplace_back(), collection.back())
                   : const_cast<T&>(*it);
      }
      //************************************************************************

      //**Member variables******************************************************
      //! Criterion for determining blocking
      Criterion criterion_;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Create a BlockingPolicy from Criteria
     * \ingroup blocking
     *
     * \example
     * \snippet Test_BlockingPolicy.cpp make_blocking_eg
     *
     * \see make_overloader
     * \see Documentation on @ref blocking for more policies
     */
    template <class FirstCriteria, class... Criteria,
              typename C = CompositeCriteria<FirstCriteria, Criteria...>>
    constexpr auto make_blocking_policy(FirstCriteria criteria, Criteria... fs)
        -> BlockingPolicy<C> {
      return C{std::move(criteria), std::move(fs)...};
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Create a BlockingPolicy from Criteria
     * \ingroup blocking
     *
     * \see make_overloader
     */
    template <class Criteria, typename C = Criteria>
    constexpr auto make_blocking_policy(Criteria criteria)
        -> BlockingPolicy<C> {
      return C{std::move(criteria)};
    }
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Restricts `Block` to an upper maximum degrees of freedom
     * \ingroup blocking
     *
     * \details
     * RestrictSizeOf restricts a block to have equal or slightly less than some
     * maximum degrees of freedom
     *
     * \tparam Block A blocks::Block
     */
    template <typename Block>
    struct RestrictSizeOf : public SingleSizePolicy<RestrictSizeOf<Block>> {
      //**Type definitions******************************************************
      //! This type
      using This = RestrictSizeOf<Block>;
      //! Parent type
      using Parent = SingleSizePolicy<This>;
      //! Inherit value
      using Parent::value;
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block
       */
      explicit inline RestrictSizeOf(const std::size_t limit) : Parent(limit){};
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \param new_dofs The degrees of freedom of the new system to be added
       * \return true if the current block passes the criteria
       */
      inline auto operator()(Block const& b,
                             const std::size_t new_dofs) const noexcept
          -> bool {
        return (::elastica::size(b) + new_dofs) <= value();
      }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Loosely Restricts `Block` to some maximum degrees of freedom
     * \ingroup blocking
     *
     * \details
     * LooselyRestrictSizeOf loosely restricts a block to have equal or slightly
     * more than some maximum degrees of freedom
     *
     * \tparam Block A blocks::Block
     */
    template <typename Block>
    struct LooselyRestrictSizeOf
        : public SingleSizePolicy<LooselyRestrictSizeOf<Block>> {
      //**Type definitions******************************************************
      //! This type
      using This = LooselyRestrictSizeOf<Block>;
      //! Parent type
      using Parent = SingleSizePolicy<This>;
      //! Inherit value
      using Parent::value;
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block
       */
      explicit inline LooselyRestrictSizeOf(const std::size_t limit)
          : Parent(limit){};
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \return true if the current block passes the criteria
       */
      inline auto operator()(Block const& b,
                             const std::size_t /*meta*/) const noexcept
          -> bool {
        return ::elastica::size(b) <= value();
      }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Always request a new block
     * \ingroup blocking
     *
     * \details
     * AlwaysNew always requests a new block to be made irrespective of any
     * criteria.
     *
     * \tparam Block A blocks::Block
     */
    template <typename Block>
    struct AlwaysNew {
      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block
       */
      explicit inline AlwaysNew(const std::size_t = 0UL){};
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \return true if the current block passes the criteria
       */
      inline auto operator()(Block const& /*b*/,
                             const std::size_t /*meta*/) const noexcept
          -> bool {
        return false;
      }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Always return the same block
     * \ingroup blocking
     *
     * \details
     * AlwaysSame always requests the system to be added to the same block
     * irrespective of any criteria.
     *
     * \tparam Block A blocks::Block
     */
    template <typename Block>
    struct AlwaysSame {
      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block, can be skipped
       */
      explicit inline AlwaysSame(const std::size_t = 0UL){};
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \return true if the current block passes the criteria
       */
      inline auto operator()(Block const& /*b*/,
                             const std::size_t /*meta*/) const noexcept
          -> bool {
        return true;
      }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Helper to compose blocking policies across all Block types
     * \ingroup blocking
     *
     * \details
     * AcrossBlockTypes applies a policy template `SizeBasedRestriction` across
     * all blocks within a simulator.
     *
     * \tparam SizeBasedRestriction Policy template restricting blocks
     */
    template <template <typename> class SizeBasedRestriction>
    struct AcrossBlockTypes
        : public SingleSizePolicy<AcrossBlockTypes<SizeBasedRestriction>> {
      //**Type definitions******************************************************
      //! This type
      using This = AcrossBlockTypes<SizeBasedRestriction>;
      //! Parent type
      using Parent = SingleSizePolicy<This>;
      //! Inherit value
      using Parent::value;
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block
       */
      explicit inline AcrossBlockTypes(const std::size_t limit)
          : Parent(limit){};
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \return true if the current block passes the criteria
       */
      template <typename Block>
      inline auto operator()(Block const& b,
                             const std::size_t new_dofs) const noexcept
          -> bool {
        return SizeBasedRestriction<Block>(value())(b, new_dofs);
      }
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Criterion for restricting size to a maximum value, across all
     * block types
     * \ingroup blocking
     *
     * \see RestrictSizeOf
     */
    using RestrictSizeAcrossBlockTypes = AcrossBlockTypes<RestrictSizeOf>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Policy corresponding to RestrictSizeAcrossBlockTypes
     * \ingroup blocking
     */
    using RestrictSizeAcrossBlockTypesPolicy =
        BlockingPolicy<RestrictSizeAcrossBlockTypes>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Criterion for loosely restricting size to a maximum value, across
     * all block types
     * \ingroup blocking
     *
     * \see LooselyRestrictSizeOf
     */
    using LooselyRestrictSizeAcrossBlockTypes =
        AcrossBlockTypes<LooselyRestrictSizeOf>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Policy corresponding to LooselyRestrictSizeAcrossBlockTypes
     * \ingroup blocking
     */
    using LooselyRestrictSizeAcrossBlockTypesPolicy =
        BlockingPolicy<LooselyRestrictSizeAcrossBlockTypes>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Criterion for always requesting a new block across all block types
     * \ingroup blocking
     *
     * \see AlwaysNew
     */
    using AlwaysNewAcrossBlockTypes = AcrossBlockTypes<AlwaysNew>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Policy corresponding to AlwaysNewAcrossBlockTypes
     * \ingroup blocking
     */
    using AlwaysNewAcrossBlockTypesPolicy =
        BlockingPolicy<AlwaysNewAcrossBlockTypes>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Criterion for always requesting the same block across all block
     * types
     * \ingroup blocking
     *
     * \see AlwaysSame
     */
    using AlwaysSameAcrossBlockTypes = AcrossBlockTypes<AlwaysSame>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Policy corresponding to AlwaysSameAcrossBlockTypes
     * \ingroup blocking
     */
    using AlwaysSameAcrossBlockTypesPolicy =
        BlockingPolicy<AlwaysSameAcrossBlockTypes>;
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
