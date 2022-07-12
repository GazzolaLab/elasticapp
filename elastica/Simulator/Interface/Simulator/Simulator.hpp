//==============================================================================
/*!
//  \file
//  \brief Implementation of the \elastica simulator class
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

#define ELASTICA_EXPLICIT_STEPPER 0

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

/// Parallelism detail
#include "Parallel/detail.hpp"

/// All connections types
#include "Simulator/Domain/Domain.hpp"
/// Interface methods
#include "Simulator/Interface/Simulator/Configuration.hpp"
#include "Simulator/Interface/Simulator/SystemsRegistrationHelpers.hpp"
#include "Simulator/Interface/Systems/Module.hpp"
#if (ELASTICA_EXPLICIT_STEPPER)
#include "Simulator/Interface/TimeSteppers/ExplicitSteppers.hpp"
#endif
#include "Simulator/Interface/Batching/DefaultBatcher.hpp"
#include "Simulator/Interface/TimeSteppers/SymplecticSteppers.hpp"
#include "Simulator/Interface/TypeTraits.hpp"
#include "Simulator/Interface/Types.hpp"
/// Serialization support
#include "Simulator/Configuration/TypeTraits/Aliases.hpp"
#include "Simulator/IO/Serialization/Types.hpp"
//
#include "Simulator/Utilities/MemoryMeters.hpp"
#include "Simulator/Utilities/Timers.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
/// Utilities
#include "Utilities/Invoke.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/Logging/Logger.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
#include "Utilities/TypeTraits.hpp"
#include "Utilities/ValueTraits/IsInf.hpp"

// DONE : Check for mixins to simulator class should always be default
//  constructible (not trivial, but default)
// DONE : Make it Singleton/Non-copyable?

namespace elastica {

  namespace interface {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The central Simulator class
     * \ingroup interface
     *
     * The Simulator template class is the backbone of any \elastica simulation
     * and is responsible for managing memory, coordinating different simulation
     * modules and providing a consistent interface for temporal evolution of
     * entities attached to the simulator.
     *
     * \section sim_examples Examples
     *
     * The Simulator class cannot be instantiated directly. To use the Simulator
     * class, its essential to create it with the elastica::make_simulator()
     * like so
     *
     * \example
     * \code
     *   // To make a new simulator with the default configuration
     *   auto my_simulator = elastica::make_simulator<>();
     *   // this returns a shared_ptr instance of a simulator. So you can
     *   // use the simulator as my_simulator->...
     *   // in the rest of the program...
     * \endcode
     *
     * The example above declares a plain Simulator class (without any modules
     * such as elastica::modules::Connections) with a default configuration
     * (elastica::configuration::DefaultSimulatorConfiguration). For creating a
     * Simulator with additional modules, one can use
     * \example
     * \code
     *   auto simulator_with_connections =
     *   elastica::make_simulator<elastica::modules::Connections>();
     * \endcode
     * For more such examples please consult the documentation of
     * elastica::make_simulator().
     *
     * \section sim_structure Structure
     *
     * The Simulator is templated on two types---the first template parameter is
     * a valid instantiation of elastica::configuration::SimulatorConfiguration
     * ---this is the type of argument constructed using
     * elastica::configuration::make_simulator_configuration()
     * and passed as a parameter to elastica::make_simulator(). The
     * second type is a variadic parameter pack of additional Modules that can
     * enhance the interface and functionality of the Simulator. Users are
     * encouraged to peruse the documentation of some of the @ref modules
     * (elastica::modules::Environments, elastica::modules::Connections,
     * elastica::modules::Constraints etc.)
     *
     * \tparam SimTraits A valid Simulator Configuration
     * \tparam Modules   The additional simulation enhancement modules
     *
     * \section sim_usage Usage
     *
     * The use of Simulator is intuitive and follows the public interface of a
     * std::vector. To add a new `system` (@ref systems) to the Simulator, one
     * can use the templated #emplace_back method. The template
     * parameter refers to the type of the `system` to be added, while the
     * arguments passed in are perfectly forwarded to the `system` constructor.
     * Notably, Simulator manages all its memory, and thus returns a view of
     * the added `system`. This view behaves exactly like the added
     * `system` and can be introspected, examined or even changed at a later
     * point in time (before calling #finalize, see below). To
     * abstract away the type of the view, users are encouraged to use
     * automatic type deduction whenever possible. For example, the code below
     * adds a class called `Sphere` to the created Simulator.
     *
     * \example
     * \code
     *   // To make a new simulator with the default configuration
     *   auto my_simulator = make_simulator<>();
     *
     *   auto my_first_sphere =
     *   my_simulator->emplace_back<Sphere>(//...Sphere arguments...//);
     *
     *   // my_simulator now has a sphere in it
     *
     *   // my_first_sphere has the same public interface as a Sphere class
     *   auto my_first_sphere_pos = my_first_sphere.positions()
     *
     *   // ...the rest of the program...
     *
     *   // after all is done, call the finalize() method
     *   my_simulator->finalize();
     * \endcode
     *
     * Before passing the Simulator off to the \elastica integrate() function,
     * it is essential to call its #finalize member. This communicates to
     * \elastica that all changes to the added \systems are done and the
     * simulation can be freezed, and exported for temporal integration. The
     * example above shows how the method can be called. In case you do not call
     * it, we have a couple of checks inside our time modules to preventing
     * this behavior. However, this feature may be changed at a future point in
     * time. Moreover, calling #finalize makes the semantics of your code
     * clear to anyone who reads it---the simulation is finalized and cannot be
     * changed at a future point in time.
     *
     * \section sim_module Design for usage by Modules
     * Health warning : This section contains implementation details and is
     * meant for use by designers of libraries. Some knowledge of `C++` and
     * design principles are required.
     *
     * For the @ref modules (such as elastica::modules::Constraints etc.) to be
     * embedded and use the information available in the Simulator, we designed
     * them as Curious mixins (i.e. as policy template parameters with
     * the Curiously Recurring Template Pattern (CRTP)), with Simulator as the
     * final implementation class (that just holds all the modules together).
     * The modules are templated bases, templated on the current
     * Simulator---hence casting to and from the Simulator
     * is type-safe by design. The requested modules, from
     * elastica::make_simulator(), are additionally friended to the
     * current Simulator and hence each module can transparently "see" and
     * access members from the other modules. In this context, the Simulator
     * is the second template parameter of the requested @ref modules, and users
     * are encouraged to peruse the documentation of some of the @ref modules
     * to get a feel for their structure and usage.
     *
     * Another crucial piece of design, that provides a convenient interface to
     * the simulator's algorithms (such as applying connections, constraints
     * etc), is achieved by making the modules mixed-in classes to this
     * Simulator, which automatically exposes only their public interface from
     * Simulator to the outside world. As an additional step, we make use of
     * infrastructure found in @ref blocks, to declare algorithms as variables
     * for purely generic programming---indeed the Simulator's interface,
     * especially for temporal integration, does NOT require the presence of
     * any public method from the Modules class as a point of algorithm
     * application (such as apply_constraints() from the
     * elastica::modules::Constraints say)---these are abstracted away as meta-
     * variables and exposed via the simulator using a common interface
     * #apply_post_dynamic_updates, #apply_post_kinematic_updates,
     * #apply_pre_dynamic_updates, #apply_updates, independent of the modules
     * requested. This reduces coupling between the simulator and the outside
     * world, which now need to only use these methods.
     *
     * The final piece of design helps integrate the simulator in time. By
     * curiously mixing in the Simulator with
     * elastica::interface::InterfaceWithSymplecticSteppers, and
     * elastica::interface::InterfaceWithExplicitSteppers, we automatically
     * embed a higher-level interface to evolving the entire simulation using
     * a simple #step method, abstracting away all information of systems,
     * connections, constraints etc. This makes it easier to manage state in
     * the simulator and interface with modules on-top of the #step method,
     * such as optimization or RL methods.
     *
     * \see make_simulator(),
     * elastica::configuration::make_simulator_configuration()
     */
    template <typename SimTraits,  // Configuration parameter
              template <typename /*SimTraits*/, typename /*Simulator*/>
              class... Modules>  // Additional modules
    class Simulator
        : private detail::HasDomainConfiguration<SimTraits>,
          public Singleton<Simulator<SimTraits, Modules...>, logging::Logger>,
          /*public DefaultInterface<SimTraits>,*/
          public ::elastica::modules::Systems<SimTraits,
                                              Simulator<SimTraits, Modules...>>,
          public Modules<SimTraits, Simulator<SimTraits, Modules...>>...,
          public InterfaceWithSymplecticSteppers<
              SimTraits, Simulator<SimTraits, Modules...>>
