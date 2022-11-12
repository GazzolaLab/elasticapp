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
#include "Simulator/Interface/Systems/BlockRefs.hpp"
#include "Simulator/Interface/Systems/Serialize.hpp"
///
// instead of mocking a block with an emplace() method here, I just chose to
// use a Cosserat rod.
#include "Simulator/Utilities/Timers.hpp"
#include "Systems/CosseratRods/CosseratRods.hpp"
#include "Systems/CosseratRods/Mocks/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Serialize.hpp"  // because you decided to put cosserat rods in
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/Size.hpp"

namespace {
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  //////// The following code block is copy pasted from another source
  template <typename Traits, typename Block>
  struct AlwaysNeededComponentFaked
      : public ::elastica::CRTPHelper<Block, AlwaysNeededComponentFaked> {
    using CRTP = ::elastica::CRTPHelper<Block, AlwaysNeededComponentFaked>;
    using CRTP::self;

    struct Position : public Traits::template CosseratRodVariable<
                          ::elastica::tags::Position,         //
                          typename Traits::DataType::Vector,  //
                          typename Traits::Place::OnNode> {};
    struct Director : public Traits::template CosseratRodInitializedVariable<
                          ::elastica::tags::Director,  //
                          typename Traits::DataType::Matrix,
                          typename Traits::Place::OnElement> {};
    struct Velocity : public Traits::template CosseratRodInitializedVariable<
                          ::elastica::tags::Velocity,         //
                          typename Traits::DataType::Vector,  //
                          typename Traits::Place::OnNode> {};
    struct AngularVelocity
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::AngularVelocity,  //
              typename Traits::DataType::Vector,
              typename Traits::Place::OnElement> {};
    struct Acceleration
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::Acceleration,     //
              typename Traits::DataType::Vector,  //
              typename Traits::Place::OnNode> {};
    struct AngularAcceleration
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::AngularAcceleration,  //
              typename Traits::DataType::Vector,
              typename Traits::Place::OnElement> {};
    inline constexpr decltype(auto) get_position() & noexcept {
      return blocks::get<::elastica::tags::Position>(self());
    }
    inline constexpr decltype(auto) get_position() const& noexcept {
      return blocks::get<::elastica::tags::Position>(self());
    }

    inline constexpr decltype(auto) get_director() & noexcept {
      return blocks::get<::elastica::tags::Director>(self());
    }
    inline constexpr decltype(auto) get_director() const& noexcept {
      return blocks::get<::elastica::tags::Director>(self());
    }

    inline constexpr decltype(auto) get_velocity() & noexcept {
      return blocks::get<::elastica::tags::Velocity>(self());
    }
    inline constexpr decltype(auto) get_velocity() const& noexcept {
      return blocks::get<::elastica::tags::Velocity>(self());
    }

    inline constexpr decltype(auto) get_angular_velocity() & noexcept {
      return blocks::get<::elastica::tags::AngularVelocity>(self());
    }
    inline constexpr decltype(auto) get_angular_velocity() const& noexcept {
      return blocks::get<::elastica::tags::AngularVelocity>(self());
    }
    inline constexpr decltype(auto) get_acceleration() & noexcept {
      return blocks::get<::elastica::tags::Acceleration>(self());
    }
    inline constexpr decltype(auto) get_acceleration() const& noexcept {
      return blocks::get<::elastica::tags::Acceleration>(self());
    }
    inline constexpr decltype(auto) get_angular_acceleration() & noexcept {
      return blocks::get<::elastica::tags::AngularAcceleration>(self());
    }
    inline constexpr decltype(auto) get_angular_acceleration() const& noexcept {
      return blocks::get<::elastica::tags::AngularAcceleration>(self());
    }

    using ComputedVariables =
        tmpl::list<Position, Director, Velocity, AngularVelocity, Acceleration,
                   AngularAcceleration>;
  };
  ////////

  template <typename Traits, typename Block>
  struct AlwaysNeededComponent {
    using ComputedVariables = tmpl::list<>;
  };

  template <typename T, typename B>
  struct EmptyComponent : public AlwaysNeededComponent<T, B> {
    using P = AlwaysNeededComponent<T, B>;
    using InitializedVariables = tmpl::list<>;
    using typename P::ComputedVariables;
    using Variables = ComputedVariables;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
  };

