//
// Created by Tejaswin Parthasarathy on 6/18/21.
//

#pragma once

#include <cstdint>
#include <tuple>

#include "Simulator/Configuration.hpp"
#include "Simulator/Interface/Batching/DefaultBatcher.hpp"
#include "Simulator/Utilities/Timers.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace mock {

  template <typename ModuleTraits>
  struct MockSingleComponentSimulationTraits : public ModuleTraits {
    using TimeType = elastica::Time;
    using SizeType = std::size_t;
    using IndexType = std::size_t;
    using Parent = ModuleTraits;

    // using Parent::Parent;
    explicit MockSingleComponentSimulationTraits(Parent m)
        : Parent(std::move(m)){};
    ~MockSingleComponentSimulationTraits() = default;
  };

  template <typename ModuleTraits>
  using MockTraits = MockSingleComponentSimulationTraits<ModuleTraits>;

  template <typename Config, typename... Configs>
  inline constexpr decltype(auto) make_simulator_configuration(
      Config config, Configs... configs) noexcept {
    struct T : public Config,
               public Configs...,
               public ::elastica::configuration::DefaultParallelConfiguration {
      T(Config c, Configs... cs)
          : Config(std::move(c)), Configs(std::move(cs))... {}
      T(T const&) = default;
      T(T&&) = default;
    };
    return MockTraits<T>(T(std::move(config), std::move(configs)...));
  }

  //  template <typename... Configs>
  //  inline constexpr decltype(auto) make_simulator_configuration(
  //      Configs... configs) noexcept {
  //    using C = typename tmpl::inherit<Configs...>::type;
  //    C c(configs...);
  //    return MockTraits<C>(std::move(c));
  //  }

  /*
   * Mocks a minimal simulator with stratightforward logic to test the different
   * modules
   */
  template <typename SimTraits,  // Configuration parameter
            template <typename /*SimTraits*/, typename /*Simulator*/>
            class Module>  // Additional modules
  class MockSingleComponentSimulator
      :  // protected ::elastica::NonCopyable,
         public Module<SimTraits,
                       MockSingleComponentSimulator<SimTraits, Module>> {
   public:
    using M =
        Module<SimTraits, MockSingleComponentSimulator<SimTraits, Module>>;

    ////////////  This piece of code is copy-pasted yo
    //! Get all algorithm steps
    using AlgorithmSteps = tmpl::append<
        typename M::UpdateSteps, typename M::PostKinematicUpdateSteps,
        typename M::PreDynamicUpdateSteps, typename M::PostDynamicUpdateSteps>;
    //! Algorithm cache type
    using Algorithms = tmpl::as_tagged_tuple<AlgorithmSteps>;
    using DefaultBatcher = ::elastica::interface::detail::DefaultBatcher;
    //! Get all batchers algorithm steps
    using Batchers =
        tmpl::append<tmpl::list<DefaultBatcher>, typename M::BatchingAlgorithm>;
    //! Batching algorithm cache type
    using BatchingAlgorithms = tmpl::as_tagged_tuple<Batchers>;
    static constexpr std::size_t n_algorithms =
        tmpl::size<AlgorithmSteps>::value;
    static constexpr std::size_t n_batching_algorithms =
        tmpl::size<Batchers>::value;
    //************************************************************************
    /*!\name Profiler type definitions */
    //@{
    //! Cache for timers for profiling the code
    using Timers = ::elastica::Timers<AlgorithmSteps>;
    //@}
    //************************************************************************
    ////////////  This piece of code is copy-pasted yo

    MockSingleComponentSimulator(SimTraits& traits, bool arg_all_registered,
                                 bool arg_is_final = false)
        : M(),
          is_final(arg_is_final),
          all_registered(arg_all_registered),
          ////////////  This piece of code is copy-pasted yo
          algorithms(std::apply(
              [](auto&&... initializer_as_a_tuple) noexcept {
                return Algorithms(
                    std::forward<decltype(initializer_as_a_tuple)>(
                        initializer_as_a_tuple)...);
              },
              repeat_as_tuple<n_algorithms>(std::make_tuple(this, traits)))),
          ////////////  This piece of code is copy-pasted yo
          ////////////  This piece of code is copy-pasted yo
          batching_algorithms(std::apply(
              [](auto&&... initializer_as_a_tuple) noexcept {
                return BatchingAlgorithms(
                    std::forward<decltype(initializer_as_a_tuple)>(
                        initializer_as_a_tuple)...);
              },
              repeat_as_tuple<n_batching_algorithms>(
                  // can use forward as tuple here in case
                  traits)))
          ////////////  This piece of code is copy-pasted yo
          {};

    MockSingleComponentSimulator(MockSingleComponentSimulator&&) noexcept =
        default;
    ~MockSingleComponentSimulator() = default;
    using T = SimTraits;
    using TimeType = typename T::TimeType;

   public:
    void finalize(TimeType time_v = {}) {
      M::finalize(time_v);
      is_final = true;
    }

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

    //************************************************************************
    /*!\brief Get an algorithm from the simulator cache
     *
     * \tparam BatchingAlgorithmTag Tag for obtaining the algorithm
     * \return Algorithm callable
     */
    template <typename BatchingAlgorithmTag>
    inline constexpr decltype(auto) get_batching_algorithm() const noexcept {
      return tuples::get<BatchingAlgorithmTag>(batching_algorithms);
    }
    //************************************************************************

   public:  // For access to modules such as with primitives
    //**Profiling methods*****************************************************
    /*!\name Profiling methods*/
    //@{

    //************************************************************************
    /*!\brief Get a timer corresponding to an algorithm
     *
     * \tparam AlgorithmTag Tag for obtaining the algorithm
     * \return Algorithm callable
     */
    template <typename AlgorithmTag>
    inline constexpr decltype(auto) get_timer() & noexcept {
      using ::elastica::get_meter;
      return get_meter<AlgorithmTag>(timers);
    }

    template <typename AlgorithmTag>
    inline constexpr decltype(auto) get_timer() const& noexcept {
      using ::elastica::get_meter;
      return get_meter<AlgorithmTag>(timers);
    }
    //************************************************************************

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

    template <typename SystemCollection>
    inline auto check_if_registered(SystemCollection const&) const noexcept
        -> bool {
      return all_registered;
    }

    template <typename SystemCollection>
    void confirm_as_registered(SystemCollection const& system) const {
      if (not check_if_registered(system))
        throw std::runtime_error(
            "Input system was not found to be registered in the simulator");
    }

    inline auto ready() const noexcept -> bool { return is_final; }
    // don't check this here, it is checked elsewhere. We just provide the
    // interface
    template <typename F>
    void for_each_system(F) const& noexcept {}
    template <typename F>
    void for_each_system(F) & noexcept {}
    template <typename F>
    void for_each_block(F) const& noexcept {}
    template <typename F>
    void for_each_block(F) & noexcept {}
    template <typename F>
    void for_each_system_untimed(F) const& noexcept {}
    template <typename F>
    void for_each_system_untimed(F) & noexcept {}
    template <typename F>
    void for_each_block_untimed(F) const& noexcept {}
    template <typename F>
    void for_each_block_untimed(F) & noexcept {}

   public:
    bool is_final = false;
    bool all_registered = false;
    Algorithms algorithms;
    BatchingAlgorithms batching_algorithms;
    Timers timers;
  };

  template <typename SimTraits,  // Configuration parameter
            template <typename /*SimTraits*/, typename /*Simulator*/>
            class Module>  // Additional modules
  using MockSimulator = MockSingleComponentSimulator<SimTraits, Module>;

  template <template <typename /*Simulator*/,  // base simulator
                      typename /*SimTraits*/>  // simulator traits
            class AddOns,                      // all capabilities
            typename SimulationConfig_, typename... Args>
  auto make_simulator(SimulationConfig_ config, Args&&... args) {
    return MockSimulator<SimulationConfig_, AddOns>(
        config, std::forward<Args>(args)...);
  }

}  // namespace mock
