//==============================================================================
/*!
//  \file
//  \brief Configuration for customizing blocking policy of Simulator
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
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
//
#include "Utilities/FakeVirtual.hpp"
#include "Utilities/Registration.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

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

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Abstract class for storing blocking policies within \elastica
       * \ingroup blocking
       *
       * \details
       * ForcingBase is an abstract class to access blocking during
       * runtime in \elastica.
       *
       * \tparam Registrars Registration parameter
       */
      template <typename BlockingPolicyRegistrars>
      class BlockingPolicyBase {
       public:
        //**********************************************************************
        //! Regisrars
        using registrars = BlockingPolicyRegistrars;
        //! All creatable (and callable) classes
        using creatable_classes =
            Registration::registrants<BlockingPolicyRegistrars>;
        //**********************************************************************

       protected:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         */
        BlockingPolicyBase() noexcept = default;
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief Virtual destructor
         */
        virtual ~BlockingPolicyBase() = default;
        //@}
        //**********************************************************************

        //**Blocking functions**************************************************
        /*!\name Blocking functions */
        //@{

        //**********************************************************************
        /*!\brief Gets an iterator to a block, onto which we add new systems to
         *
         * \param collection The collection of blocks::Block
         * \param new_dofs The degrees of freedom of the new block to be added
         * \return Iterator to a block element in \param collection
         */
        template <typename T>
        auto get_iterator_to_block(std::vector<T>& collection,
                                   std::size_t new_dofs) const {
          return call_with_dynamic_type<typename std::vector<T>::iterator,
                                        creatable_classes>(
              this, [&collection, new_dofs](auto* const blocking_policy) {
                return blocking_policy->get_iterator_to_block(collection,
                                                              new_dofs);
              });
        }
        //**********************************************************************
        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    namespace Registrars {
      /// Registers the `LinearSolver::Serial::ExplicitInverse` linear solver
      template <typename Criterion>
      using BlockingPolicy =
          Registration::Registrar<::elastica::configuration::BlockingPolicy,
                                  Criterion>;
    }  // namespace Registrars

    //**************************************************************************
    /*!\brief Policy class for blocking systems
     * \ingroup blocking
     *
     * \details
     * BlockingPolicy is the policy class to determine how systems added to an
     * \elastica simulator are to be distributed in blocks. Its intended use
     * is as a template parameter in configuration::SystemsConfiguration.
     *
     * \tparam Criterion_ The criterion used to select a new block
     *
     * \see make_blocking_policy() for idiomatic construction
     */
    template <typename Criterion_,
              typename Registrars =
                  tmpl::list<Registrars::BlockingPolicy<Criterion_>>>
    struct BlockingPolicy : public detail::BlockingPolicyBase<Registrars> {
      //**Type definitions******************************************************
      //! Criterion type
      using Criterion = Criterion_;
      //! Criteria argument
      struct CriterionParameter {
        using type = Criterion;
        static constexpr String help = {"Policy criteria parameter"};
      };
      //! List of options
      using options = tmpl::list<CriterionParameter>;
      //! Help string
      static constexpr String help = {"Blocking policy"};
      //************************************************************************

      //************************************************************************
      static std::string name() {
        // Ideally should use pretty_type::name<> but I didn't want to impose
        // another dependency
        return Criterion::name();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Default constructor (used in options)
       */
      explicit BlockingPolicy() : criterion_() {}
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       */
      explicit BlockingPolicy(Criterion c) : criterion_(std::move(c)) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Gets an iterator to a block, onto which we add new systems to
       *
       * \param collection The collection of blocks::Block
       * \param new_dofs The degrees of freedom of the new block to be added
       * \return Iterator to a block element in \a collection
       */
      template <typename T>
      auto get_iterator_to_block(std::vector<T>& collection,
                                 std::size_t new_dofs) const ->
          typename std::vector<T>::iterator {
        static_assert(std::is_default_constructible<T>::value,
                      R"error(
The vector type passed in does not meet requirements of a Block!
)error");
        const auto it =
            std::find_if(std::cbegin(collection), std::cend(collection),
                         [this, new_dofs](T const& t) -> bool {
                           return criterion_(t, new_dofs);
                         });

        if (it == std::cend(collection)) {
          collection.emplace_back();
          return (collection.end() - 1UL);
        } else {
          return collection.begin() + std::distance(collection.cbegin(), it);
        }
      }
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
      [[deprecated(
          "This code is intended for testing purposes only and will be "
          "changed/removed in a future version of Elastica")]]  //
      auto
      operator()(std::vector<T>& collection, std::size_t new_dofs) const -> T& {
        return *get_iterator_to_block(collection, new_dofs);
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
     * \see make_overloader
     * \see Documentation on @ref blocking for more policies
     */
    template <class FirstCriteria, class... Criteria,
              typename C = CompositeCriteria<FirstCriteria, Criteria...>>
    constexpr decltype(auto) make_blocking_policy(FirstCriteria criteria,
                                                  Criteria... crits) {
      using RT = BlockingPolicy<C>;
      return RT{C{std::move(criteria), std::move(crits)...}};
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Create a BlockingPolicy from Criteria
     * \ingroup blocking
     *
     * \see make_overloader
     */
    template <class Criteria>
    constexpr decltype(auto) make_blocking_policy(Criteria criteria) {
      using RT = BlockingPolicy<Criteria>;
      return RT{std::move(criteria)};
    }
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Restricts `Block` to an upper maximum degrees of freedom
     * \ingroup blocking
     *
     * \details
     * RestrictSize restricts a block to have equal or slightly less than
     * some maximum degrees of freedom
     *
     * \tparam Block A blocks::Block
     */
    template <typename Block>
    struct RestrictSize : public SingleSizePolicy<RestrictSize<Block>> {
      //**Type definitions******************************************************
      //! This type
      using This = RestrictSize<Block>;
      //! Parent type
      using Parent = SingleSizePolicy<This>;
      //! Inherit value
      using Parent::value;
      //! Inherit constructors
      using Parent::Parent;
      //************************************************************************

      //************************************************************************
      static std::string name() { return "RestrictSize"; }
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
     * LooselyRestrictSizeOf loosely restricts a block to have equal or
     * slightly more than some maximum degrees of freedom
     *
     * \tparam Block A blocks::Block
     */
    template <typename Block>
    struct LooselyRestrictSize
        : public SingleSizePolicy<LooselyRestrictSize<Block>> {
      //**Type
      // definitions******************************************************
      //! This type
      using This = LooselyRestrictSize<Block>;
      //! Parent type
      using Parent = SingleSizePolicy<This>;
      //! Inherit value
      using Parent::value;
      //! Inherit constructors
      using Parent::Parent;
      //************************************************************************

      //************************************************************************
      static std::string name() { return "LooselyRestrictSize"; }
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
      /*!\brief Default constructor.
       */
      explicit inline AlwaysNew(){};
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block
       */
      explicit inline AlwaysNew(const std::size_t limit) {
        (void)limit;
      }
      //************************************************************************

      //************************************************************************
      static std::string name() { return "AlwaysNew"; }
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \param meta Policy-specific size metadata (unused for AlwaysNew).
       * \return true if the current block passes the criteria
       */
      inline auto operator()(Block const& b,
                             const std::size_t meta) const noexcept
          -> bool {
        (void)b;
        (void)meta;
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
      /*!\brief Default constructor.
       */
      explicit inline AlwaysSame(){};
      //************************************************************************

      //************************************************************************
      /*!\brief Policy constructor.
       *
       * \param limit Max size per block, can be skipped
       */
      explicit inline AlwaysSame(const std::size_t limit) {
        (void)limit;
      }
      //************************************************************************

      //************************************************************************
      static std::string name() { return "AlwaysSame"; }
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \param meta Policy-specific size metadata (unused for AlwaysSame).
       * \return true if the current block passes the criteria
       */
      inline auto operator()(Block const& b,
                             const std::size_t meta) const noexcept
          -> bool {
        (void)b;
        (void)meta;
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
     * AcrossBlockTypes applies a policy template `SizeBasedRestriction`
     * across all blocks within a simulator.
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
      //! Inherit constructors
      using Parent::Parent;
      //! Help string for block types
      static constexpr String help = {"Policy across block types"};
      //************************************************************************

      //************************************************************************
      static std::string name() {
        return SizeBasedRestriction<int>::name() + "AcrossBlockTypes";
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \param new_dofs Degrees of freedom for the system being added
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
    using RestrictSizeAcrossBlockTypes = AcrossBlockTypes<RestrictSize>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Policy corresponding to RestrictSizeAcrossBlockTypes
     * \ingroup blocking
     */
    using RestrictSizeAcrossBlockTypesPolicy =
        BlockingPolicy<RestrictSizeAcrossBlockTypes>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Criterion for loosely restricting size to a maximum value,
     * across all block types \ingroup blocking
     *
     * \see LooselyRestrictSizeOf
     */
    using LooselyRestrictSizeAcrossBlockTypes =
        AcrossBlockTypes<LooselyRestrictSize>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Policy corresponding to LooselyRestrictSizeAcrossBlockTypes
     * \ingroup blocking
     */
    using LooselyRestrictSizeAcrossBlockTypesPolicy =
        BlockingPolicy<LooselyRestrictSizeAcrossBlockTypes>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Criterion for always requesting a new block across all block
     * types \ingroup blocking
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

    namespace detail {
      //************************************************************************
      using BlockingPolicyRegistrars = tmpl::list<
          Registrars::BlockingPolicy<RestrictSizeAcrossBlockTypes>,
          Registrars::BlockingPolicy<LooselyRestrictSizeAcrossBlockTypes>,
          Registrars::BlockingPolicy<AlwaysNewAcrossBlockTypes>,
          Registrars::BlockingPolicy<AlwaysSameAcrossBlockTypes>>;
      //************************************************************************

      template <typename SrcRegistrars, typename TargetRegistrars>
      static constexpr bool contained =
          (tmpl::size<
               tmpl::list_difference<SrcRegistrars, TargetRegistrars>>::value ==
           0U);

    }  // namespace detail

    //**************************************************************************
    /*!\brief Make blocking policy to be runtime polymorphic using registrars
     * \ingroup blocking
     */
    template <typename Registrars, class C, typename R>
    auto virtualize(BlockingPolicy<C, R> policy)
        -> std::unique_ptr<detail::BlockingPolicyBase<Registrars>> {
      // policy registrars should be a subset of pointer registrars
      // so {policy} - {pointer} = null set, whose size is always zero
      static_assert(detail::contained<R, Registrars>,
                    "Policy to be assigned needs to be registered by the "
                    "Registrar mechanism!");
      return std::make_unique<BlockingPolicy<C, Registrars>>(
          std::move(policy.criterion_));
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Make blocking policy to be runtime polymorphic using registrars
     * \ingroup blocking
     */
    template <class C, typename R>
    auto virtualize(BlockingPolicy<C, R> policy) {
      // append if it does not exist
      using registrars = tmpl::conditional_t<
          detail::contained<R, detail::BlockingPolicyRegistrars>,
          detail::BlockingPolicyRegistrars,
          tmpl::append<R, detail::BlockingPolicyRegistrars>>;
      return virtualize<registrars>(std::move(policy));
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