#if (ELASTICA_EXPLICIT_STEPPER)
        ,
          public InterfaceWithExplicitSteppers<SimTraits,
                                               Simulator<SimTraits, Modules...>>
#endif
    {
     private:
      //**Compile-time checks***************************************************
      // These need the constructor of Modules<SimTraits, int> be accessible
      // static_assert(
      //     cpp17::conjunction_v<
      //         std::is_default_constructible<Modules<SimTraits, int>>...>,
      //     "Simulation modules must have a default/empty constructor!");
      //************************************************************************

      //**Type definitions******************************************************

      //************************************************************************
      /*!\name Top-level Type definitions */
      //@{
      //! Type of the Simulator
      using This = Simulator<SimTraits, Modules...>;
      //! Singleton parent
      using ParentSingleton = Singleton<This, logging::Logger>;
      //! System module
      using SystemsModule = ::elastica::modules::Systems<SimTraits, This>;
      //! Typelist of all requested modules
      using ModulesList =
          tmpl::list<SystemsModule, Modules<SimTraits, This>...>;
      //! Symplectic Interface
      using SymplecticInterface =
          InterfaceWithSymplecticSteppers<SimTraits, This>;
      //! Explicit Interface
#if (ELASTICA_EXPLICIT_STEPPER)
      using ExplicitInterface = InterfaceWithExplicitSteppers<SimTraits, This>;
#endif
      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\name Algorithm Type definitions */
      //@{
      //! Algorithms to be carried out after the kinematic update step
      using PostKinematicUpdateSteps = tmpl::append<
          typename Modules<SimTraits, This>::PostKinematicUpdateSteps...>;
      //! Get pre-dynamic algorithm steps (there is no need to re-order)
      using PreDynamicUpdateSteps = tmpl::append<
          typename Modules<SimTraits, This>::PreDynamicUpdateSteps...>;
      //! Get post-dynamic algorithm steps (there is no need to re-order)
      using PostDynamicUpdateSteps = tmpl::append<
          typename Modules<SimTraits, This>::PostDynamicUpdateSteps...>;
      //! Update steps over the systems
      using UpdateSteps =
          tmpl::append<typename SystemsModule::UpdateSteps,
                       typename Modules<SimTraits, This>::UpdateSteps...>;
      //! Get all algorithm steps
      using AlgorithmSteps =
          tmpl::append<UpdateSteps, PostKinematicUpdateSteps,
                       PreDynamicUpdateSteps, PostDynamicUpdateSteps>;
      //! Algorithm cache type
      using Algorithms = tmpl::as_tagged_tuple<AlgorithmSteps>;
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Batch Algorithm Type definitions */
      //@{
      //! Default
      using DefaultBatcher = ::elastica::interface::detail::DefaultBatcher;
      //! Get all batchers algorithm steps
      using Batchers =
          tmpl::append<tmpl::list<DefaultBatcher>,
                       typename Modules<SimTraits, This>::BatchingAlgorithm...>;
      //! Batching algorithm cache type
      using BatchingAlgorithms = tmpl::as_tagged_tuple<Batchers>;
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Profiler type definitions */
      //@{
      //! Cache for timers for profiling the code
      using Timers = ::elastica::Timers<AlgorithmSteps>;
      //! Cache of intrusive memory meters
      using MemoryMeters = ::elastica::MemoryMeters<AlgorithmSteps>;
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Submodules type definitions */
      //@{
      using SubModules =
          tmpl::append<tmpl::list<SystemsModule>,  // so that even if there are
                                                   // no submodules friendship
                                                   // is declared with systems
                       typename Modules<SimTraits, This>::SubModules...>;
      //@}
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Internal size type
      using SizeType = typename SimTraits::SizeType;
      //! Internal type of index
      using IndexType = typename SimTraits::IndexType;
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;
      //! Type of the simulation domain
      using DomainType = ::elastica::tt::domain_type_t<SimTraits>;
      //! Type of direction
      using DirectionType = typename SimTraits::DirectionType;
      //! Alias template for Type of the simulation domain
      template <DirectionType D>
      using BoundaryTypeAlong =
          typename SimTraits::template BoundaryTypeAlong<D>;
      //************************************************************************

     private:
      //**Friendships***********************************************************
      //! Friendship with Singleton
      BEFRIEND_SINGLETON;
      //! Friendship with Symplectic Interface
      friend SymplecticInterface;
#if (ELASTICA_EXPLICIT_STEPPER)
      //! Friendship with Explicit Interface
      friend ExplicitInterface;
#endif
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<This>;

      //   //!\brief Friendship with make_simulator
      //   template <template <typename /*Simulator*/, typename
      //   /*SimTraits*/>
      //             class... AddOns,
      //             typename SimulationOptions>
      //   friend auto make_simulator(SimulationOptions);

      // Make all the bases friends by default
      // Awesome trick to do it, very clever
      // Idea is to construct temporarily a tuple with all base types
      // and then unpack it one by one and make it a friend
      // https://stackoverflow.com/a/31514214
#define REPEAT_2(M, N) M(N) M(N + 1)
#define REPEAT_4(M, N) REPEAT_2(M, N) REPEAT_2(M, N + 2)
#define REPEAT_8(M, N) REPEAT_4(M, N) REPEAT_4(M, N + 4)
#define FRIEND(N) \
  friend tmpl::at_c<ModulesList, std::min(N, sizeof...(Modules))>;

      // Declare friendship with each requested module so that they have access
      // to private get<> functions
      REPEAT_8(FRIEND, 0UL)

#undef FRIEND

#define FRIEND(N) \
  friend tmpl::at_c<SubModules, std::min(N, tmpl::size<SubModules>::value - 1)>;
      // Declare friendship with each requested submodule so that they have
      // access to private get<> functions
      REPEAT_8(FRIEND, 0U)
#undef FRIEND

#undef REPEAT_8
#undef REPEAT_4
#undef REPEAT_2
      //************************************************************************

      //**Domain methods********************************************************
      /*!\name Domain methods*/
      //@{

     protected:
      //**Domain range check****************************************************
      /*!\brief Validity check for a periodic simulator domain
       *
       * Checks if the upper and lower domain bounds are properly set for a
       * periodic domain, if used within the simulator
       */
      template <typename D = DomainType,
                Requires<std::is_same<D, PeriodicDomain>::value> = nullptr>
      void check_domain() const {
        // https://stackoverflow.com/a/12958954
#define CHECK_DOMAIN_PERIODIC_ALONG(DIRECTION)                               \
  if (std::is_same<BoundaryTypeAlong<DomainType::DIRECTION>,                 \
                   IsPeriodic>::value and                                    \
      (is_inf(domain_->template get_lower_bound<DomainType::DIRECTION>()) or \
       is_inf(domain_->template get_upper_bound<DomainType::DIRECTION>())))  \
    throw std::runtime_error(                                                \
        "DomainType is set to be Periodic along " #DIRECTION                 \
        " yet span of the domain along is unset (or) infinite "              \
        "Please call the simulators set_domain method with finite "          \
        "values in the periodic direction "                                  \
        "before the call to integrate() or finalize()!");

        CHECK_DOMAIN_PERIODIC_ALONG(X);
        CHECK_DOMAIN_PERIODIC_ALONG(Y);
        CHECK_DOMAIN_PERIODIC_ALONG(Z);

#undef CHECK_DOMAIN_PERIODIC_ALONG
      }
      //************************************************************************

      //**Domain range check****************************************************
      /*!\brief Validity check for an infinite simulator domain
       *
       * For an unbounded domain, the set methods are no-op, so no checks are
       * needed
       */
      template <typename D = DomainType,  // Domain
                Requires                  // Restricting condition
                <std::is_same<D, UnboundedDomain>::value> = nullptr>
      void check_domain() const noexcept {}
      //************************************************************************

     private:
      //**Set domain************************************************************
      /*!\brief Set parameters of the computational domain
       */
      template <typename... Args>  // Domain arguments
      [[deprecated("Set domain from configuration instead")]] void set_domain(
          Args&&... /*args*/) {
        check_if_not_final();
        // domain_.set(std::forward<Args>(args)...);
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Access to the domain value, useful for serialization
       */
      inline auto domain() const& noexcept -> DomainType const& {
        return *domain_;
      }
      inline auto domain() & noexcept -> DomainType& { return *domain_; }
      //************************************************************************

      //@}
      //************************************************************************

      //**Utility methods*******************************************************
      /*!\name Utility methods*/
      //@{
     protected:
      //**ready method**********************************************************
      /*!\brief Throw if simulation is already finalized
       *
       * \throws logic_error if simulation is already finalized
       */
      auto check_if_not_final() const -> void {
        const char* error_message = R"error(
The simulator has been finalized, yet you are trying to modify its states!
Please ensure that Simulator doesn't change state after finalize() is called by
reorganizing your code logic.
)error";
        if (ready())
          throw std::logic_error(error_message);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Throw if simulation is not yet finalized
       *
       * \throws logic_error if simulation is not already finalized
       */
      auto check_if_final() const -> void {
        const char* error_message = R"error(
The simulator has not yet been finalized, yet you are trying to access constant
parameters! Please ensure that access of Simulator occurs after finalize() is called by
reorganizing your code logic.
)error";
        if (not ready())
          throw std::logic_error(error_message);
      }
      //************************************************************************

     public:
      //**ready method**********************************************************
      /*!\brief Is the simulation ready for integration?
       *
       * \return bool
       */
      inline auto ready() const noexcept -> bool {
        return ready_for_integration_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Is the simulation finalized?
       *
       * \return bool
       */
      inline auto is_final() const noexcept -> bool { return ready(); }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Finalize the modules
       *
       * \param time_v Time of finalizing simulation
       * \exception std::nested_exception
       * \return void
       */
      void finalize_modules(TimeType time_v) {
        try {
          // Travel up the hierarchy and call finalize on all
          // systems module does not need to be finalized
          EXPAND_PACK_LEFT_TO_RIGHT(Modules<SimTraits, This>::finalize(time_v));
        } catch (...) {
          const char* error_message =
              "Preparing the simulator for temporal integration failed "
              "because ";
          std::throw_with_nested(std::runtime_error(error_message));
        };
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Finalize simulation
       *
       * This kicks-off (internally) changes to the Simulator class as it now
       * has all information about the present simulation and can arrange all
       * systems and modules for an efficient simulation. As a side effect, it
       * prevents further changes to the Simulator (views of objects already
       * held can change, however).
       *
       * \param time_v Time of finalizing simulation
       * \exception std::runtime_error, if finalization of modules fail
       * \return void
       */
      void finalize(TimeType time_v = TimeType{0.0}) {
        try {
          finalize_modules(time_v);
        } catch (const std::exception& e) {
          abort_with_backtrace(e);
        }

        //// If domain is not set when the DomainType is periodic, cry for
        //// attention
        // check_domain();

        // Set to be ready for integration
        ready_for_integration_ = true;
      }
      //************************************************************************
      //@}
      //************************************************************************

     public:
      //**Interface methods*****************************************************
      /*!\name System interface methods*/
      //@{

      //************************************************************************
      /*!\brief Interface for emplacing entities into the simulator
       *
       * \details
       * Forwards to the most relevant emplace_back() found by searching all the
       * modules
       *
       * \see ::elastica::modules::Systems::emplace_back(),
       * ::elastica::interface::environments::WithImposedPrimitives::emplace_back()
       */
      template <class AnyRegisteredSystem, class... Args>
      inline decltype(auto) emplace_back(Args&&... args) {
        using ModuleWithAppropriateOverload = typename ChooseMethodOverload<
            detail::tt::has_emplace_back_t,
            ModulesList>::template For<AnyRegisteredSystem, Args&&...>::type;
        return static_cast<ModuleWithAppropriateOverload*>(this)
            ->template emplace_back<AnyRegisteredSystem>(
                std::forward<Args>(args)...);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get an invalid reference type for given a system type
       *
       * \details
       * Forwards to the most relevant get_invalid_ref() found by searching all
       * the modules
       *
       * \see ::elastica::modules::Systems::get_invalid_ref(),
       * ::elastica::interface::environments::WithImposedPrimitives::get_invalid_ref()
       */
      template <class AnyRegisteredSystem>
      inline decltype(auto) get_invalid_ref() const noexcept {
        using ModuleWithAppropriateOverload = typename ChooseMethodOverload<
            detail::tt::has_get_invalid_ref,
            ModulesList>::template For<AnyRegisteredSystem>::type;
        return static_cast<ModuleWithAppropriateOverload const*>(this)
            ->template get_invalid_ref<AnyRegisteredSystem>();
      }
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Integration methods***************************************************
      /*!\name Temporal integration methods*/
      //@{

      //************************************************************************
      /*!\brief Access to the algorithms
       */
      inline constexpr auto algorithms() const noexcept -> Algorithms const& {
        return *algorithms_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get an algorithm from the simulator cache
       *
       * \tparam AlgorithmTag Tag for obtaining the algorithm
       * \return Algorithm callable
       */
      template <typename AlgorithmTag>
      inline constexpr decltype(auto) get_algorithm() const noexcept {
        return tuples::get<AlgorithmTag>(algorithms());
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the batching algorithms
       */
      inline constexpr auto batching_algorithms() const noexcept
          -> BatchingAlgorithms const& {
        return *batching_algorithms_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get a batching algorithm from the simulator cache
       *
       * \tparam BatchingAlgorithmTag Tag for obtaining the algorithm
       * \return Algorithm callable
       */
      template <typename BatchingAlgorithmTag>
      inline constexpr decltype(auto) get_batching_algorithm() const noexcept {
        return tuples::get<BatchingAlgorithmTag>(batching_algorithms());
      }
      //************************************************************************
     protected:
      //************************************************************************
      template <typename Step>
      void prologue() /*const*/ noexcept {
        using namespace ::elastica::logging;

        log<debug>([](auto const& log) {
          log << "Performing " + ::pretty_type::name<Step>() + " ...\n";
        });

        ELASTICA_PROFILING_SECTION {
          get_timer<Step>().start();
          get_memory_meter<Step>().start();
        }
      };
      //************************************************************************

      //************************************************************************
      template <typename Step>
      void epilogue() /*const*/ noexcept {
        using namespace ::elastica::logging;

        ELASTICA_PROFILING_SECTION {
          get_timer<Step>().stop();
          get_timer<Step>().stop();
        }

        log<debug>([](auto const& log) {
          log << "Finished " + ::pretty_type::name<Step>() + " ...\n";
        });
      };
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Perform an algorithmic step without additional book-keeping for
       * time taken/memory used
       *
       * \note
       * We need strong exception safety guarantee from all algorithms here.
       */
      template <typename Step, typename... Args>
      void perform_without_instrumentation(
          Args&&... args) /* const */ noexcept {
        tuples::get<Step>(algorithms())(std::forward<Args>(args)...);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Perform an algorithmic step while recording time take/memory
       * used
       *
       * \note
       * We need strong exception safety guarantee from all algorithms here.
       */
      template <typename Step, typename... Args>
      void perform(Args&&... args) /* const */ noexcept {
        prologue<Step>();
        perform_without_instrumentation<Step>(std::forward<Args>(args)...);
        epilogue<Step>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply algorithmic updates across modules occurring after the
       * kinematic update step (of a symplectic stepper).
       *
       * \note
       * We need strong exception safety guarantee from all algorithms here.
       *
       * \param time_v Time of simulation
       * \return void
       */
      inline auto apply_post_kinematic_updates(
          TimeType time_v) /*const*/ noexcept -> void {
        tmpl::for_each<PostKinematicUpdateSteps>([this, time_v](auto update) {
          this->template perform<tmpl::type_from<decltype(update)>>(time_v);
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply algorithmic updates across modules occurring before the
       * dynamic update step (of a symplectic stepper).
       *
       * \note
       * We need strong exception safety guarantee from all algorithms here.
       *
       * \param time_v Time of simulation
       * \return void
       */
      inline auto apply_pre_dynamic_updates(TimeType time_v) /*const*/ noexcept
          -> void {
        tmpl::for_each<PreDynamicUpdateSteps>([this, time_v](auto update) {
          this->template perform<tmpl::type_from<decltype(update)>>(time_v);
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply algorithmic updates across modules occurring after the
       * dynamic update step (of a symplectic stepper).
       *
       * \note
       * We need strong exception safety guarantee from all algorithms here.
       *
       * \param time_v Time of simulation
       * \return void
       */
      inline auto apply_post_dynamic_updates(TimeType time_v) /*const*/ noexcept
          -> void {
        tmpl::for_each<PostDynamicUpdateSteps>([this, time_v](auto update) {
          perform<tmpl::type_from<decltype(update)>>(time_v);
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply all algorithmic updates across modules
       *
       * \note
       * We need strong exception safety guarantee from all algorithms here.
       *
       * \param time_v Time of simulation
       * \return void
       */
      inline auto apply_updates(TimeType time_v) /*const*/ noexcept -> void {
        apply_post_kinematic_updates(time_v);
        apply_pre_dynamic_updates(time_v);
        apply_post_dynamic_updates(time_v);
      }
      //************************************************************************

      //@}
      //************************************************************************

     public:  // For access to modules such as with primitives
      //**Profiling methods*****************************************************
      /*!\name Profiling methods*/
      //@{

      //************************************************************************
      /*!\name Timer functions */
      //@{
      /*!\brief Gets the registered timers.
       */
      inline constexpr auto timers() & noexcept -> Timers& { return timers_; }
      inline constexpr auto timers() const& noexcept -> Timers const& {
        return timers_;
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Timer functions */
      //@{
      /*!\brief Get a timer corresponding to an algorithm
       *
       * \tparam AlgorithmTag Tag for obtaining the algorithm
       * \return Algorithm callable
       */
      template <typename AlgorithmTag>
      inline constexpr decltype(auto) get_timer() & noexcept {
        using ::elastica::get_meter;
        return get_meter<AlgorithmTag>(timers());
      }
      template <typename AlgorithmTag>
      inline constexpr decltype(auto) get_timer() const& noexcept {
        using ::elastica::get_meter;
        return get_meter<AlgorithmTag>(timers());
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Memory meter functions */
      //@{
      /*!\brief Gets the registered memory meters
       */
      inline constexpr auto memory_meters() & noexcept -> MemoryMeters& {
        return memory_meters_;
      }
      inline constexpr auto memory_meters() const& noexcept
          -> MemoryMeters const& {
        return memory_meters_;
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Memory meter functions */
      //@{
      /*!\brief Get a memory meter corresponding to an algorithm
       *
       * \tparam AlgorithmTag Tag for obtaining the algorithm
       * \return Algorithm callable
       */
      template <typename AlgorithmTag>
      inline constexpr decltype(auto) get_memory_meter() & noexcept {
        using ::elastica::get_meter;
        return get_meter<AlgorithmTag>(memory_meters());
      }
      template <typename AlgorithmTag>
      inline constexpr decltype(auto) get_memory_meter() const& noexcept {
        using ::elastica::get_meter;
        return get_meter<AlgorithmTag>(memory_meters());
      }
      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Logs the profiling information into a stream
       *
       * \param os Output stream for streaming into
       * \return os
       */
      inline void stream_profiling_summary_to(std::ostream& os) const {
        /*
        ELASTICA_PROFILING_SECTION {
          constexpr std::size_t n_timers = tmpl::size<Timers>::value;
          std::array<double, n_timers> minValues, maxValues, totalValues;
          std::array<std::size_t, n_timers> numValues;

          auto populate_timers = [this, &minValues, &maxValues, &totalValues,
                                  &numValues](auto Is) {
            auto const& timer =
                this->get_timer<tmpl::at_c<AlgorithmSteps, Is>>();
            minValues[Is] = timer.min();
            maxValues[Is] = timer.max();
            totalValues[Is] = timer.total();
            numValues[Is] = timer.getCounter();
          };

          // Compile time for-each, don't like brigands' type_ ID
          index_apply<n_timers>([populate_timers](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(populate_timers(Is));
          });

          const std::size_t named_width = 30UL;
          const std::size_t field_width = 10UL;
          const auto delimiter = ",";  // ?
          const std::string header_indicator = "#";
          // os << std::setw(name_width) << TimerTag::log_name() << delimiter;

          std::ostringstream ss;
          ss << std::left  //
             << std::setw(named_width) << header_indicator + " code_part"
             << delimiter << std::right  //
             << std::setw(field_width) << "min_time" << delimiter
             << std::setw(field_width) << "max_time" << delimiter
             << std::setw(field_width) << "avg_time" << delimiter
             << std::setw(field_width) << "tot_time" << delimiter
             << std::setw(field_width) << "n_samples";
          std::string header = ss.str();
          std::string horizontal_line =
              std::string(::elastica::size(header), '-');
          os << horizontal_line << "\n";
          os << header << "\n";

          auto pretty_printer = [this, &os, &minValues, &maxValues,
                                 &totalValues, &numValues,
                                 delimiter = ","](auto Is) {
            using TimerTag = tmpl::at_c<Timers, Is>;
            const double average_time = totalValues[Is] / numValues[Is];
            os << std::left  //
               << std::setw(named_width) << TimerTag::log_name() << delimiter
               << std::right  //
               << std::setw(field_width) << minValues[Is] << delimiter
               << std::setw(field_width) << maxValues[Is] << delimiter
               << std::setw(field_width) << average_time << delimiter
               << std::setw(field_width) << totalValues[Is] << delimiter
               << std::setw(field_width) << numValues[Is] << "\n";
          };
          // Compile time for-each, don't like brigands' type_ ID
          index_apply<n_timers>([pretty_printer](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(pretty_printer(Is));
          });
        }
         */
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Prints the profiling information to standard output
       */
      inline void print_profiling_summary() const {
        (void)stream_profiling_summary_to(std::cout);
      }
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //**metadata methods******************************************************
      /*!\name Metadata methods*/
      //@{

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      struct MetadataGenerator {
        MetadataGenerator(This const& simulator) : sim_(simulator) {}
        ~MetadataGenerator() = default;

        using Result = typename AlgorithmMetadata::Result;
        using Metadata = typename AlgorithmMetadata::Metadata;

        template <typename Update>
        auto generate_metadata() const -> Result {
          return {pretty_type::name<Update>(), Update::metadata(sim_)};
        }

        template <typename Tags>
        auto operator()(Tags /*metadata*/) const -> Metadata {
          Metadata results;
          results.reserve(tmpl::size<Tags>::value);

          tmpl::for_each<Tags>([&](auto tag) {
            using Tag = tmpl::type_from<decltype(tag)>;
            results.push_back(generate_metadata<Tag>());
          });

          return results;
        }

        This const& sim_;
      };
      /*! \endcond */
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Generates metadata of the kinematic update step
       *
       * \return vector of string pairs corresponding to algorithm and its
       * metadata
       */
      auto metadata_update_kinematics() const noexcept ->
          typename MetadataGenerator::Metadata {
        return MetadataGenerator(*this)(
            tmpl::list<typename SystemsModule::UpdateKinematics>{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Generates metadata of the dynamic update step
       *
       * \return vector of string pairs corresponding to algorithm and its
       * metadata
       */
      auto metadata_update_dynamics() const noexcept ->
          typename MetadataGenerator::Metadata {
        return MetadataGenerator(*this)(
            tmpl::list<typename SystemsModule::UpdateDynamics>{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Generates metadata of the dynamic compute step
       *
       * \return vector of string pairs corresponding to algorithm and its
       * metadata
       */
      auto metadata_compute_internal_dynamics() const noexcept ->
          typename MetadataGenerator::Metadata {
        return MetadataGenerator(*this)(
            tmpl::list<typename SystemsModule::ComputeInternalDynamics>{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Generates metadata of the post kinematic update steps
       *
       * \return vector of string pairs corresponding to algorithm and its
       * metadata
       */
      auto metadata_post_kinematic_updates() const noexcept ->
          typename MetadataGenerator::Metadata {
        return MetadataGenerator(*this)(PostKinematicUpdateSteps{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Generates metadata of the pre dynamic update steps
       *
       * \return vector of string pairs corresponding to algorithm and its
       * metadata
       */
      auto metadata_pre_dynamic_updates() const noexcept ->
          typename MetadataGenerator::Metadata {
        return MetadataGenerator(*this)(PreDynamicUpdateSteps{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Generates metadata of the post dynamic update steps
       *
       * \return vector of string pairs corresponding to algorithm and its
       * metadata
       */
      auto metadata_post_dynamic_updates() const noexcept ->
          typename MetadataGenerator::Metadata {
        return MetadataGenerator(*this)(PostDynamicUpdateSteps{});
      }
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief The destructor.
       */
      ~Simulator() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the Simulator instance";
        });
      };
      //************************************************************************

      //@}
      //************************************************************************

     private:
      // Prevent access to instance from the outside world
      using ParentSingleton::instance;

      //************************************************************************
      /*!\brief Initialize (delayed) members using traits
       *
       * \param traits Traits to initialize the simulator from
       * \return void
       */
      void initialize_from_traits(SimTraits traits) {
        constexpr std::size_t n_algorithms = tmpl::size<AlgorithmSteps>::value;
        constexpr std::size_t n_batching_algorithms =
            tmpl::size<Batchers>::value;
        // simulator
        domain_ = std::make_unique<DomainType>(std::move(traits).domain());
        algorithms_ = std::make_unique<Algorithms>(
            // tagged tuple does not have a tuple constructor but has a
            // variadic constructor
            cpp17::apply(
                [](auto&&... initializer_as_a_tuple) noexcept {
                  return Algorithms(
                      std::forward<decltype(initializer_as_a_tuple)>(
                          initializer_as_a_tuple)...);
                },
                repeat_as_tuple<n_algorithms>(
                    // can use forward as tuple here in case
                    std::tuple<This*, SimTraits const&>(this, traits))));
        batching_algorithms_ = std::make_unique<BatchingAlgorithms>(
            // tagged tuple does not have a tuple constructor but has a
            // variadic constructor
            cpp17::apply(
                [](auto&&... initializer_trait) noexcept {
                  return BatchingAlgorithms(
                      std::forward<decltype(initializer_trait)>(
                          initializer_trait)...);
                },
                repeat_as_tuple<n_batching_algorithms>(
                    // can use forward as tuple here in case
                    traits)));
        // systems modules
        // the () functions return the dereferenced result which cannot be
        // assigned
        SystemsModule::blocking_policy_id() =
            std::make_unique<typename SimTraits::BlockingPolicy>(
                std::move(traits).blocking_policy());
        SystemsModule::iteration_policy_id() =
            std::make_unique<typename SimTraits::IterationPolicy>(
                std::move(traits).iteration_policy());
      }
      //************************************************************************

     public:
      //**Initialize function***************************************************
      /*!\brief Access to the underlying simulator
       *
       * \return Handle to the sole singleton of type T
       */
      static auto initialize(SimTraits traits) {
        auto simulator = This::instance();
        simulator->initialize_from_traits(std::move(traits));
        // any initialization of the parallelism engine for example goes here
        ::elastica::parallel::detail::initialize_smp(traits.n_threads());
        return simulator;
      }
      //************************************************************************

     private:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief The default constructor.
       */
      Simulator()
          : ParentSingleton(),
            SystemsModule(),
            Modules<SimTraits, This>()...,
            SymplecticInterface(),
#if ELASTICA_EXPLICIT_STEPPER
            ExplicitInterface(),
#endif
            algorithms_(nullptr),
            batching_algorithms_(nullptr),
            domain_(nullptr),
            timers_(),
            memory_meters_(),
            ready_for_integration_(false) {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the Simulation instance";
        });
      }
      //************************************************************************
      //@}
      //************************************************************************

     protected:
      //**Reset methods*********************************************************
      /*!\name Reset methods*/
      //@{

      //************************************************************************
      /*!\brief Resets the individual modules
       *
       * \param time_v The time of reset, will be propagated to the modules
       * \exception std::runtime_error If the reset of any module throws an
       * error, it is propagated here, rather than a hard exit.
       */
      void reset_modules(TimeType time_v) {
        try {
          // Travel up the hierarchy and call reset on all
          // modules, including the Systems module
          SystemsModule::reset_module(time_v);
          EXPAND_PACK_LEFT_TO_RIGHT(
              Modules<SimTraits, This>::reset_module(time_v));
          ready_for_integration_ = false;

          // tuples::apply() is const
          using ::elastica::for_each;
          for_each(timers(), [](auto& meter) { meter.reset(); });
          for_each(memory_meters(), [](auto& meter) { meter.reset(); });

        } catch (...) {
          const char* error_message =
              "Resetting the simulator for temporal integration failed "
              "because ";
          std::throw_with_nested(std::runtime_error(error_message));
        };
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      // NOTE : This is the biggest letdown of a singleton pattern. What can
      // be value initialized here needs to be converted to a pointer, since
      // the initialization of algorithms require a stateful constructor, and
      // algorithms falls short on this.
      // We cannot do a tagged tuple of algorithm pointers too, because then
      // obtaining the types using the tags becomes painful.

      //! Handle to algorithms
      std::unique_ptr<Algorithms const> algorithms_;
      //! Handle to batching algorithms
      std::unique_ptr<BatchingAlgorithms const> batching_algorithms_;
      //! Handle to a domain object
      std::unique_ptr<DomainType const> domain_;
      //! Timer caches
      Timers timers_;
      //! Memory meter caches
      MemoryMeters memory_meters_;
      //! State of simulation finalization
      bool ready_for_integration_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica
