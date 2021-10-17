//==============================================================================
/*!
//  \file
//  \brief Configuration for the Constraints module of an \elastica Simulator
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
#include <cstddef>  // std::size_t

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/Protocols/Constraints.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"  // for registrants

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Constraints.hpp"  // for registrars

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration for the Constraints module
     * \ingroup config
     *
     * \details
     * ConstraintsConfiguration denotes the compile-time customization point
     * of the ::elastica::modules::Constraints of an
     * ::elastica::interface::Simulator. It offers the ability to customize the
     * simulation with execution strategies for Connection algorithms, via the
     * template parameter.
     *
     * \tparam ExecutionPolicy Tag policy for execution of connection algorithms
     * one of ExecutionPolicies in @ref execution
     * \tparam Registrars Registrars for runtime support
     *
     * \see make_constraints_configuration() for idiomatic construction
     */
    template <typename ExecutionPolicy, typename Registrars>
    class ConstraintsConfiguration;
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Definition of connections configuration for fully
     * sequential/parallel configurations
     * \ingroup config
     */
    template <typename ExecutionPolicy, typename Registrars>
    class ConstraintsConfiguration
        : public ::tt::ConformsTo<protocols::ConstraintsConfiguration> {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy
      using ConstraintsExecutionPolicy = ExecutionPolicy;
      //! Type of registrars
      using ConstraintsRegistrars = Registrars;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! This type
      using This = ConstraintsConfiguration<ConstraintsExecutionPolicy,
                                            ConstraintsRegistrars>;
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors*/
      //@{
     public:
      constexpr ConstraintsConfiguration() noexcept = default;
      //@}
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Definition of connections configuration for a hybrid configuration
     * \ingroup config
     */
    template <typename Registrars>
    class ConstraintsConfiguration<hybrid_policy, Registrars>
        : public ::tt::ConformsTo<protocols::ConstraintsConfiguration> {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy
      using ConstraintsExecutionPolicy = hybrid_policy;
      //! Type of registrars
      using ConstraintsRegistrars = Registrars;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! This type
      using This = ConstraintsConfiguration<ConstraintsExecutionPolicy,
                                            ConstraintsRegistrars>;
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors*/
      //@{
     public:
      inline constexpr explicit ConstraintsConfiguration(
          std::size_t const parallel_threshold) noexcept
          : threshold(parallel_threshold){};
      constexpr ConstraintsConfiguration() = delete;
      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Obtains the threshold for parallelism
       */
      inline auto constexpr constraints_parallelism_threshold() const noexcept
          -> std::size_t {
        return This::threshold;
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! Threshold for parallelism
      std::size_t const threshold;
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a constraint configuration
     * \ingroup config
     *
     * \param p parallel execution policy for the configuration, one of the
     * types in @ref execution
     * \param threshold Threshold beyond which parallelism is enabled, for
     * a hybrid execution policy
     *
     * \example
     * \snippet ModuleConfiguration/Test_Constraints.cpp seq_eg
     * \snippet ModuleConfiguration/Test_Constraints.cpp par_eg
     * \snippet ModuleConfiguration/Test_Constraints.cpp hybrid_eg
     *
     * \tparam ConstraintsRegistrars Registrars for runtime support
     *
     * \see ConstraintsConfiguration
     */
    template <typename ConstraintsRegistrars =
                  ELASTICA_DEFAULT_REGISTRARS_FOR_CONSTRAINTS,
              typename ConstraintsExecutionPolicy>
    inline constexpr decltype(auto) make_constraints_configuration(
        ConstraintsExecutionPolicy const& p,
        const std::size_t threshold = 0UL) noexcept {
      elastica::ignore_unused(threshold);
      return detail::make_executed<ConstraintsConfiguration,
                                   ConstraintsRegistrars>(p);
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename ConstraintsRegistrars =
                  ELASTICA_DEFAULT_REGISTRARS_FOR_CONSTRAINTS>
    inline constexpr decltype(auto) make_constraints_configuration(
        decltype(hybrid) p, const std::size_t threshold) noexcept {
      return detail::make_executed<ConstraintsConfiguration,
                                   ConstraintsRegistrars>(p, threshold);
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
