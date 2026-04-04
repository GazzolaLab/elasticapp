//==============================================================================
/*!
//  \file
//  \brief Configuration for customizing Execution policies of Simulator modules
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
#include <cstddef> // size_t

#include "Utilities/StaticConst.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup execution Algorithm Execution
 * \ingroup config
 * \brief Algorithm execution policies
 *
 * \details
 * Compile-time configuration for execution strategies (serial, parallel etc)
 * for algorithmic sub-components within a ::elastica::interface::Simulator
 */
//******************************************************************************

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**Execution policies******************************************************
    /*!\name ExecutionPolicies*/
    //@{

    //**************************************************************************
    /*!\brief A serial execution policy
     * \ingroup execution
     *
     * \details
     * The execution policy type used as a unique type to disambiguate parallel
     * algorithm overloading and require that a parallel algorithm's execution
     * may not be parallelized.
     */
    struct sequential_policy {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief A parallel execution policy
     * \ingroup execution
     *
     * \details
     * The execution policy type used as a unique type to disambiguate parallel
     * algorithm overloading and indicate that a parallel algorithm's execution
     * may be parallelized.
     */
    struct parallel_policy {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief A parallel execution policy
     * \ingroup execution
     *
     * \details
     * The execution policy type used as a unique type to disambiguate parallel
     * algorithm overloading and indicate that a parallel algorithm's execution
     * is hybridized (i.e. has mixed sequential and parallel execution modes),
     * based on the data cardinality/size.
     */
    struct hybrid_policy {};
    //**************************************************************************

    // gpu
    // mpi? here doesnt make much sense though

    //@}
    //**************************************************************************

    namespace {
      //************************************************************************
      /*!\brief Global instance of a elastica::configuration::sequential_policy
       * \ingroup execution
       *
       * \details
       * Used idiomatically as a function parameter to specify the execution
       * policy of algorithms in sequence
       *
       * \example
       * With the setup shown in
       * \snippet ModuleConfiguration/Test_ExecutionPolicy.cpp execution_setup
       * the following code performs sequential execution
       * \snippet ModuleConfiguration/Test_ExecutionPolicy.cpp seq_eg
       */
      constexpr auto const& seq = static_const<sequential_policy>::value;
      //************************************************************************

      //************************************************************************
      /*!\brief Instance of a elastica::configuration::parallel_policy
       * \ingroup execution
       *
       * \details
       * Used idiomatically as a function parameter to specify the execution
       * policy of algorithms in parallel
       *
       * \example
       * With the setup shown in
       * \snippet ModuleConfiguration/Test_ExecutionPolicy.cpp execution_setup
       * the following code performs parallel execution
       * \snippet ModuleConfiguration/Test_ExecutionPolicy.cpp par_eg
       */
      constexpr auto const& par = static_const<parallel_policy>::value;
      //************************************************************************

      //************************************************************************
      /*!\brief Instance of a elastica::configuration::hybrid_policy
       * \ingroup execution
       *
       * \details
       * Used idiomatically as a function parameter to specify the execution
       * policy of algorithms in hybrid (sequential/parallel), possibly based on
       * some thresholds
       *
       * \example
       * With the setup shown in
       * \snippet ModuleConfiguration/Test_ExecutionPolicy.cpp execution_setup
       * the following code performs hybrid execution
       * \snippet ModuleConfiguration/Test_ExecutionPolicy.cpp hybrid_eg
       */
      constexpr auto const& hybrid = static_const<hybrid_policy>::value;
      //************************************************************************
    }  // namespace

    namespace detail {
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <template <typename, typename...>
                class ConfigurationToBeConstructed,
                typename... ConfigurationMetaArgs>
      inline constexpr decltype(auto) make_executed(
          ::elastica::configuration::sequential_policy const&) noexcept {
        using RT = ConfigurationToBeConstructed<
            ::elastica::configuration::sequential_policy,
            ConfigurationMetaArgs...>;
        return RT{};
      }

      template <template <typename, typename...>
                class ConfigurationToBeConstructed,
                typename... ConfigurationMetaArgs>
      inline constexpr decltype(auto) make_executed(
          ::elastica::configuration::parallel_policy const&) noexcept {
        using RT = ConfigurationToBeConstructed<
            ::elastica::configuration::parallel_policy,
            ConfigurationMetaArgs...>;
        return RT{};
      }

      template <template <typename, typename...>
                class ConfigurationToBeConstructed,
                typename... ConfigurationMetaArgs>
      inline constexpr decltype(auto) make_executed(
          ::elastica::configuration::hybrid_policy const&,
          const std::size_t threshold) noexcept {
        using RT = ConfigurationToBeConstructed<
            ::elastica::configuration::hybrid_policy, ConfigurationMetaArgs...>;
        return RT{threshold};
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace configuration

}  // namespace elastica
