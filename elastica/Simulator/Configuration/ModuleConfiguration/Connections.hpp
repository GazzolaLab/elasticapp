//==============================================================================
/*!
//  \file
//  \brief Configuration for the Connections module of an \elastica Simulator
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
#include "Simulator/Configuration/Protocols/Connections.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"  // for registrants

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Connections.hpp"  // for registrars

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration for the Connections module
     * \ingroup config
     *
     * \details
     * ConnectionsConfiguration denotes the compile-time customization point
     * of ::elastica::modules::Connections of an
     * ::elastica::interface::Simulator. It offers the ability to customize the
     * simulation with execution strategies for Connection algorithms, via the
     * template parameter.
     *
     * \tparam ExecutionPolicy Tag policy for execution of connection algorithms
     * one of ExecutionPolicies in @ref execution
     * \tparam Registrars Registrars for runtime support
     *
     * \see make_connections_configuration() for idiomatic construction
     */
    template <typename ExecutionPolicy, typename Registrars>
    class ConnectionsConfiguration;
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Definition of connections configuration for fully
     * sequential/parallel configurations
     * \ingroup config
     */
    template <typename ExecutionPolicy, typename Registrars>
    class ConnectionsConfiguration
        : public ::tt::ConformsTo<protocols::ConnectionsConfiguration> {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy
      using ConnectionsExecutionPolicy = ExecutionPolicy;
      //! Type of registrars
      using ConnectionsRegistrars = Registrars;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! This type
      using This = ConnectionsConfiguration<ConnectionsExecutionPolicy,
                                            ConnectionsRegistrars>;
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors*/
      //@{
     public:
      constexpr ConnectionsConfiguration() noexcept = default;

     public:
      constexpr ConnectionsConfiguration(
          ConnectionsConfiguration const&) noexcept = default;
      constexpr ConnectionsConfiguration(ConnectionsConfiguration&&) noexcept =
          default;
      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor*/
      //@{
      ~ConnectionsConfiguration() = default;
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
    class ConnectionsConfiguration<hybrid_policy, Registrars>
        : public ::tt::ConformsTo<protocols::ConnectionsConfiguration> {
     public:
      //**Type definitions******************************************************
      //! Type of execution policy
      using ConnectionsExecutionPolicy = hybrid_policy;
      //! Type of registrars
      using ConnectionsRegistrars = Registrars;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! This type
      using This = ConnectionsConfiguration<ConnectionsExecutionPolicy,
                                            ConnectionsRegistrars>;
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors*/
      //@{
     public:
      inline constexpr explicit ConnectionsConfiguration(
          std::size_t const parallel_threshold) noexcept
          : threshold(parallel_threshold){};
      constexpr ConnectionsConfiguration() = delete;

     public:
      constexpr ConnectionsConfiguration(
          ConnectionsConfiguration const&) noexcept = default;
      constexpr ConnectionsConfiguration(ConnectionsConfiguration&&) noexcept =
          default;
      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Obtains the threshold for parallelism
       */
      inline auto constexpr connections_parallelism_threshold() const noexcept
          -> std::size_t {
        return This::threshold;
      }
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor*/
      //@{
      ~ConnectionsConfiguration() = default;
      //@}
      //************************************************************************
      //

     private:
      //**Member variables******************************************************
      //! Threshold for parallelism
      std::size_t const threshold;
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a connection configuration idiomatically
     * \ingroup config
     *
     * \param p parallel execution policy for the configuration, one of the
     * types in @ref execution
     * \param threshold Threshold beyond which
     * parallelism is enabled, only used for a hybrid execution policy
     *
     * \example
     * \snippet ModuleConfiguration/Test_Connections.cpp seq_eg
     * \snippet ModuleConfiguration/Test_Connections.cpp par_eg
     * \snippet ModuleConfiguration/Test_Connections.cpp hybrid_eg
     *
     * \tparam ConnectionsRegistrars Registrars for runtime support
     *
     * \see ConnectionsConfiguration
     */
    template <typename ConnectionsRegistrars =
                  ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS,
              typename ConnectionsExecutionPolicy>
    inline constexpr decltype(auto) make_connections_configuration(
        ConnectionsExecutionPolicy const& p,
        const std::size_t threshold = 0UL) noexcept {
      elastica::ignore_unused(threshold);
      return detail::make_executed<ConnectionsConfiguration,
                                   ConnectionsRegistrars>(p);
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename ConnectionsRegistrars =
                  ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS>
    inline constexpr decltype(auto) make_connections_configuration(
        decltype(hybrid) p, const std::size_t threshold) noexcept {
      return detail::make_executed<ConnectionsConfiguration,
                                   ConnectionsRegistrars>(p, threshold);
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