  template <typename T, typename B>
  struct EmptyComponentButWithADifferentName
      : public AlwaysNeededComponent<T, B> {
    using P = AlwaysNeededComponent<T, B>;
    using InitializedVariables = tmpl::list<>;
    using typename P::ComputedVariables;
    using Variables = ComputedVariables;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
  };

  // to test for_each() over a subset of systems
  template <typename T, typename B>
  struct AnotherEmptyComponent : public AlwaysNeededComponent<T, B> {
    using P = AlwaysNeededComponent<T, B>;
    using InitializedVariables = tmpl::list<>;
    using typename P::ComputedVariables;
    using Variables = ComputedVariables;

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

    //! Cache for timers for profiling the code
    using Timers = ::elastica::Timers<AlgorithmSteps>;
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

    // rest
    using P::n_total_systems;  // for testing reset
    using P::reset_module;
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

  auto iteration_policy_for_testing(std::size_t parameter) {
    return make_iteration_policy(iterate_across_system_types_in(seq),
                                 // for testing iteration policy
                                 iterate_over_systems_in(hybrid),
                                 AllSerialBelow(parameter));
  }

  template <typename ListOfSystems, typename Arg>
  inline decltype(auto) make_systems_simulator(Arg&& arg) {
    auto make_config = [&arg]() {
      return ::mock::make_systems_configuration<ListOfSystems>(
          // this does not matter
          std::forward<Arg>(arg), iteration_policy_for_testing(500UL));
    };
    auto config = make_config();
    //
    using Config = std::decay_t<decltype(config)>;
    using Sim = ::mock::SystemsSimulator<Config>;
    return Sim(std::move(config));
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.BlockRefs",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystemPlugins = tmpl::list<ECPlugin>;  // , ECNamedPlugin>;
    tmpl::for_each<ListOfSystemPlugins>([&](auto p) {
      using Plugin = tmpl::type_from<decltype(p)>;
      using Block = GenerateBlock<Plugin>;

      std::vector<Block> vs;
      // Make 3 blocks;
      vs.resize(3);

      for (std::size_t i = 0UL; i < 3UL; ++i) {
        for (std::size_t j = 0UL; j < (1 + 2 * i); ++j) {
          vs[i].emplace(
              ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                  blocks::initialize<::elastica::tags::NElement>(
                      // at least two elements needed
                      [=]() { return 2UL + i + j; })));
        }
      }

      SECTION("BlockRefGenerator") {
        {
          blocks::BlockRefGenerator<Plugin> ref;
          CHECK(ref.source() == nullptr);
          CHECK(&ref.parent() == &ref);
        }

        {
          blocks::BlockRefGenerator<Plugin> ref(vs, 1UL);
          CHECK(ref.source() == &vs);
          CHECK(&ref.parent() == &ref);
          CHECK(ref.index() == 1UL);
          CHECK(&ref.get() == &vs[1UL]);
          CHECK(&blocks::generate_from(ref) == &vs[1UL]);
        }

        {
          blocks::BlockRefGenerator<Plugin> aref(vs, 1UL);
          blocks::BlockRefGenerator<Plugin> bref(vs, 2UL);
          CHECK(aref.uuid() == aref.uuid());
          CHECK(aref.uuid() != bref.uuid());
        }

        // Comparison operators
        {
          blocks::BlockRefGenerator<Plugin> aref(vs, 1UL);
          blocks::BlockRefGenerator<Plugin> bref(vs, 1UL);
          blocks::BlockRefGenerator<Plugin> cref(vs, 2UL);
          CHECK(aref == aref);
          CHECK(aref == bref);
          CHECK(aref != cref);

          CHECK(aref < cref);

          CHECK(cref > bref);

          CHECK(aref <= bref);
          CHECK(aref <= cref);

          CHECK(cref >= aref);
          CHECK(cref >= bref);
        }
      }

      SECTION("BlockViewGenerator") {
        blocks::BlockRefGenerator<Plugin> bref(vs, 2UL);
        auto check_view = [&](auto& bref_sl1) {
          CHECK(bref_sl1.parent() == bref);
          CHECK(bref_sl1.uuid() == bref.uuid());
          CHECK(blocks::get<::elastica::tags::NElement>(bref_sl1.get()).at(0) ==
                5);
          CHECK(blocks::get<::elastica::tags::NElement>(
                    blocks::generate_from(bref_sl1))
                    .at(0) == 5);
          CHECK(blocks::get<::elastica::tags::NElement>(bref_sl1.get()).at(1) ==
                6);
          CHECK(blocks::get<::elastica::tags::NElement>(bref_sl1.get()).at(2) ==
                7);
        };
        {
          auto bref_sl1 = blocks::slice(bref, 1, 4);  // 1, 2, 3
          check_view(bref_sl1);
        }
        {
          auto bref_sl1 = bref[{1, 4}];  // 1, 2, 3
          check_view(bref_sl1);
        }
        {
          auto bref_sl1 = blocks::slice(bref, 1, ::elastica::end);  // 1, 2, 3
          check_view(bref_sl1);
          auto another_sl1 = blocks::slice(bref_sl1, 0, 3);
          check_view(another_sl1);
        }
        {
          auto bref_sl1 =
              blocks::slice(bref, 1, ::elastica::end - 1);  // 1, 2, 3
          check_view(bref_sl1);
        }
        {
          auto bref_sl1 = blocks::slice(bref);  // 0, 1, 2, 3
        }
      }

      SECTION("BlockSliceGenerator") {
        blocks::BlockRefGenerator<Plugin> aref(vs, 1UL);
        auto check_slice = [&](auto& aref_sl) {
          // Refs need not be preserved, only copies do
          CHECK(aref_sl.parent() == aref);
          CHECK(aref_sl.uuid() == aref.uuid());
          CHECK(blocks::get<::elastica::tags::NElement>(aref_sl.get()).at(0) ==
                3);
          CHECK(blocks::get<::elastica::tags::NElement>(
                    blocks::generate_from(aref_sl))
                    .at(0) == 3);
        };
        {
          auto aref_sl = blocks::slice(aref, 0);
          check_slice(aref_sl);
        }
        {
          auto aref_sl = blocks::slice(aref, ::elastica::end - 3UL);  // 0UL
          check_slice(aref_sl);
        }
        {
          auto aref_sl = aref[0UL];
          check_slice(aref_sl);
        }
        {
          auto aref_sl = aref[::elastica::end - 3UL];  // 0UL
          check_slice(aref_sl);
        }
        {
          auto aref_view = blocks::slice(aref, 0UL, elastica::end);  // 0ULUL
          auto aref_sl = blocks::slice(aref_view, 0UL);              // 0ULUL
          check_slice(aref_sl);

          auto another_aref_view =
              blocks::slice(aref, 1UL, elastica::end);  // 1UL, 2
          auto another_aref_sl = blocks::slice(another_aref_view, 1UL);  // 0UL
          CHECK(another_aref_sl.index() == 2UL);
        }

        blocks::BlockRefGenerator<Plugin> bref(vs, 2UL);
        auto bref_sl1 = blocks::slice(bref, 1);
        auto bref_sl3 = blocks::slice(bref, 3);

        CHECK(bref_sl1.parent() == bref);
        CHECK(bref_sl3.parent() == bref);

        CHECK(bref_sl1.uuid() == bref_sl3.uuid());
        CHECK(blocks::get<::elastica::tags::NElement>(bref_sl1.get()).at(0) ==
              5);
        CHECK(blocks::get<::elastica::tags::NElement>(bref_sl3.get()).at(0) ==
              7);
      }
    });
  }

