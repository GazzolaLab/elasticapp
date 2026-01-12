//==============================================================================
/*!
//  \file
//  \brief Configuration for the Environments module of an \elastica Simulator
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

#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
#include "Simulator/Configuration/Protocols/Environments.hpp"
#include "Utilities/ProtocolHelpers.hpp"
///
#include "Simulator/Environments/Primitives/Types.hpp"  // needed for template instantiation
#include "Simulator/Environments/Tags.hpp"  // needed for template instantiation
#include "Utilities/TMPL.hpp"               // tags are type lists

// internal module configurations
#include "Simulator/Environments/ModuleConfiguration/ModuleConfiguration.hpp"

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Environments.hpp"  // for tags

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration of environment within the Simulator module
     * \ingroup config
     *
     * \details
     * EnvironmentsConfiguration denotes the compile-time customization point of
     * the Environments (such as Collision detection) included within an
     * ::elastica::interface::Simulator. It offers the ability to customize the
     * simulation with environments and static bodies within this environment
     * via the template parameter.
     *
     * TODO make the ImposedPrimitives also tags
     *
     * \tparam EnvironmentTags Tags for indicating which environments
     * need to be assembled in the current \elastica simulation.
     * \tparam ImposedPrimitives Tags for indicating which primitive shapes,
     * whose dynamics are not computed with time, need to be assembled in the
     * current \elastica simulation.
     * \tparam ExecutionPolicy Tag policy for execution of environment
     * algorithms one of Execution Policies in @ref execution
     *
     * \see make_environments_configuration() for idiomatic construction
     */
    template <typename EnvironmentTags, typename ImposedPrimitives,
              typename ExecutionPolicy, typename... IndividualConfigurations>
    class EnvironmentsConfiguration
        : public ::tt::ConformsTo<protocols::EnvironmentsConfiguration>,
          public IndividualConfigurations... {
     public:
      //**Type definitions******************************************************
      // TODO : Ensure ordering with collision last
      //! Tags of admissible environment primitives in the default simulation
      using AdmissibleEnvironmentTags = EnvironmentTags;
      //! Type of admissible imposed primitives
      using AdmissibleImposedPrimitives = tmpl::remove_duplicates<
          // no dupes
          tmpl::transform<
              // get value types
              ImposedPrimitives, std::remove_cvref<tmpl::_1>
              // get value types
              >
          // no dupes
          >;
      //! Type of execution policy
      using EnvironmentsExecutionPolicy = ExecutionPolicy;
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor*/
      //@{

      //************************************************************************
      /*!\brief The composite constructor.
       */
      constexpr explicit EnvironmentsConfiguration(
          IndividualConfigurations... configs) noexcept
          : IndividualConfigurations(std::move(configs))... {}
      //************************************************************************

     private:
      //************************************************************************
      static_assert(sizeof...(IndividualConfigurations) <=
                        tmpl::size<EnvironmentTags>::value,
                    "Number of configurations passed in must be less than or "
                    "equal to the number of Tags");
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a environment configuration
     * \ingroup config
     *
     * \example
     * \snippet ModuleConfiguration/Test_Environments.cpp seq_eg
     *
     * \param p execution policy for the configuration
     * \param individual_configurations individual environment configurations
     *
     * For details on Template parameters that need to be provided, see
     * EnvironmentsConfiguration
     */
    template <
        typename EnvironmentTags = ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS,
        typename ListOfImposedPrimitives =
            ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
        typename... IndividualConfigurations>
    inline constexpr decltype(auto) make_environments_configuration(
        decltype(seq) /* meta */,
        IndividualConfigurations... individual_configurations) noexcept {
      return EnvironmentsConfiguration<EnvironmentTags, ListOfImposedPrimitives,
                                       sequential_policy,
                                       IndividualConfigurations...>(
          std::move(individual_configurations)...);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically makes an environment configuration
     * \ingroup config
     *
     * \example
     * \snippet ModuleConfiguration/Test_Environments.cpp par_eg
     *
     * \param p execution policy for the configuration
     * \param individual_configurations individual environment configurations
     *
     * For details on Template parameters that need to be provided, see
     * EnvironmentsConfiguration
     */
    template <
        typename EnvironmentTags = ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS,
        typename ListOfImposedPrimitives =
            ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
        typename... IndividualConfigurations>
    inline constexpr decltype(auto) make_environments_configuration(
        decltype(par) /* meta */,
        IndividualConfigurations... individual_configurations) noexcept {
      return EnvironmentsConfiguration<EnvironmentTags, ListOfImposedPrimitives,
                                       parallel_policy,
                                       IndividualConfigurations...>{
          std::move(individual_configurations)...};
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
