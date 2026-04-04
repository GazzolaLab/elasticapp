//==============================================================================
/*!
//  \file
//  \brief Configuration for customizing iteration policy over systems in
//  Simulator
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
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
#include "Simulator/Configuration/ModuleConfiguration/common/common.hpp"
#include "Utilities/Size.hpp"

//******************************************************************************
/*!\defgroup iteration Iteration Policies
 * \ingroup config
 * \brief Iteration policies
 *
 * \details
 * Compile-time configuration for iteration strategies (for e.g how are you
 * going to iterate over the systems) for systems within a
 * ::elastica::interface::Simulator
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
    /*!\brief Policy class for systems iteration
     * \ingroup iteration
     *
     * \details
     * IterationPolicy is the policy class to determine how systems will be
     * iterated over on, within a \elastica simulator. Its
     * intended use is as a template parameter in
     * configuration::SystemsConfiguration.
     *
     * \tparam ExecutionPolicyOverSystemTypes The execution policy to be
     * followed while iterating over heterogeneous types of systems, see @ref
     * execution
     * \tparam ExecutionPolicyForEachSystemType The execution policy to be
     * followed over while iterating over a collection of homogeneous systems,
     * see @ref execution
     * \tparam Criterion The criterion used to iterate over blocks (if needed by
     * hybrid_policy)
     *
     * \see make_iteration_policy() for idiomatic construction
     */
    template <typename ExecutionPolicyOverSystemTypes,
              typename ExecutionPolicyForEachSystemType, typename CriteriaIfAny>
    struct IterationPolicy;
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of IterationPolicy for non-hybrid parallel policy
     * over systems
     * \ingroup iteration
     */
    template <typename ExecutionStrategyOverSystems,
              typename ExecutionStrategyForEachSystem, typename Criterion>
    struct IterationPolicy {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy over systems
      using ExecutionPolicyAcrossSystemTypes = ExecutionStrategyOverSystems;
      //! Type of execution policy for each system
      using ExecutionPolicyForEachSystemType = ExecutionStrategyForEachSystem;
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of IterationPolicy for a hybrid parallel policy
     * over systems
     * \ingroup iteration
     */
    template <typename ExecutionStrategyOverSystems, typename Criterion>
    struct IterationPolicy<ExecutionStrategyOverSystems, hybrid_policy,
                           Criterion> : public Criterion {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy over systems
      using ExecutionPolicyAcrossSystemTypes = ExecutionStrategyOverSystems;
      //! Type of execution policy for each system
      using ExecutionPolicyForEachSystemType = hybrid_policy;
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Policy constructor.
       */
      explicit IterationPolicy(Criterion c) noexcept
          : Criterion(std::move(c)){};
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    namespace detail {
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename T>
      struct IterateAcrossSystemTypesIn {
        using type = T;
      };

      template <typename T>
      struct IterateOverSystemIn {
        using type = T;
      };
      /*! \endcond */
      //************************************************************************
    }  // namespace detail

    //**************************************************************************
    /*!\brief Marks strategy for iterating across system types (i.e. in between
     * systems such as Cosserat rods, spheres etc). in sequence.
     * \ingroup iteration
     *
     * \details
     * Intended for use with make_iteration_policy().
     *
     * \example
     * \snippet Test_IterationPolicy.cpp seq_eg
     */
    inline constexpr auto iterate_across_system_types_in(
        decltype(seq) /* meta */) noexcept
        -> detail::IterateAcrossSystemTypesIn<sequential_policy> {
      return {};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Marks strategy for iterating across system types (i.e. in between
     * systems such as Cosserat rods, spheres etc). in parallel.
     * \ingroup iteration
     *
     * \details
     * Intended for use with make_iteration_policy().
     *
     * \example
     * \snippet Test_IterationPolicy.cpp mixed_eg
     */
    inline constexpr auto iterate_across_system_types_in(
        decltype(par) /* meta */) noexcept
        -> detail::IterateAcrossSystemTypesIn<parallel_policy> {
      return {};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Marks strategy for iterating over system of the same type
     * (i.e. for each system of the same type within a collection) using one of
     * the execution strategies.
     * \ingroup iteration
     *
     * \details
     * Intended for use with make_iteration_policy().
     *
     * \example
     * \snippet Test_IterationPolicy.cpp seq_eg
     * \snippet Test_IterationPolicy.cpp mixed_eg
     * \snippet Test_IterationPolicy.cpp hybrid_eg
     */
    template <class ExecutionPolicyForEachSystemType>
    inline constexpr auto iterate_over_systems_in(
        ExecutionPolicyForEachSystemType const& /* meta */) noexcept
        -> detail::IterateOverSystemIn<ExecutionPolicyForEachSystemType> {
      return {};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Create a IterationPolicy from Criteria
     * \ingroup iteration
     *
     * \details
     * Make an iteration policy with one or more criteria. This is useful when
     * the `ExecutionPolicyForEachSystemType` indicates a hybrid evaluation
     * strategy in which case we need a criterion (such as SerialBelow or
     * AllSerialBelow) to determine when to switch between sequential and
     * parallel strategies.
     * This is intended to be used with make_systems_configuration().
     *
     * \example
     * \snippet Test_IterationPolicy.cpp hybrid_eg
     *
     * \param criteria Thresholding Criterion for enabling hybrid parallelism
     * \param others Thresholding Criterion for enabling hybrid parallelism
     * \see make_overloader
     */
    template <class ExecutionPolicyAcrossSystemTypes, class FirstCriteria,
              class... Criteria,
              typename C = CompositeCriteria<FirstCriteria, Criteria...>>
    constexpr decltype(auto) make_iteration_policy(
        detail::IterateAcrossSystemTypesIn<
            ExecutionPolicyAcrossSystemTypes> /*meta*/,
        detail::IterateOverSystemIn<hybrid_policy> /*meta*/,
        FirstCriteria criteria, Criteria... others) noexcept {
      /*
       * If the client supplies hybrid to the middle policy and does not provide
       * any other functors, then there is a hard error as you cannot derive
       * from void
       */
      using RT =
          IterationPolicy<ExecutionPolicyAcrossSystemTypes, hybrid_policy, C>;
      return RT{C{std::move(criteria), std::move(others)...}};
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Create a IterationPolicy from Criteria
     * \ingroup iteration
     *
     * \see make_overloader
     */
    template <class ExecutionPolicyAcrossSystemTypes, class Criteria>
    constexpr decltype(auto) make_iteration_policy(
        detail::IterateAcrossSystemTypesIn<
            ExecutionPolicyAcrossSystemTypes> /*meta*/,
        detail::IterateOverSystemIn<hybrid_policy> /*meta*/,
        Criteria criteria) noexcept {
      /*
       * If the client supplies anything other than hybrid to the middle policy
       * provides any other functors, then there is a hard error as no
       * constructor is defined for taking in a Criteria type.
       */
      using RT = IterationPolicy<ExecutionPolicyAcrossSystemTypes,
                                 hybrid_policy, Criteria>;
      return RT{std::move(criteria)};
    }
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Creates an IterationPolicy
     * \ingroup iteration
     *
     * \details
     * Make an iteration policy without any criteria. This is useful when both
     * the `ExecutionPolicyAcrossSystemTypes` and
     * `ExecutionPolicyForEachSystemType` indicate either purely sequential or
     * purely parallel evaluation.
     * This is intended to be used with make_systems_configuration().
     *
     * \example
     * \snippet Test_IterationPolicy.cpp seq_eg
     * The arguments can also be mixed
     * \snippet Test_IterationPolicy.cpp mixed_eg
     *
     * \see make_overloader
     * \see Documentation on @ref iteration for all policies
     */
    template <class ExecutionPolicyAcrossSystemTypes,
              class ExecutionPolicyForEachSystemType>
    constexpr auto make_iteration_policy(
        detail::IterateAcrossSystemTypesIn<
            ExecutionPolicyAcrossSystemTypes> /*meta*/,
        detail::IterateOverSystemIn<
            ExecutionPolicyForEachSystemType> /*meta*/) noexcept
        -> IterationPolicy<ExecutionPolicyAcrossSystemTypes,
                           ExecutionPolicyForEachSystemType, void> {
      /*
       * If the client supplies hybrid to the middle policy and does not provide
       * any other functors, then there is a hard error as you cannot derive
       * from void
       */
      return {};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Serial iteration when the `Block` has less than specified elements
     * \ingroup iteration
     *
     * \tparam BlockCollection A blocks::Block
     */
    template <typename Block>
    struct SerialBelow : public SingleSizePolicy<SerialBelow<Block>> {
      //**Type definitions******************************************************
      //! This type
      using This = SerialBelow<Block>;
      //! Parent type
      using Parent = SingleSizePolicy<This>;
      //! Inherit value
      using Parent::value;
      //! Inherit constructors
      using Parent::Parent;
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \return true if the current block passes the criteria
       */
      inline auto operator()(std::vector<Block> const& b) const noexcept
          -> bool {
        return ::elastica::size(b) <= value();
      }
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Serial iteration for all `Blocks` has less than specified elements
     * \ingroup iteration
     *
     * \tparam BlockCollection A blocks::Block
     */
    struct AllSerialBelow : public SingleSizePolicy<AllSerialBelow> {
      //**Type definitions******************************************************
      //! This type
      using This = AllSerialBelow;
      //! Parent type
      using Parent = SingleSizePolicy<This>;
      //! Inherit value
      using Parent::value;
      //! Inherit constructors
      using Parent::Parent;
      //************************************************************************

      //************************************************************************
      /*!\brief Determines if current block passes the criteria
       *
       * \param b The current blocks::Block
       * \return true if the current block passes the criteria
       */
      template <typename Block>
      inline auto operator()(std::vector<Block> const& b) const noexcept
          -> bool {
        /* The following code is actually the best in terms of cleanliness, but
         * I dont' know if an object here is a good idea (even if the
         * constructors and destructors are explicitly inlined).
         */
        // return SerialBelow<Block>(value())(b, new_dofs);
        return ::elastica::size(b) <= value();
      }
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica