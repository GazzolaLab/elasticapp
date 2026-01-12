//
// Created by Tejaswin Parthasarathy on 6/27/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration.hpp"
#include "Simulator/Interface/Systems/Mocks.hpp"
#include "Simulator/Interface/Systems/Module.hpp"
#include "Simulator/Interface/TimeSteppers/SymplecticSteppers.hpp"
#include "Simulator/Utilities/Timers.hpp"
#include "Systems/Block.hpp"
#include "Systems/CosseratRods.hpp"  // have access here so that external loads are qualified
#include "Systems/CosseratRods/Mocks/CosseratRodTraits.hpp"
#include "Time/Protocols.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace ::elastica::configuration;
}  // namespace

namespace {
  struct Dummy {
    // no-op
    template <typename T>
    Dummy& operator+=(T&&) {
      return *this;
    }
  };

  struct Assignable {
    template <typename T>
    void operator=(T) {}
  };

  // copied from Test_Module, with some changes
  template <typename, typename>
  struct EmptyComponent {
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}

    auto kinematic_states() { return Dummy(); }
    auto dynamic_states() { return Dummy(); }

    // dummy
    int kinematic_rates(...) { return 2; }
    int dynamic_rates(...) { return 2; }

    void compute_internal_dynamics(::elastica::Time) { /* no-op*/
    }

    /* For conformance with external_loads and torques*/
    auto get_external_loads() & { return Assignable{}; }
    auto get_external_loads() const& { return Assignable{}; }
    auto get_external_torques() & { return Assignable{}; }
    auto get_external_torques() const& { return Assignable{}; }
  };

  template <typename, typename>
  struct EmptyComponentButWithADifferentName {
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}

    auto kinematic_states() { return Dummy(); }
    auto dynamic_states() { return Dummy(); }
    // dummy
    int kinematic_rates(...) { return 2; }
    int dynamic_rates(...) { return 2; }

    void compute_internal_dynamics(::elastica::Time) { /* no-op*/
    }

    /* For conformance with external_loads and torques*/
    auto get_external_loads() & { return Assignable{}; }
    auto get_external_loads() const& { return Assignable{}; }
    auto get_external_torques() & { return Assignable{}; }
    auto get_external_torques() const& { return Assignable{}; }
  };

  // to test for_each() over a subset of systems
  template <typename, typename>
  struct AnotherEmptyComponent {
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
    auto kinematic_states() { return Dummy(); }
    auto dynamic_states() { return Dummy(); }

    // dummy
    int kinematic_rates(...) { return 2; }
    int dynamic_rates(...) { return 2; }

    void compute_internal_dynamics(::elastica::Time) { /* no-op*/
    }

    /* For conformance with external_loads and torques*/
    auto get_external_loads() & { return Assignable{}; }
    auto get_external_loads() const& { return Assignable{}; }
    auto get_external_torques() & { return Assignable{}; }
    auto get_external_torques() const& { return Assignable{}; }
  };

}  // namespace

namespace {

  template <template <typename, typename> class... T>
  using GenerateTestPlugin = ::elastica::cosserat_rod::CosseratRodPlugin<
      ::mock::MinimalCosseratRodTraits, ::blocks::Block, T...>;
  template <typename Plugin>
  using GenerateBlock = blocks::Block<Plugin>;

  template <typename Block>
  using GetPluginFromBlock = typename Block::PluginType;

  using ECPlugin = GenerateTestPlugin<EmptyComponent>;
  using ECNamedPlugin = GenerateTestPlugin<EmptyComponentButWithADifferentName>;
  using AnotherECPlugin = GenerateTestPlugin<AnotherEmptyComponent>;

  using ECBlock = GenerateBlock<ECPlugin>;
  using ECNamedBlock = GenerateBlock<ECNamedPlugin>;
  using AnotherECBlock = GenerateBlock<AnotherECPlugin>;

}  // namespace

namespace mock {

  // copied from Systems/Test_Module.cpp with modifications
  template <typename Traits>
  class SystemsSimulator
      : public ::elastica::modules::Systems<Traits, SystemsSimulator<Traits>>,
        public ::elastica::interface::InterfaceWithSymplecticSteppers<
            Traits, SystemsSimulator<Traits>> {
   public:
    using P = ::elastica::modules::Systems<Traits, SystemsSimulator<Traits>>;
    ////////////  This piece of code is copy-pasted yo
    //! Get all algorithm steps
    using AlgorithmSteps = typename P::UpdateSteps;
    //! Algorithm cache type
    using Algorithms = tmpl::as_tagged_tuple<AlgorithmSteps>;
    static constexpr std::size_t n_algorithms =
        tmpl::size<AlgorithmSteps>::value;

    //! Cache for timers for profiling the code
    using Timers = ::elastica::Timers<AlgorithmSteps>;

    ////////////  This piece of code is copy-pasted yo

    using T = Traits;
    using TimeType = typename T::TimeType;

    SystemsSimulator(Traits traits, bool arg_is_final = false)
        : P(),
          ////////////  This piece of code is copy-pasted yo
          algorithms(std::apply(
              [](auto&&... initializer_as_a_tuple) noexcept {
                return Algorithms(
                    std::forward<decltype(initializer_as_a_tuple)>(
                        initializer_as_a_tuple)...);
              },
              repeat_as_tuple<n_algorithms>(
                  std::make_tuple(this, std::move(traits))))),
          ////////////  This piece of code is copy-pasted yo
          is_final(arg_is_final),
          order_of_steps() {
      P::blocking_policy() = ::elastica::configuration::virtualize(
          std::move(traits).blocking_policy());
      P::iteration_policy_id() =
          std::make_unique<typename Traits::IterationPolicy>(
              std::move(traits).iteration_policy());
    }
    ~SystemsSimulator() = default;
    SystemsSimulator(SystemsSimulator&&) noexcept = default;

    void finalize(TimeType = {}) { is_final = true; }

    // ditto copy
    auto check_if_not_final() const -> void {
      const char* error_message = R"error(
The simulator has been finalized, yet you are trying to modify its states!
Please ensure that Simulator doesn't change state after finalize() is called by
reorganizing your code logic.
)error";
      if (ready())
        throw std::logic_error(error_message);
    }
    inline auto ready() const noexcept -> bool { return is_final; }

    // copied
    //************************************************************************
    /*!\brief Get an algorithm from the simulator cache
     *
     * \tparam AlgorithmTag Tag for obtaining the algorithm
     * \return Algorithm callable
     */
    template <typename AlgorithmTag>
    inline constexpr decltype(auto) get_algorithm() const noexcept {
      return tuples::get<AlgorithmTag>(algorithms);
    }
    //************************************************************************
    void apply_post_kinematic_updates(TimeType) {
      order_of_steps.emplace_back("post_kinematic_update");
    }
    void apply_pre_dynamic_updates(TimeType) {
      order_of_steps.emplace_back("pre_dynamic_update");
    }
    void apply_post_dynamic_updates(TimeType) {
      order_of_steps.emplace_back("post_dynamic_update");
    }
    template <typename F>
    inline decltype(auto) for_each_block_update_kinematics(F func) noexcept {
      order_of_steps.emplace_back("for_each_block_update_kinematics");
      return P::for_each_block_update_kinematics(std::move(func));
    }

    template <typename F>
    inline decltype(auto) for_each_block_update_dynamics(F func) noexcept {
      order_of_steps.emplace_back("for_each_block_update_dynamics");
      return P::for_each_block_update_dynamics(std::move(func));
    }

    template <typename F>
    inline decltype(auto) for_each_block_compute_internal_dynamics(
        F func) noexcept {
      order_of_steps.emplace_back("for_each_block_compute_internal_dynamics");
      return P::for_each_block_compute_internal_dynamics(std::move(func));
    }

    //************************************************************************
    template <typename F>
    inline decltype(auto) for_each_block(F func) noexcept {
      order_of_steps.emplace_back("for_each_block");
      return P::for_each_block(std::move(func));
    }
    //************************************************************************

    template <typename AlgorithmTag>
    inline constexpr decltype(auto) get_timer() & noexcept {
      using ::elastica::get_meter;
      return get_meter<AlgorithmTag>(timers_);
    }

    template <typename AlgorithmTag>
    inline constexpr decltype(auto) get_timer() const& noexcept {
      using ::elastica::get_meter;
      return get_meter<AlgorithmTag>(timers_);
    }

   public:
    Algorithms algorithms;
    bool is_final = false;
    std::vector<std::string> order_of_steps;
    mutable Timers timers_;
  };

}  // namespace mock

namespace {

  template <typename ListOfSystems, typename Arg>
  inline decltype(auto) make_systems_simulator(Arg&& arg) {
    auto make_config = [&arg]() {
      return ::mock::make_systems_configuration<ListOfSystems>(
          // this does not matter
          std::forward<Arg>(arg),
          make_iteration_policy(iterate_across_system_types_in(seq),
                                // for testing iteration policy
                                iterate_over_systems_in(hybrid),
                                AllSerialBelow(500UL)));
    };
    auto config = make_config();
    //
    using Config = std::decay_t<decltype(config)>;
    using Sim = ::mock::SystemsSimulator<Config>;
    static_assert(
        ::tt::conforms_to_v<
            Sim, ::elastica::timestepper::protocols::SymplecticSteppable>,
        "Symplectic meta failure!");

    return Sim(std::move(config));
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Time.SymplecticSteppers",
                     "[Unit][Simulator][Interface][Time]") {
    // clang-format on
    using ListOfSystemPlugins =
        tmpl::list<ECPlugin, ECNamedPlugin, AnotherECPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));
    using Sim IGNORE_UNUSED = decltype(sim);
    elastica::ignore_unused(sim);

    using Algorithm = ::elastica::timestepper::symplectic::PositionVerlet;
    using TimeStepper =
        ::elastica::timestepper::symplectic::SymplecticStepper<Algorithm>;
    const TimeStepper time_stepper{};

    ::elastica::Time time_v{0.0};
    ::elastica::TimeDelta time_delta{0.2};  // dont do nan bro
    ::elastica::TimeStepID time_iteration{
        time_stepper.n_time_updates_per_step()};
    sim.step(time_stepper, time_v, time_delta, time_iteration);
    CHECK(time_iteration.step() == 1UL);
    CHECK(time_iteration.substep() == 0UL);

    // position verlet has
    // one kinematic step , followed by dynamic, followed by kinematic
    // for_each_block, dynamic, for_each_block
    // for_each_block, (post_kinematic_update, for_each_block, pre_dynamic,
    // for_each_block, post_dynamic), for_each_block
    std::vector<std::string> expected;
    expected.emplace_back("for_each_block_update_kinematics");
    expected.emplace_back("post_kinematic_update");
    expected.emplace_back("for_each_block_compute_internal_dynamics");
    expected.emplace_back("pre_dynamic_update");
    expected.emplace_back("for_each_block_update_dynamics");
    expected.emplace_back("post_dynamic_update");
    expected.emplace_back("for_each_block_update_kinematics");
    CHECK(sim.order_of_steps == expected);
  }

  // TODO : Do it with a cosserat rod.

}  // namespace