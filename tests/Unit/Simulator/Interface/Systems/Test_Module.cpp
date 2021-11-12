//
// Created by Tejaswin Parthasarathy on 6/26/21.
//

#include <atomic>
#include <memory>
#include <tuple>

//
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
///
#include "Simulator/Configuration.hpp"
#include "Simulator/Interface/Systems/Mocks.hpp"
#include "Simulator/Interface/Systems/Module.hpp"
///
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
/// serializatoin
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/MockSimulatorSerialize.hpp"
#include "Simulator/Interface/Systems/Serialize.hpp"
///
// instead of mocking a block with an emplace() method here, I just chose to
// use a Cosserat rod.
#include "Simulator/Interface/Simulator/Profiling.hpp"
#include "Systems/CosseratRods/CosseratRods.hpp"
#include "Systems/CosseratRods/Mocks/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Serialize.hpp"  // because you decided to put cosserat rods in
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  template <typename, typename>
  struct EmptyComponent {
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
  };

  template <typename, typename>
  struct EmptyComponentButWithADifferentName {
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
  };

  // to test for_each() over a subset of systems
  template <typename, typename>
  struct AnotherEmptyComponent {
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
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

  template <typename Traits>
  class SystemsSimulator
      : public ::elastica::modules::Systems<Traits, SystemsSimulator<Traits>> {
   public:
    using P = ::elastica::modules::Systems<Traits, SystemsSimulator<Traits>>;
    ////////////  This piece of code is copy-pasted yo
    //! Get all algorithm steps
    using AlgorithmSteps = typename P::UpdateSteps;
    //! Algorithm cache type
    using Algorithms = tmpl::as_tagged_tuple<AlgorithmSteps>;
    static constexpr std::size_t n_algorithms =
        tmpl::size<AlgorithmSteps>::value;

    template <typename T>
    struct Transform {
      using type = ::elastica::interface::tags::WcProfilerTag<T>;
    };
    //! Tags for timers profiling the code
    using TimerTags = tmpl::transform<AlgorithmSteps, Transform<tmpl::_1>>;
    //! Cache for timers for profiling the code
    using Timers = tmpl::as_tagged_tuple<TimerTags>;
    ////////////  This piece of code is copy-pasted yo

    using T = Traits;
    using TimeType = typename T::TimeType;

    SystemsSimulator(Traits traits, bool arg_is_final = false)
        : P(),
          ////////////  This piece of code is copy-pasted yo
          algorithms(cpp17::apply(
              [](auto&&... initializer_as_a_tuple) noexcept {
                return Algorithms(
                    std::forward<decltype(initializer_as_a_tuple)>(
                        initializer_as_a_tuple)...);
              },
              repeat_as_tuple<n_algorithms>(
                  std::make_tuple(this, std::move(traits))))),
          ////////////  This piece of code is copy-pasted yo
          is_final(arg_is_final) {
      P::blocking_policy_id() =
          std::make_unique<typename Traits::BlockingPolicy>(
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

    template <typename AlgorithmTag>
    inline constexpr decltype(auto) get_timer() const noexcept {
      using Tag = typename Transform<AlgorithmTag>::type;
      return tuples::get<Tag>(timers_);
    }

   public:
    Algorithms algorithms;
    bool is_final = false;
    mutable Timers timers_;

    // meta
    using typename P::SystemCollections;
    template <typename Plug>
    using as_block = typename P::template as_block<Plug>;
    template <typename L>
    using as_blocks = typename P::template as_blocks<L>;

    // constexpr
    using P::get_number_of_system_collections;
    using P::get_system_collection_container;

    // utility, doesn't need to be test per se.
    using P::blocking_policy;
    using P::iteration_policy;

    // before
    using P::check_if_admissible;

    //
    using P::emplace_back;

    // after
    using P::check_if_registered;
    using P::confirm_as_registered;

    // need not be tested, extensive tests can be found in iteration policy
    using P::to_be_iterated_sequentially;

    // after after
    using P::for_each_block;
    using P::for_each_system;
  };

}  // namespace mock

namespace elastica {

  namespace io {
    template <class Traits>
    struct Serialize<::mock::SystemsSimulator<Traits>> {
      using type = ::mock::SystemsSimulator<Traits>;
      using actual_type = typename type::P;

      // 1. check if blocking is the same, else abort
      template <typename InArchive>
      static inline void load(InArchive& ar, type& systems_module) {
        ar(static_cast<actual_type&>(systems_module));
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& systems_module) {
        ar(static_cast<actual_type const&>(systems_module));
      }
    };

  }  // namespace io

}  // namespace elastica

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
    return Sim(std::move(config));
  }

  // clang-format off
   ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module",
                      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));
    using Sim = decltype(sim);
    static_assert(Sim::get_number_of_system_collections() == 2,
                  "Systems meta-failure!");
    static_assert(
        cpp17::is_same_v<
            typename Sim::SystemCollections,
            std::tuple<std::vector<ECBlock>, std::vector<ECNamedBlock>>>,
        "Systems meta-failure!");
    Sim::check_if_admissible<ECBlock>();
    Sim::check_if_admissible<ECNamedBlock>();

    static_assert(cpp17::is_same_v<typename Sim::as_block<ECPlugin>, ECBlock>,
                  "Systems meta-failure!");
    static_assert(
        cpp17::is_same_v<typename Sim::as_block<ECNamedPlugin>, ECNamedBlock>,
        "Systems meta-failure!");

    using Plugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    static_assert(
        cpp17::is_same_v<typename Sim::as_blocks<Plugins>, ListOfSystems>,
        "Systems meta-failure!");

    (void)(sim.blocking_policy());
    (void)(cpp17::as_const(sim).blocking_policy());
    (void)(sim.iteration_policy());
    (void)(cpp17::as_const(sim).iteration_policy());
    elastica::ignore_unused(sim);
  }

  void test_emplace_back_by_LooselyRestrictSizeAcrossBlockTypes() {
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(LooselyRestrictSizeAcrossBlockTypes{20UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    // check if push back works in accordance with the blocking policy
    const std::size_t n_elements = 13UL;  // boo!

    tmpl::for_each<ListOfSystems>([&](auto p) {
      using Block = tmpl::type_from<decltype(p)>;
      using Plugin = GetPluginFromBlock<Block>;

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));

      // 1 block is now there with 13 UL elements
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      CHECK(::elastica::size(
                sim.get_system_collection_container<Block>()[0UL]) == 13UL);

      // another
      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));

      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      // no need to check size of each slices here

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      // now the policy has no chocie but to add another block
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            2UL);
    });
  }

  void test_emplace_back_by_RestrictSizeAcrossBlockTypes() {
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    // check if push back works in accordance with the blocking policy
    const std::size_t n_elements = 8UL;

    tmpl::for_each<ListOfSystems>([&](auto p) {
      using Block = tmpl::type_from<decltype(p)>;
      using Plugin = GetPluginFromBlock<Block>;

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));

      // 1 block is now there with 8 UL elements
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      CHECK(
          ::elastica::size(sim.get_system_collection_container<Block>()[0UL]) ==
          n_elements);

      // another
      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));

      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      // no need to check size of each slices here

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      // now the policy has no choice but to add another block
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            2UL);
    });
  }

  // needed for validationg the apply_over_blcoks test
  void test_emplace_back_by_LooselyRestrictCustomizedToBlockTypes() {
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    const std::size_t n_elements = 8UL;
    auto sim = make_systems_simulator<ListOfSystems>(
        // check if push back works in accordance with the blocking policy
        make_blocking_policy(
            LooselyRestrictSizeOf<ECBlock>{2 * n_elements},
            LooselyRestrictSizeOf<ECNamedBlock>{n_elements - 1UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    {
      using Plugin = ECPlugin;
      using Block = ECBlock;

      for (std::size_t i = 0UL; i < 2UL; ++i) {
        sim.emplace_back<Plugin>(
            ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                blocks::initialize<::elastica::tags::NElement>(n_elements)));
        // 1 block is now there with 8 UL elements
        CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
              1UL);
      }

      for (std::size_t i = 0UL; i < 2UL; ++i) {
        sim.emplace_back<Plugin>(
            ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                blocks::initialize<::elastica::tags::NElement>(n_elements)));
        // 1 block is now there with 8 UL elements
        CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
              2UL);
      }
    }

    {
      using Plugin = ECNamedPlugin;
      using Block = ECNamedBlock;
      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      // 1 block is now there with 8 UL elements
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      // 1 block is now there with 8 UL elements
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            2UL);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.emplace_back",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    // I just test the two most complex here as if they work, the rest will
    // also work.
    test_emplace_back_by_LooselyRestrictSizeAcrossBlockTypes();
    test_emplace_back_by_RestrictSizeAcrossBlockTypes();
    test_emplace_back_by_LooselyRestrictCustomizedToBlockTypes();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.n_systems",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    // I just test the two most complex here as if they work, the rest will
    // also work.

    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(LooselyRestrictSizeAcrossBlockTypes{20UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    // check if push back works in accordance with the blocking policy
    const std::size_t n_elements = 13UL;  // boo!
    std::size_t n_system_count = 0UL;

    tmpl::for_each<ListOfSystems>([&](auto p) {
      using Block = tmpl::type_from<decltype(p)>;
      using Plugin = GetPluginFromBlock<Block>;

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      CHECK(sim.n_systems() == ++n_system_count);

      // another
      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      CHECK(sim.n_systems() == ++n_system_count);

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      CHECK(sim.n_systems() == ++n_system_count);
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.exception_testing_for_emplace_back",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));
    sim.finalize();

    {
      using Plugin = ECPlugin;
      CHECK_THROWS_WITH(
          sim.emplace_back<Plugin>(
              ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                  blocks::initialize<::elastica::tags::NElement>(200UL))),
          Catch::Contains("simulator has been finalized"));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.check_if_registered",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));

    tmpl::for_each<ListOfSystems>([&](auto p) {
      using Block = tmpl::type_from<decltype(p)>;
      using Plugin = GetPluginFromBlock<Block>;
      const std::size_t n_elements = 16UL;

      CHECK(sim.template get_system_collection_container<Block>().empty());

      auto sliced = sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      CHECK(sim.check_if_registered(sliced));

      // todo generate a slice
      Block block{};
      auto another_sliced = block.emplace(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      CHECK_FALSE(sim.check_if_registered(another_sliced));
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.confirm_as_registered",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));

    tmpl::for_each<ListOfSystems>([&](auto p) {
      using Block = tmpl::type_from<decltype(p)>;
      using Plugin = GetPluginFromBlock<Block>;
      const std::size_t n_elements = 16UL;

      CHECK(sim.template get_system_collection_container<Block>().empty());

      auto sliced = sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      CHECK(sim.check_if_registered(sliced));
      CHECK_NOTHROW(sim.confirm_as_registered(sliced));

      // todo generate a slice
      Block block{};
      auto another_sliced = block.emplace(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      CHECK_THROWS_WITH(sim.confirm_as_registered(another_sliced),
                        Catch::Contains("Input system was not found"));
    });
  }
  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.to_be_iterated_sequentially",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));
    CHECK(sim.to_be_iterated_sequentially(
        sim.get_system_collection_container<ECBlock>()));
    CHECK(sim.to_be_iterated_sequentially(
        sim.get_system_collection_container<ECNamedBlock>()));
  }

  inline decltype(auto) setup_sim_with_state(const std::size_t n_units) {
    const std::size_t n_elements = 16UL;
    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock, AnotherECBlock>;
    auto sim = make_systems_simulator<ListOfSystems>(make_blocking_policy(
        LooselyRestrictSizeOf<ECBlock>{2 * n_elements},
        LooselyRestrictSizeOf<ECNamedBlock>{n_elements - 1UL},
        RestrictSizeOf<AnotherECBlock>{n_elements}));
    // gives n_units / 2 blocks of ECBlock and n_units ECNamedBlock
    tmpl::for_each<ListOfSystems>([&](auto p) {
      using Block = tmpl::type_from<decltype(p)>;
      using Plugin = GetPluginFromBlock<Block>;
      for (auto i = 0UL; i < n_units; ++i)
        sim.template emplace_back<Plugin>(
            ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                blocks::initialize<::elastica::tags::NElement>(n_elements)));
    });
    return sim;
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.for_each_block",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    const std::size_t n_units = 4UL;
    auto sim = setup_sim_with_state(n_units);

    // apply at block granuliarity
    const std::size_t expected_n_ec_blocks = n_units / 2UL;
    const std::size_t expected_n_ec_named_blocks = n_units;
    const std::size_t expected_another_ec_blocks = n_units;
    std::atomic<std::size_t> ec_block_counter(0UL), ec_named_block_counter(0UL),
        another_ec_block_counter(0UL);
    sim.for_each_block(make_overloader(
        [&](ECBlock const&) { ++ec_block_counter; },
        [&](ECNamedBlock const&) { ++ec_named_block_counter; },
        [&](AnotherECBlock const&) { ++another_ec_block_counter; }));

    CHECK(ec_block_counter == expected_n_ec_blocks);
    CHECK(ec_named_block_counter == expected_n_ec_named_blocks);
    CHECK(another_ec_block_counter == expected_another_ec_blocks);

    std::atomic<std::size_t> only_ec_block_counter(0UL);
    sim.for_each_block<tmpl::list<ECBlock>>(
        [&](ECBlock const&) { ++only_ec_block_counter; });
    CHECK(only_ec_block_counter == expected_n_ec_blocks);

    std::atomic<std::size_t> subset_counter(0UL);
    sim.for_each_block<tmpl::list<ECNamedBlock, AnotherECBlock>>(
        [&](auto const&) { ++subset_counter; });
    CHECK(subset_counter ==
          (expected_n_ec_named_blocks + expected_another_ec_blocks));

    std::atomic<std::size_t> n_elements_in_ec_named_block(0UL),
        n_elements_in_another_ec_block(0UL);
    sim.for_each_block<tmpl::list<ECNamedBlock, AnotherECBlock>>(
        make_overloader(
            [&](ECNamedBlock const& b) {
              n_elements_in_ec_named_block += ::elastica::size(b);
            },
            [&](AnotherECBlock const& b) {
              n_elements_in_another_ec_block += ::elastica::size(b);
            }));
    // this works because each block only has 1 unit, so size(block) =
    // size(unit) = n_elements
    CHECK(n_elements_in_ec_named_block == 64UL);
    CHECK(n_elements_in_another_ec_block == 64UL);
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.for_each_system",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    const std::size_t n_units = 4UL;
    auto sim = setup_sim_with_state(n_units);

    // apply at block granuliarity
    //    const std::size_t expected_n_ec_blocks = n_units / 2UL;
    //    const std::size_t expected_n_ec_named_blocks = n_units;

    const std::size_t n_elements = 24UL;
    auto ec_sliced = sim.emplace_back<ECPlugin>(
        ::elastica::cosserat_rod::initialize_cosserat_rod<ECPlugin>(
            blocks::initialize<::elastica::tags::NElement>(n_elements)));
    auto ec_named_sliced = sim.emplace_back<ECNamedPlugin>(
        ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
            blocks::initialize<::elastica::tags::NElement>(n_elements)));
    auto another_ec_sliced = sim.emplace_back<AnotherECPlugin>(
        ::elastica::cosserat_rod::initialize_cosserat_rod<AnotherECPlugin>(
            blocks::initialize<::elastica::tags::NElement>(n_elements)));
    {
      sim.emplace_back<ECNamedPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      sim.emplace_back<ECNamedPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
      sim.emplace_back<ECNamedPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));

      sim.emplace_back<AnotherECPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<AnotherECPlugin>(
              blocks::initialize<::elastica::tags::NElement>(n_elements)));
    }

    std::atomic<std::size_t> ec_slice_counter(0UL), ec_named_slice_counter(0UL),
        another_ec_slice_counter(0UL);
    sim.for_each_system(make_overloader(
        [&](decltype(ec_sliced) const&) { ++ec_slice_counter; },
        [&](decltype(ec_named_sliced) const&) { ++ec_named_slice_counter; },
        [&](decltype(another_ec_sliced) const&) {
          ++another_ec_slice_counter;
        }));

    CHECK(ec_slice_counter == 5UL);
    CHECK(ec_named_slice_counter == 8UL);
    CHECK(another_ec_slice_counter == 6UL);

    std::atomic<std::size_t> n_elements_in_ec_block(0UL);
    sim.for_each_system<tmpl::list<ECPlugin>>(
        [&](decltype(ec_sliced) const& b) {
          n_elements_in_ec_block += ::elastica::size(b);
        });
    // here if we had looped over the block we would have got a different number
    // due to ghosting.
    CHECK(n_elements_in_ec_block == (16 * n_units + n_elements));

    std::atomic<std::size_t> subset_counter(0UL);
    sim.for_each_system<tmpl::list<ECNamedPlugin, AnotherECPlugin>>(
        [&](auto const&) { ++subset_counter; });
    CHECK(subset_counter == (8UL + 6UL));
  }

  // TODO : test serialization
  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.serialize",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using namespace elastica::io;
    // NOTHROW case
    {
      auto sim = setup_sim_with_state(20UL);
      std::stringstream ss;  // any stream can be used
      //std::size_t n_elements(16UL);

      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        // modify the members a little
        sim.for_each_system([](auto& b) {
          // for minimal cosserat traits we have a vector
          blocks::get<::elastica::tags::NElement>(b)[0UL] += 2UL;
        });

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        // from sim
        auto another_sim = setup_sim_with_state(20UL);
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        // modify the members a little
        another_sim.for_each_system([](auto const& b) {
          // for minimal cosserat traits we have a vector
          CHECK(blocks::get<::elastica::tags::NElement>(b)[0UL] == 16UL);
        });

        CHECK_NOTHROW(another_sim.finalize());
        CHECK_NOTHROW(iarchive(another_sim));

        // after no throw see if it has the correct ones
        another_sim.for_each_system([](auto const& b) {
          // for minimal cosserat traits we have a vector
          CHECK(blocks::get<::elastica::tags::NElement>(b)[0UL] == 18UL);
        });
      }
    } // nothrow case
  }

}  // namespace
