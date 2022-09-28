//==============================================================================
/*!
//  \file
//  \brief Implementation of configuration for an \elastica simulator
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

#include <memory>
#include <utility>  // move
#include <vector>

///
#include "Simulator/Configuration/TypeTraits.hpp"
#include "Simulator/Configuration/Types.hpp"
///
#include "Systems/Block/Block/Block.hpp"
///
#include "Time/SimulationTime.hpp"
///
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace configuration {

    namespace detail {

      template <typename SysID>
      struct AddSystemStorageType {
        using type = std::vector<SysID>;
      };

      template <typename Plugin>
      struct AddBlock {
        using type = blocks::Block<Plugin>;
      };

      //      template <typename SystemStorage, typename = std::nullptr_t>
      //      struct RemoveSystemStorageType;

      // all this hooha for abstracting away the actual type of SystemStorage
      template <typename SystemStorage,
                Requires<cpp17::is_same_v<
                    typename AddSystemStorageType<
                        typename SystemStorage::value_type>::type,
                    SystemStorage>> = nullptr>
      struct RemoveSystemStorageType {
        using type = typename SystemStorage::value_type;
      };

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration for all simulations
     * \ingroup config
     *
     * The SimulatorConfiguration template class serves as the main
     * customization point of any \elastica simulation and is responsible for
     * assembling together different parts of the \elastica library using
     * compile-time meta-programming rules. Its intended to be the first
     * template parameter of Simulator.
     *
     * SimulatorConfiguration is templated on the configurations of individual
     * modules (called `ModuleConfigs`), each of which is responsible for
     * customizing its respective module's behavior. The order of these
     * `ModuleConfigs` is irrelevant---as long as the corresponding module
     * configurations are present, SimulatorConfiguration correctly customizes
     * an \elastica simulation.
     *
     * \tparam ModuleConfigs The configuration of the modules
     *
     * At the very least, for a functional simulation involving a valid
     * SimulatorConfiguration, one needs to provide a valid DomainConfiguration
     * (conforming to DomainProtocol) and a valid SystemsConfiguration
     * (conforming to SystemsProtocol)---please consult the appropriate protocol
     * documentation for details. Other configurations are optional, unless and
     * until support for a module is explicitly requested : see the example
     * sections below for more details.
     *
     * \section sim_config_examples Examples
     * To construct a SimulatorConfiguration, please look at the documentation
     * of elastica::configuration::make_simulator_configuration(), for
     * idiomatic construction rules.
     *
     * Here, we focus on how to use a SimulatorConfiguration after construction.
     * The SimulatorConfiguration class finds its main usage in
     * elastica::make_simulator(), like so
     *
     * \example
     * \code
     *   // your custom configuration
     *   struct MyEnvironmentsConfiguration {
     *   //....implementation details...//
     *   };
     *
     *   struct MyConnectionConfiguration {
     *   //....implementation details...//
     *   };
     *
     *   using MySimulatorConfiguration =
     *   SimulatorConfiguration<
     *                       // default configurations
     *                       DefaultDomainConfiguration,
     *                       DefaultSystemsConfiguration,
     *                       // custom environment configuration for your
     *                       // modules
     *                       MyEnvironmentsConfiguration,
     *                       MyConnectionConfiguration
     *                       >;
     *
     *   // To make a new simulator with this configuration
     *   auto my_simulator =
     *   make_simulator<Environments>(MySimulatorConfiguration{});
     * \endcode
     *
     * We direct your attention to two things:
     * - SimulatorConfiguration is intended to be passed on as the argument to
     *   elastica::make_simulator(). Consult its documentation for more details.
     * - Any additional/extraneous configurations passed to the
     *   SimulatorConfiguration will be silently ignored. For instance, in the
     *   case above, `MyConnectionConfiguration` is embedded into
     *   `MySimulatorConfiguration` but it is not used by
     *   elastica::make_simulator().This is because as we explicitly requested
     *   it to only have support for the Environments module in its template
     *   argument list.
     *
     * However, if a Configuration for a particular requested module is missing
     * a compiler error is thrown, as we cannot assume any information. That is,
     *
     * \example
     * \code
     *   using MySimulatorConfiguration =
     *   SimulatorConfiguration<
     *                       DefaultDomainConfiguration,
     *                       DefaultSystemsConfiguration,
     *                       >;
     *
     *   // Results in a compile-time error since MySimulatorConfiguration does
     *   // not have support embedded for Environment configuration, although
     *   // the Environments module is requested below.
     *   auto my_incorrect_simulator =
     *   make_simulator<Environments>(MySimulatorConfiguration{});
     * \endcode
     *
     * \section sim_config_specializations Creating custom specializations
     *
     * If you want to additionally customize the SimulatorConfiguration class
     * to introduce additional metaprogramming rules based on a Configuration
     * for a new module, it is possible to extend the SimulatorConfiguration
     * by deriving from it. Any additions go into this derived class. As an
     * example, let us consider a new Configuration for, say, adding a top
     * level Controls configuration enforced by some protocol. The following
     * code gives a prototypical way of extending the configuration:
     *
     * \example
     * \code
     *   // Custom configuration for some special add-on
     *   struct ControlConfiguration
     *   : public ::tt::ConformsTo<protocols::ControlConfiguration>{
     *     // Need to customize based on nested `type` member
     *     using ControlAlgorithmType = PIDController;
     *   };
     *
     *   // While this one compiles, it doesn't have custom rules built-in
     *   // on how to "use" the ControlConfiguration and so does not work. Oops!
     *   using NotWorkingControlledConfiguration =
     *     SimulatorConfiguration<
     *                           DefaultDomainConfiguration,
     *                           DefaultSystemsConfiguration,
     *                           ControlConfiguration>;
     *
     *   // We side-step the issue by deriving from SimulatorConfiguration
     *   // and embedding our controls logic
     *   template <class... Configs>
     *   struct ControlledSimulatorConfiguration :
     *   public SimulatorConfiguration<Configs...>{
     *       using ControllerType = typename ControlAlgorithmType::type;
     *   };
     *
     *   // Now we can use the ControllerType in our Control module!
     *   using MyWorkingControlledConfiguration =
     *     ControlledSimulatorConfiguration<
     *                                       DefaultDomainConfiguration,
     *                                       DefaultSystemsConfiguration,
     *                                       ControlConfiguration
     *                                      >;
     *
     *   // Make a new simulator with this configuration as usual
     *   auto controlled_sim =
     *   make_simulator<Controls>(MyWorkingControlledConfiguration{});
     * \endcode
     *
     * \section sim_config_usage Use of Configuration by Modules
     *
     * To use this configuration class, the Simulator and all independent
     * module classes (that add-on to the Simulator) are templated on a
     * Configuration parameter. The SimulatorConfiguration is intended to stamp
     * out Simulators and add-ons using this template parameter so that all
     * configuration information is available at the time of definition. This
     * also means that any module has access to the configuration parameters of
     * all other modules, which helps in compile-time composition of
     * algorithms/methods/data-structures for efficient runtime simulations.
     *
     * As such, other parts of \elastica (such as the collision (or)
     * flow module) may also have its own configuration independent of the
     * current SimulatorConfiguration---but they are also templated on
     * SimulatorConfiguration as a parameter for customizing its
     * algorithms/methods/data-structures to play well with the rest of the
     * simulation.
     *
     * \see make_simulator(), Simulator
     * \see make_simulator_configuration() for idiomatic construction
     */
    template <class... ModuleConfigs>
    class SimulatorConfiguration : public ModuleConfigs... {
     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Deleted default constructor
       */
      SimulatorConfiguration() = delete;
      //************************************************************************

      //************************************************************************
      /*!\brief Constructs the Simulator configuration
       *
       * \details
       * As the module configurations may have arbitrary state, here we
       * construct them piece-by-piece and pass it as a whole into the
       * SimulatorConfiguration
       */
      explicit SimulatorConfiguration(ModuleConfigs... module_configs) noexcept
          : ModuleConfigs(std::move(module_configs))... {}
      //************************************************************************

      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~SimulatorConfiguration() = default;
      //@}
      //************************************************************************

     public:
      //////////////////////////////////////////////////////////////////////////
      // Miscellaneous Configuration (data structure and interface)
      //////////////////////////////////////////////////////////////////////////
      /*!\name Miscellaneous Configuration*/
      //@{

      // TODO : Determine based on the parallel strategy what
      //        the storage is going to be?

      //**Type definitions******************************************************
      //! Alias template for adding block to a plugin type
      template <typename T>
      using AddBlockType = typename detail::AddBlock<T>::type;
      //! Alias template for adding storage to a type
      template <typename T>
      using AddSystemStorageType =
          typename detail::AddSystemStorageType<T>::type;
      //! Alias template for removing storage from a type
      template <typename T>
      using RemoveSystemStorageType =
          typename detail::RemoveSystemStorageType<T>::type;
      //! Alias template for adding storage to a primitive type
      template <typename PrimitiveType>
      using AddPrimitiveStorageType = AddSystemStorageType<PrimitiveType>;
      //! Alias template for removing storage from a primitive type
      template <typename PrimitiveType>
      using RemovePrimitiveStorageType = RemoveSystemStorageType<PrimitiveType>;
      //! Alias template for adding storage to an imposed primitive type
      template <typename ImposedPrimitiveType>
      using AddImposedPrimitiveStorageType =
          AddSystemStorageType<ImposedPrimitiveType>;
      //! Alias template for removing storage from a imposed primitive type
      template <typename ImposedPrimitiveType>
      using RemoveImposedPrimitiveStorageType =
          RemoveSystemStorageType<ImposedPrimitiveType>;
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! Typelist of configurations
      using Configurations = tmpl::list<ModuleConfigs...>;
      //************************************************************************

     public:
      //////////////////////////////////////////////////////////////////////////
      // Time Configuration
      //////////////////////////////////////////////////////////////////////////
      /*!\name Time Configuration*/
      //@{

      //**Type definitions******************************************************
      //!< Type of simulation time
      using TimeType = ::elastica::Time;
      //!< Type of simulation time increments
      using TimeDeltaType = ::elastica::TimeDelta;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //////////////////////////////////////////////////////////////////////////
      // Index Type
      //////////////////////////////////////////////////////////////////////////
      /*!\name Index Configuration*/
      //@{

      //**Type definitions******************************************************
      //!< Type of index
      using IndexType = std::size_t;
      //!< Type of size
      using SizeType = std::size_t;
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //////////////////////////////////////////////////////////////////////////
      // Systems Configuration
      //////////////////////////////////////////////////////////////////////////
      /*!\name Systems Configuration*/
      //@{
      // This is needed here to configure the Primitive types, which is a common
      // link between the Systems and Environments module.
      /*! \cond ELASTICA_INTERNAL */
      //**Type definitions******************************************************
      // 1. Simulation always needs some systems to integrate

      //! Type of systems configuration find result
      using systems_configuration_find_result =
          tmpl::find<Configurations, tt::IsSystemsConfiguration<tmpl::_1>>;
      //! Type of systems configuration find result
      using systems_configuration_found =
          tmpl::bool_<tmpl::size<systems_configuration_find_result>::value !=
                      0>;
      static_assert(systems_configuration_found::value,
                    R"error(
A conforming Systems configuration has not been found in the list of
configurations passed in! Aborting compilation... Please check if you have
provided a Systems configuration and ensure it derives publicly from
::elastica::configuration::protocols::SystemsConfiguration
)error");
      //! Type of Systems configuration
      using SystemsConfiguration =
          tmpl::front<systems_configuration_find_result>;

      // 2. Check with feedback if its a valid simulator configuration
      using SystemsConfigurationValidityChecker IGNORE_UNUSED =
          decltype(tt::IsValidSystemsConfiguration<SystemsConfiguration>{});
      /*! \endcond */
      //************************************************************************

     public:
      //**Type definitions******************************************************
      // 3. Now we can retrieve collections and primitives, alias lookup
      // almost free!

      //! Typelist of admissible systems
      using ListOfAdmissibleSystemCollections = tmpl::transform<
          typename SystemsConfiguration::AdmissibleSystemPlugins,
          tmpl::bind<AddBlockType, tmpl::_1>>;

      //@}
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a simulator configuration
     * \ingroup config
     *
     * \details
     * Makes a simulator configuration to configure
     * ::elastica::interface::Simulator. The value returned is the sole
     * parameter in elastica::make_simulator(). You need not use this function
     * in case you don't need customization, in which case
     * elastica::configuration::DefaultSimulatorConfiguration might be useful.
     *
     * \example
     * If you choose to customize the simulation, the arguments always must have
     * at least have a valid SystemsConfiguration and DomainConfiguration, as
     * seen below
     * \snippet Test_SimulatorConfiguration.cpp basic_eg
     * Note that the order of arguments do not matter---you can pass in
     * DomainConfiguration first followed by a SystemsConfiguration. We show
     * this in the following code which is made more readable by using the
     * ::elastica namespace
     * \snippet Test_SimulatorConfiguration.cpp basic_shorthand_eg
     *
     * In case you want to build up a simulator with other modules then you
     * should pass in as arguments the Configuration parameter of the respective
     * module. For example, in case you want @ref connections in your simulator
     * here is how you add a ConnectionsConfiguration
     * \snippet Test_SimulatorConfiguration.cpp with_connections_eg
     *
     * Similarly for more modules you can pass in their configuration as
     * parameters:
     * \snippet Test_SimulatorConfiguration.cpp all_eg
     *
     * A useful pattern for avoiding code-bloat in your client file, in case you
     * do not need customization for some modules, is to use the default
     * configurations found in @ref default_config. For example, in case you
     * want to have a simulation with Connections, Constraints and Forcing
     * and only want to customize Forcing (maybe because you expect many
     * external forces in your simulator) such that you want to evaluate forcing
     * in parallel, you can choose to use defaults for all other modules:
     * \snippet Test_SimulatorConfiguration.cpp defaults_eg
     *
     * See the documentation on @ref default_config for more detail on the
     * default parameters.
     *
     * \param configs Configurations of individual modules to make the simulator
     * from
     * \return SimulatorConfiguration
     */
    template <typename... Configs>
    inline constexpr decltype(auto) make_simulator_configuration(
        Configs... configs) noexcept {
      return SimulatorConfiguration<Configs...>(std::move(configs)...);
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