  // clang-format off
   ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module",
                      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    EMPTY_TEST();

    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
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

    using ListOfSystems = tmpl::list<ECBlock, ECNamedBlock>;
    static_assert(cpp17::is_same_v<typename Sim::as_blocks<ListOfSystemPlugins>,
                                   ListOfSystems>,
                  "Systems meta-failure!");

    (void)(sim.blocking_policy());
    (void)(cpp17::as_const(sim).blocking_policy());
    (void)(sim.iteration_policy());
    (void)(cpp17::as_const(sim).iteration_policy());
    elastica::ignore_unused(sim);
  }

  void test_emplace_back_by_LooselyRestrictSizeAcrossBlockTypes() {
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(LooselyRestrictSizeAcrossBlockTypes{20UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    // check if push back works in accordance with the blocking policy
    const std::size_t n_elements = 13UL;  // boo!

    tmpl::for_each<ListOfSystemPlugins>([&](auto p) {
      using Plugin = tmpl::type_from<decltype(p)>;
      using Block = GenerateBlock<Plugin>;

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));

      // 1 block is now there with 13 UL elements
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      CHECK(::elastica::size(
                sim.get_system_collection_container<Block>()[0UL]) == 13UL);

      // another
      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));

      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      // no need to check size of each slices here

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      // now the policy has no chocie but to add another block
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            2UL);
    });
  }

  void test_emplace_back_by_RestrictSizeAcrossBlockTypes() {
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    // check if push back works in accordance with the blocking policy
    const std::size_t n_elements = 8UL;

    tmpl::for_each<ListOfSystemPlugins>([&](auto p) {
      using Plugin = tmpl::type_from<decltype(p)>;
      using Block = GenerateBlock<Plugin>;

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));

      // 1 block is now there with 8 UL elements
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      CHECK(
          ::elastica::size(sim.get_system_collection_container<Block>()[0UL]) ==
          n_elements);

      // another
      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));

      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);
      // no need to check size of each slices here

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      // now the policy has no choice but to add another block
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            2UL);
    });
  }

  // needed for validationg the apply_over_blcoks test
  void test_emplace_back_by_LooselyRestrictCustomizedToBlockTypes() {
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    const std::size_t n_elements = 8UL;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        // check if push back works in accordance with the blocking policy
        make_blocking_policy(
            LooselyRestrictSize<ECBlock>{2 * n_elements},
            LooselyRestrictSize<ECNamedBlock>{n_elements - 1UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    {
      using Plugin = ECPlugin;
      using Block = ECBlock;

      for (std::size_t i = 0UL; i < 2UL; ++i) {
        sim.emplace_back<Plugin>(
            ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                blocks::initialize<::elastica::tags::NElement>(
                    [=](...) { return n_elements; })));
        // 1 block is now there with 8 UL elements
        CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
              1UL);
      }

      for (std::size_t i = 0UL; i < 2UL; ++i) {
        sim.emplace_back<Plugin>(
            ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                blocks::initialize<::elastica::tags::NElement>(
                    [=](...) { return n_elements; })));
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
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      // 1 block is now there with 8 UL elements
      CHECK(::elastica::size(sim.get_system_collection_container<Block>()) ==
            1UL);

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
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

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.blocking_policy",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(LooselyRestrictSizeAcrossBlockTypes{20UL}));
    CHECK(sim.blocking_policy() != nullptr);
    SECTION("same policy but different parameter") {
      auto policy_with_higher_threshold =
          make_blocking_policy(LooselyRestrictSizeAcrossBlockTypes{40UL});
      // Don't need to set a registrar here since
      // LooselyRestrictSizeAcrossBlockTypes is in the default list
      sim.blocking_policy() = ::elastica::configuration::virtualize(
          std::move(policy_with_higher_threshold));
    }
    SECTION("different policy") {
      auto one = make_blocking_policy(RestrictSizeAcrossBlockTypes{40UL});
      sim.blocking_policy() =
          ::elastica::configuration::virtualize(std::move(one));

      auto two = make_blocking_policy(AlwaysSameAcrossBlockTypes{40UL});
      sim.blocking_policy() =
          ::elastica::configuration::virtualize(std::move(two));

      auto three = make_blocking_policy(AlwaysNewAcrossBlockTypes{40UL});
      sim.blocking_policy() =
          ::elastica::configuration::virtualize(std::move(three));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.iteration_policy_id",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    {
      auto sim = make_systems_simulator<ListOfSystemPlugins>(
          make_blocking_policy(LooselyRestrictSizeAcrossBlockTypes{20UL}));
      CHECK(sim.iteration_policy_id() != nullptr);

      auto policy_with_lower_threshold = iteration_policy_for_testing(100UL);
      sim.iteration_policy_id() =
          std::make_unique<decltype(policy_with_lower_threshold)>(
              std::move(policy_with_lower_threshold));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.n_systems",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    // I just test the two most complex here as if they work, the rest will
    // also work.

    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(LooselyRestrictSizeAcrossBlockTypes{20UL}));

    CHECK(sim.template get_system_collection_container<ECBlock>().empty());

    // check if push back works in accordance with the blocking policy
    const std::size_t n_elements = 13UL;  // boo!
    std::size_t n_system_count = 0UL;

    tmpl::for_each<ListOfSystemPlugins>([&](auto p) {
      using Plugin = tmpl::type_from<decltype(p)>;

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      CHECK(sim.n_systems() == ++n_system_count);

      // another
      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      CHECK(sim.n_systems() == ++n_system_count);

      sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      CHECK(sim.n_systems() == ++n_system_count);
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.exception_testing_for_emplace_back",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));
    sim.finalize();

    {
      using Plugin = ECPlugin;
      CHECK_THROWS_WITH(
          sim.emplace_back<Plugin>(
              ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                  blocks::initialize<::elastica::tags::NElement>(
                      [](...) { return 200UL; }))),
          Catch::Contains("simulator has been finalized"));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.check_if_registered",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));

    tmpl::for_each<ListOfSystemPlugins>([&](auto p) {
      using Plugin = tmpl::type_from<decltype(p)>;
      using Block = GenerateBlock<Plugin>;
      const std::size_t n_elements = 16UL;

      CHECK(sim.template get_system_collection_container<Block>().empty());

      auto sliced = (sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; }))));
      CHECK(sim.check_if_registered(sliced));

      // This code no longer compiles by design -> only lazy block slice
      // generators are accepted going forth
      // Block block{};
      // auto another_sliced = block.emplace(
      //     ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
      //         blocks::initialize<::elastica::tags::NElement>([=](...){return
      //         n_elements;} )));
      // CHECK_FALSE(sim.check_if_registered(std::ref(another_sliced)));
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.confirm_as_registered",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));

    tmpl::for_each<ListOfSystemPlugins>([&](auto p) {
      using Plugin = tmpl::type_from<decltype(p)>;
      using Block = GenerateBlock<Plugin>;
      const std::size_t n_elements = 16UL;

      CHECK(sim.template get_system_collection_container<Block>().empty());

      auto sliced_ref = (sim.emplace_back<Plugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; }))));
      // Check that the plugin is actually what we want.
      static_assert(cpp17::is_same_v<decltype(sliced_ref),
                                     ::blocks::BlockSliceGenerator<Plugin>>,
                    "Metafailure");
      CHECK(sim.check_if_registered(sliced_ref));
      CHECK_NOTHROW(sim.confirm_as_registered(sliced_ref));

      // This code no longer compiles by design -> only lazy block slice
      // generators are accepted going forth
      // Block block{};
      // auto another_sliced = block.emplace(
      //     ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
      //         blocks::initialize<::elastica::tags::NElement>([=](...){return
      //         n_elements;} )));
      // CHECK_THROWS_WITH(sim.confirm_as_registered(another_sliced),
      //                   Catch::Contains("Input system was not found"));
    });
  }
  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.to_be_iterated_sequentially",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}));
    CHECK(sim.to_be_iterated_sequentially(
        sim.get_system_collection_container<ECBlock>()));
    CHECK(sim.to_be_iterated_sequentially(
        sim.get_system_collection_container<ECNamedBlock>()));
  }

  inline decltype(auto) setup_sim_with_state(const std::size_t n_units) {
    const std::size_t n_elements = 16UL;
    using ListOfSystemPlugins =
        tmpl::list<ECPlugin, ECNamedPlugin, AnotherECPlugin>;
    auto sim = make_systems_simulator<ListOfSystemPlugins>(make_blocking_policy(
        LooselyRestrictSize<ECBlock>{2 * n_elements},
        LooselyRestrictSize<ECNamedBlock>{n_elements - 1UL},
        RestrictSize<AnotherECBlock>{n_elements}));
    // gives n_units / 2 blocks of ECBlock and n_units ECNamedBlock
    tmpl::for_each<ListOfSystemPlugins>([&](auto p) {
      using Plugin = tmpl::type_from<decltype(p)>;
      for (auto i = 0UL; i < n_units; ++i)
        sim.template emplace_back<Plugin>(
            ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                blocks::initialize<::elastica::tags::NElement>(
                    [=](...) { return n_elements; })));
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

    std::size_t ec_seq_block_counter(0UL), ec_named_seq_block_counter(0UL),
        another_ec_seq_block_counter(0UL);
    sim.for_each_block_sequential(make_overloader(
        [&](ECBlock const&) { ++ec_seq_block_counter; },
        [&](ECNamedBlock const&) { ++ec_named_seq_block_counter; },
        [&](AnotherECBlock const&) { ++another_ec_seq_block_counter; }));

    CHECK(ec_seq_block_counter == expected_n_ec_blocks);
    CHECK(ec_named_seq_block_counter == expected_n_ec_named_blocks);
    CHECK(another_ec_seq_block_counter == expected_another_ec_blocks);

    sim.for_each_block_sequential_untimed(make_overloader(
        [&](ECBlock const&) { ++ec_seq_block_counter; },
        [&](ECNamedBlock const&) { ++ec_named_seq_block_counter; },
        [&](AnotherECBlock const&) { ++another_ec_seq_block_counter; }));

    CHECK(ec_seq_block_counter == 2 * expected_n_ec_blocks);
    CHECK(ec_named_seq_block_counter == 2 * expected_n_ec_named_blocks);
    CHECK(another_ec_seq_block_counter == 2 * expected_another_ec_blocks);
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
    auto ec_sliced =
        (sim.emplace_back<ECPlugin>(
             ::elastica::cosserat_rod::initialize_cosserat_rod<ECPlugin>(
                 blocks::initialize<::elastica::tags::NElement>(
                     [=](...) { return n_elements; }))))
            .get();
    auto ec_named_sliced =
        (sim.emplace_back<ECNamedPlugin>(
             ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
                 blocks::initialize<::elastica::tags::NElement>(
                     [=](...) { return n_elements; }))))
            .get();
    auto another_ec_sliced =
        (sim.emplace_back<AnotherECPlugin>(
             ::elastica::cosserat_rod::initialize_cosserat_rod<AnotherECPlugin>(
                 blocks::initialize<::elastica::tags::NElement>(
                     [=](...) { return n_elements; }))))
            .get();
    {
      sim.emplace_back<ECNamedPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      sim.emplace_back<ECNamedPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
      sim.emplace_back<ECNamedPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<ECNamedPlugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));

      sim.emplace_back<AnotherECPlugin>(
          ::elastica::cosserat_rod::initialize_cosserat_rod<AnotherECPlugin>(
              blocks::initialize<::elastica::tags::NElement>(
                  [=](...) { return n_elements; })));
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

    std::size_t ec_slice_seq_counter(0UL), ec_named_slice_seq_counter(0UL),
        another_ec_slice_seq_counter(0UL);
    sim.for_each_system_sequential(make_overloader(
        [&](decltype(ec_sliced) const&) { ++ec_slice_seq_counter; },
        [&](decltype(ec_named_sliced) const&) { ++ec_named_slice_seq_counter; },
        [&](decltype(another_ec_sliced) const&) {
          ++another_ec_slice_seq_counter;
        }));

    CHECK(ec_slice_seq_counter == 5UL);
    CHECK(ec_named_slice_seq_counter == 8UL);
    CHECK(another_ec_slice_seq_counter == 6UL);

    sim.for_each_system_sequential_untimed(make_overloader(
        [&](decltype(ec_sliced) const&) { ++ec_slice_seq_counter; },
        [&](decltype(ec_named_sliced) const&) { ++ec_named_slice_seq_counter; },
        [&](decltype(another_ec_sliced) const&) {
          ++another_ec_slice_seq_counter;
        }));
    CHECK(ec_slice_seq_counter == 5UL * 2UL);
    CHECK(ec_named_slice_seq_counter == 8UL * 2UL);
    CHECK(another_ec_slice_seq_counter == 6UL * 2UL);
  }

  // TODO : test serialization
  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.serialize",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    using namespace elastica::io;
    // NOTHROW case
    {
      // The simulator is setup with a composite blocking policy, which is
      // intended  for testing serialization from a (runtime) dynamic type
      auto sim = setup_sim_with_state(20UL);
      std::stringstream ss;  // any stream can be used
      // std::size_t n_elements(16UL);

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
    }  // nothrow case
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Systems.Module.reset",
                     "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    auto sim = setup_sim_with_state(20UL);
    // pre check
    {
      auto check_size = [&](auto Is) {
        CHECK(std::get<Is>(sim.n_total_systems()) > 0UL);
      };
      auto check_container = [&](auto Is) {
        CHECK_FALSE(
            std::get<Is>(sim.get_system_collection_container()).empty());
      };
      index_apply<3UL>([&](auto... Is) {
        EXPAND_PACK_LEFT_TO_RIGHT(check_size(Is));
        EXPAND_PACK_LEFT_TO_RIGHT(check_container(Is));
      });
    }

    // argument doesnt matter
    CHECK_NOTHROW(sim.reset_module(elastica::Time{0.0}));

    // post
    {
      auto check_size = [&](auto Is) {
        CHECK(std::get<Is>(sim.n_total_systems()) == 0UL);
      };
      auto check_container = [&](auto Is) {
        CHECK(std::get<Is>(sim.get_system_collection_container()).empty());
      };

      index_apply<3UL>([&](auto... Is) {
        EXPAND_PACK_LEFT_TO_RIGHT(check_size(Is));
        EXPAND_PACK_LEFT_TO_RIGHT(check_container(Is));
      });
    }
  }

}  // namespace
