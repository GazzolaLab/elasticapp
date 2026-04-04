//==============================================================================
/*!
//  \file
//  \brief Configuration for the Forcing module of an \elastica Simulator
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
#include <cstddef>  // std::size_t

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/Protocols/Forcing.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"  // for registrants

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Forcing.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration for the Forcing module
     * \ingroup config
     *
     * \details
     * ForcingConfiguration denotes the compile-time customization point of the
     * ::elastica::modules::Forcing of an
     * ::elastica::interface::Simulator. It offers the ability to customize the
     * simulation with execution strategies for Connection algorithms, via the
     * template parameter.
     *
     * \tparam ExecutionPolicy Tag policy for execution of connection algorithms
     * one of Execution Policies in @ref execution
     * \tparam Registrars Registrars for runtime support
     *
     * \see make_forcing_configuration() for idiomatic construction
     */
    template <typename ExecutionPolicy, typename Registrars>
    class ForcingConfiguration;
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Definition of connections configuration for fully
     * sequential/parallel configurations
     * \ingroup config
     */
    template <typename ExecutionPolicy, typename Registrars>
    class ForcingConfiguration
        : public ::tt::ConformsTo<protocols::ForcingConfiguration> {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy
      using ForcingExecutionPolicy = ExecutionPolicy;
      //! Type of registrars
      using ForcingRegistrars = Registrars;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! This type
      using This =
          ForcingConfiguration<ForcingExecutionPolicy, ForcingRegistrars>;
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
    class ForcingConfiguration<hybrid_policy, Registrars>
        : public ::tt::ConformsTo<protocols::ForcingConfiguration> {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy
      using ForcingExecutionPolicy = hybrid_policy;
      //! Type of registrars
      using ForcingRegistrars = Registrars;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! This type
      using This =
          ForcingConfiguration<ForcingExecutionPolicy, ForcingRegistrars>;
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors*/
      //@{
     public:
      inline constexpr explicit ForcingConfiguration(
          std::size_t const parallel_threshold) noexcept
          : threshold(parallel_threshold){};
      constexpr ForcingConfiguration() = delete;
      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Obtains the threshold for parallelism
       */
      inline auto constexpr forcing_parallelism_threshold() const noexcept
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
    /*!\brief Makes a forcing configuration
     * \ingroup config
     *
     * \param p parallel execution policy for the configuration, one of the
     * types in @ref execution
     * \param threshold Threshold beyond which parallelism is enabled, for
     * a hybrid execution policy
     *
     * \example
     * \snippet ModuleConfiguration/Test_Forcing.cpp seq_eg
     * \snippet ModuleConfiguration/Test_Forcing.cpp par_eg
     * \snippet ModuleConfiguration/Test_Forcing.cpp hybrid_eg
     *
     * \tparam ForcingRegistrars Registrars for runtime support
     *
     * \see ForcingConfiguration
     */
    template <
        typename ForcingRegistrars = ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING,
        typename ForcingExecutionPolicy>
    inline constexpr decltype(auto) make_forcing_configuration(
        ForcingExecutionPolicy const& p,
        const std::size_t threshold = 0UL) noexcept {
      elastica::ignore_unused(threshold);
      return detail::make_executed<ForcingConfiguration, ForcingRegistrars>(p);
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <
        typename ForcingRegistrars = ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING>
    inline constexpr decltype(auto) make_forcing_configuration(
        decltype(hybrid) p, const std::size_t threshold) noexcept {
      return detail::make_executed<ForcingConfiguration, ForcingRegistrars>(
          p, threshold);
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
