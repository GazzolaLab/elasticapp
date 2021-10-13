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

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

/// All connections types
#include "Simulator/Domain/Domain.hpp"
/// Interface methods
#include "Simulator/Interface/Simulator/Configuration.hpp"
#include "Simulator/Interface/Systems/Module.hpp"
#include "Simulator/Interface/TimeSteppers/ExplicitSteppers.hpp"
#include "Simulator/Interface/TimeSteppers/SymplecticSteppers.hpp"
#include "Simulator/Interface/TypeTraits.hpp"
#include "Simulator/Interface/Types.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
/// Utilities
#include "ErrorHandling/NestedExceptions.hpp"
#include "Simulator/Configuration/TypeTraits/Aliases.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/Logging/Logger.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
#include "Utilities/TypeTraits.hpp"
#include "Utilities/ValueTraits/IsInf.hpp"
#include "Utilities/WidthStream.hpp"

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
              SimTraits, Simulator<SimTraits, Modules...>>,
          public InterfaceWithExplicitSteppers<
              SimTraits, Simulator<SimTraits, Modules...>> {
     private:
      //**Compile-time checks***************************************************
      static_assert(
          cpp17::conjunction_v<
              std::is_default_constructible<Modules<SimTraits, int>>...>,
          "Simulation modules must have a default/empty constructor!");
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
      using ExplicitInterface = InterfaceWithExplicitSteppers<SimTraits, This>;
      //@}
      //************************************************************************

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
      using typename SystemsModule::UpdateSteps;
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
      //! Get all batchers algorithm steps
      using Batchers =
          tmpl::append<typename Modules<SimTraits, This>::BatchingAlgorithm...>;
      //! Batching algorithm cache type
      using BatchingAlgorithms = tmpl::as_tagged_tuple<Batchers>;
      //@}
      //************************************************************************

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
      //! Friendship with Explicit Interface
      friend ExplicitInterface;
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
#undef REPEAT_8
#undef REPEAT_4
#undef REPEAT_2
      //************************************************************************

      //**Domain methods********************************************************
      /*!\name Domain methods*/
      //@{

     private:
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
#define CHECK_DOMAIN_PERIODIC_ALONG(DIRECTION)                            \
  if (std::is_same<BoundaryTypeAlong<DomainType::DIRECTION>,              \
                   IsPeriodic>::value and                                 \
      (is_inf(domain_.template get_lower_bound<DomainType::DIRECTION>) or \
       is_inf(domain_.template get_upper_bound<DomainType::DIRECTION>)))  \
    throw std::runtime_error(                                             \
        "DomainType is set to be Periodic along " #DIRECTION              \
        " yet span of the domain along is unset (or) infinite "           \
        "Please call the simulators set_domain method with finite "       \
        "values in the periodic direction "                               \
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
          Args&&... args) {
        check_if_not_final();
        // domain_.set(std::forward<Args>(args)...);
      }
      //************************************************************************

     protected:
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
     private:
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
      void finalize(TimeType time_v = 0.0) {
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

     protected:
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
      inline auto apply_post_kinematic_updates(TimeType time_v) const noexcept
          -> void {
        tuples::apply<PostKinematicUpdateSteps>(
            [=](auto const&... post_kinematic_updates) {
              EXPAND_PACK_LEFT_TO_RIGHT(post_kinematic_updates(time_v));
            },
            algorithms());
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
      inline auto apply_pre_dynamic_updates(TimeType time_v) const noexcept
          -> void {
        tuples::apply<PreDynamicUpdateSteps>(
            [=](auto const&... pre_dynamic_updates) {
              EXPAND_PACK_LEFT_TO_RIGHT(pre_dynamic_updates(time_v));
            },
            algorithms());
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
      inline auto apply_post_dynamic_updates(TimeType time_v) const noexcept
          -> void {
        tuples::apply<PostDynamicUpdateSteps>(
            [=](auto const&... post_dynamic_updates) {
              EXPAND_PACK_LEFT_TO_RIGHT(post_dynamic_updates(time_v));
            },
            algorithms());
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
      inline auto apply_updates(TimeType time_v) const noexcept -> void {
        apply_post_kinematic_updates(time_v);
        apply_pre_dynamic_updates(time_v);
        apply_post_dynamic_updates(time_v);
      }
      //************************************************************************

      //************************************************************************
      //@}
      //************************************************************************

     protected:
      //**information methods***************************************************
      /*!\name Info methods*/
      //@{
      int info_tab_width() const noexcept { return 4; }

      struct InfoPrinter {
        InfoPrinter(This const& simulator, ::elastica::widthstream& stream)
            : sim_(simulator), os_(stream) {}
        ~InfoPrinter() = default;

        template <typename TagWrappedAsType>
        void operator()(TagWrappedAsType /*meta*/) const {
          using Update = tmpl::type_from<TagWrappedAsType>;
          // do not use pretty type here as the template carries with it some
          // useful information which gets missed.
          os_ << pretty_type::short_name<Update>() << "\n";

          // clang-format off
          Indent(os_, sim_.info_tab_width())
              // clang-format on
              ([&](::elastica::widthstream& stream) {
                stream << sim_.get_algorithm<Update>().info();
              });
        }

        This const& sim_;
        ::elastica::widthstream& os_;
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Prints information of the post kinematic update steps
       *
       * \param os Stream for printing out
       * \return void
       */
      auto info_update(::elastica::widthstream& os) const noexcept -> void {
        // here we already put in the info that there is only 1 update step
        using Update = tmpl::front<UpdateSteps>;
        os << pretty_type::short_name<Update>() << "\n"
           << get_algorithm<Update>().info();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Prints information of the post kinematic update steps
       *
       * \param os Stream for printing out
       * \return void
       */
      auto info_post_kinematic_updates(
          ::elastica::widthstream& os) const noexcept -> void {
        tmpl::for_each<PostKinematicUpdateSteps>(Printer(*this, os));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Prints information of the pre dynamic update steps
       *
       * \param os Stream for printing out
       * \return void
       */
      auto info_pre_dynamic_updates(::elastica::widthstream& os) const noexcept
          -> void {
        tmpl::for_each<PreDynamicUpdateSteps>(Printer(*this, os));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Prints information of the post dynamic update steps
       *
       * \param os Stream for printing out
       * \return void
       */
      auto info_post_dynamic_updates(::elastica::widthstream& os) const noexcept
          -> void {
        tmpl::for_each<PostDynamicUpdateSteps>(Printer(*this, os));
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
      void initialize_from_traits(SimTraits&& traits) {
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
        SystemsModule::blocking_policy_ =
            std::make_unique<typename SimTraits::BlockingPolicy>(
                std::move(traits).blocking_policy());
        SystemsModule::iteration_policy_ =
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
      static auto initialize(SimTraits&& traits) {
        auto simulator = This::instance();
        simulator->initialize_from_traits(std::move(traits));
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
            ExplicitInterface(),
            algorithms_(nullptr),
            batching_algorithms_(nullptr),
            domain_(nullptr),
            ready_for_integration_(false) {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the Simulation instance";
        });
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
      //! State of simulation finalization
      bool ready_for_integration_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica
