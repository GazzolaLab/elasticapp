//
// Created by Tejaswin Parthasarathy on 4/18/22.
//
#include <atomic>
#include <tuple>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Interface/Environments/MockEnvironments.hpp"
#include "Simulator/Interface/Environments/WithPrimitives.hpp"
#include "Simulator/Interface/MockSimulator.hpp"
#include "Simulator/Interface/Systems/Mocks.hpp"
#include "Simulator/Interface/Systems/Module.hpp"
#include "Simulator/Utilities/Timers.hpp"
//
#include "Utilities/CRTP.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/RepeatAsTuple.hpp"
/// serializatoin
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/Environments/Serialize.hpp"  // for serialize
#include "Simulator/Interface/Systems/Serialize.hpp"       // for serialize
#include "Systems/CosseratRods/Serialize.hpp"  // because you decided to put cosserat rods in
// cosserat
#include "Systems/CosseratRods/CosseratRods.hpp"
#include "Systems/CosseratRods/Mocks/CosseratRodTraits.hpp"

//.......................................... copied from module test
namespace {

  //////// The following code block is copy pasted from another source
  template <typename Traits, typename Block>
  struct AlwaysNeededComponent
      : public ::elastica::CRTPHelper<Block, AlwaysNeededComponent> {
    using CRTP = ::elastica::CRTPHelper<Block, AlwaysNeededComponent>;
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
    struct Material
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::Material,        //
              typename Traits::DataType::Index,  // actually should be a
                                                 // different type, but whatever
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

    inline constexpr decltype(auto) get_material() & noexcept {
      return blocks::get<::elastica::tags::Material>(self());
    }
    inline constexpr decltype(auto) get_material() const& noexcept {
      return blocks::get<::elastica::tags::Material>(self());
    }

    inline constexpr decltype(auto) get_material(std::size_t idx) & noexcept {
      return Material::slice(get_material(), idx);
    }
    inline constexpr decltype(auto) get_material(
        std::size_t idx) const& noexcept {
      return Material::slice(get_material(), idx);
    }

    using ComputedVariables =
        tmpl::list<Position, Director, Velocity, AngularVelocity, Acceleration,
                   AngularAcceleration, Material>;
  };
  ////////

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
//.......................................... copied from module test

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
  class WithPrimitivesSimulator
      : public ::elastica::modules::Systems<Traits,
                                            WithPrimitivesSimulator<Traits>>,
        public ::elastica::interface::environments::WithPrimitives<
            Traits, WithPrimitivesSimulator<Traits>> {
   public:
    using P =
        elastica::modules::Systems<Traits, WithPrimitivesSimulator<Traits>>;
    using Q = ::elastica::interface::environments::WithPrimitives<
        Traits, WithPrimitivesSimulator<Traits>>;

    ////////////  This piece of code is copy-pasted yo
    //! Get all algorithm steps
    using This = WithPrimitivesSimulator<Traits>;
    using AlgorithmSteps =
        tmpl::append<typename P::UpdateSteps, typename Q::UpdateSteps>;
    //! Algorithm cache type
    using Algorithms = tmpl::as_tagged_tuple<AlgorithmSteps>;
    static constexpr std::size_t n_algorithms =
        tmpl::size<AlgorithmSteps>::value;

    //! Cache for timers for profiling the code
    using Timers = ::elastica::Timers<AlgorithmSteps>;
    ////////////  This piece of code is copy-pasted yo

    using T = Traits;
    using TimeType = typename T::TimeType;

    WithPrimitivesSimulator(Traits traits, bool arg_is_final = false)
        : P(),
          ////////////  This piece of code is copy-pasted yo
          algorithms(std::apply(
              [](auto&&... initializer_as_a_tuple) noexcept {
                return Algorithms(
                    std::forward<decltype(initializer_as_a_tuple)>(
                        initializer_as_a_tuple)...);
              },
              repeat_as_tuple<n_algorithms>(
                  // make_tuple doesn't work?
                  std::tuple<This*, Traits const&>(this, traits)))),
          ////////////  This piece of code is copy-pasted yo
          is_final(arg_is_final) {
      P::blocking_policy() = ::elastica::configuration::virtualize(
          std::move(traits).blocking_policy());
      P::iteration_policy_id() =
          std::make_unique<typename Traits::IterationPolicy>(
              std::move(traits).iteration_policy());
    }
    ~WithPrimitivesSimulator() = default;
    WithPrimitivesSimulator(WithPrimitivesSimulator&&) noexcept = default;

    using typename Q::AdmissiblePrimitives;
    // before
    using P::check_if_admissible;
    using typename P::SystemCollections;

    void finalize(TimeType t = {}) {
      Q::finalize(t);
      is_final = true;
    }

    using Q::offsets;
    using Q::primitives;
    using Q::reset_module;

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
    Timers timers_;
  };

}  // namespace mock

namespace elastica {

  namespace customization {

    template <>
    struct PrimitiveTrait<ECBlock> {
      using type = ::elastica::collision::SpherePrimitive;
    };
    template <>
    struct PrimitiveTrait<ECNamedBlock> {
      using type = ::elastica::collision::CapsulePrimitive;
    };
    template <>
    struct PrimitiveTrait<AnotherECBlock> {
      using type = ::elastica::collision::CapsulePrimitive;
    };

  }  // namespace customization

}  // namespace elastica

namespace elastica {

  namespace io {

    template <class Traits>
    struct Serialize<::mock::WithPrimitivesSimulator<Traits>> {
      using type = ::mock::WithPrimitivesSimulator<Traits>;
      using actual_type = typename type::P;
      using actual_another_type = typename type::Q;

      // 1. check if blocking is the same, else abort
      template <typename InArchive>
      static inline void load(InArchive& ar, type& module) {
        ar(static_cast<actual_type&>(module));
        ar(static_cast<actual_another_type&>(module));
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& module) {
        ar(static_cast<actual_type const&>(module));
        ar(static_cast<actual_another_type const&>(module));
      }
    };

  }  // namespace io

}  // namespace elastica

namespace {

  template <typename ListOfSystemPlugins>
  inline decltype(auto) make_primitives_simulator() {
    using namespace ::elastica::configuration;
    using ListOfImposedPrimitives = tmpl::list<>;

    auto env_config =
        ::mock::make_environments_configuration<ListOfImposedPrimitives>(
            elastica::configuration::seq);
    auto sys_config = ::mock::make_systems_configuration<ListOfSystemPlugins>(
        // this does not matter
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
        make_iteration_policy(iterate_across_system_types_in(seq),
                              // for testing iteration policy
                              iterate_over_systems_in(hybrid),
                              AllSerialBelow(500UL)));
    auto config(mock::make_simulator_configuration(std::move(sys_config),
                                                   std::move(env_config)));
    //
    using Config = std::decay_t<decltype(config)>;
    using Sim = ::mock::WithPrimitivesSimulator<Config>;
    return Sim(std::move(config));
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environments.WithPrimitives.Module",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    EMPTY_TEST();

    using ListOfSystemPlugins = tmpl::list<ECPlugin, ECNamedPlugin>;
    auto sim = make_primitives_simulator<ListOfSystemPlugins>();
    using Sim = decltype(sim);
    static_assert(Sim::get_number_of_system_collections() == 2,
                  "Systems meta-failure!");
    static_assert(
        std::is_same_v<
            typename Sim::SystemCollections,
            std::tuple<std::vector<ECBlock>, std::vector<ECNamedBlock>>>,
        "Systems meta-failure!");
    // By default, it compiles with gravity
    static_assert(not Sim::primitives_are_required(), "Meta-failure");

    (void)(sim.primitives());
    (void)(sim.offsets());
    (void)(std::as_const(sim).primitives());
    (void)(std::as_const(sim).offsets());
    elastica::ignore_unused(sim);
  }

  inline decltype(auto) setup_sim_with_state(const std::size_t n_units) {
    const std::size_t n_elements = 16UL;
    using ListOfSystemPlugins =
        tmpl::list<ECPlugin, ECNamedPlugin, AnotherECPlugin>;

    auto sim = make_primitives_simulator<ListOfSystemPlugins>();
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
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environments.WithPrimitives.finalize",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    const std::size_t n_units = 4UL;
    auto sim = setup_sim_with_state(n_units);

    sim.finalize();
    if (sim.primitives_are_required()) {
      // it takes care of populating it
    } else {
      sim.populate_primitives();
    }

    CHECK(sim.template n_primitives<::elastica::collision::SpherePrimitive>() ==
          64UL);
    CHECK(
        sim.template n_primitives<::elastica::collision::CapsulePrimitive>() ==
        128UL);

    // check one primitive just to be safe
    using elastica::collision::get_collection;
    auto& capsule_collection =
        get_collection<elastica::collision::CapsulePrimitive>(sim.primitives());

    {
      auto& capsule = capsule_collection.at(5UL);
      CHECK(capsule.getSystemID() == 5UL);
      CHECK(capsule.get_material() == 0);
    }

    {
      auto& capsule = capsule_collection.at(100UL);
      CHECK(capsule.getSystemID() == 100UL);
      CHECK(capsule.get_material() == 0);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environments.WithPrimitives.for_each",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    const std::size_t n_units = 4UL;
    auto sim = setup_sim_with_state(n_units);

    sim.finalize();
    if (sim.primitives_are_required()) {
      // it takes care of populating it
    } else {
      sim.populate_primitives();
    }

    CHECK(sim.template n_primitives<::elastica::collision::SpherePrimitive>() ==
          64UL);
    CHECK(
        sim.template n_primitives<::elastica::collision::CapsulePrimitive>() ==
        128UL);

    SECTION("over all primitives") {
      using Counter = std::atomic<std::size_t>;
      std::array<Counter, 2UL> counters;
      std::fill(std::begin(counters), std::end(counters), 0UL);

      std::array<Counter, 2UL> expected_counters;
      expected_counters[0UL].store(
          sim.template n_primitives<::elastica::collision::SpherePrimitive>());
      expected_counters[1UL].store(
          sim.template n_primitives<::elastica::collision::CapsulePrimitive>());

      auto func = index_apply<2UL>([&](auto... Is) {
        // using P = tmpl::at_c<typename
        // decltype(sim)::ListOfAdmissibleImposedPrimitives, Is>;
        return make_overloader(
            [&](tmpl::at_c<tmpl::list<::elastica::collision::SpherePrimitive,
                                      ::elastica::collision::CapsulePrimitive>,
                           Is> const&) { ++counters[Is]; }...);
      });

      sim.for_each_primitive(func);

      CHECK(counters == expected_counters);
    }
    SECTION("over subset of primitives") {
      using Counter = std::atomic<std::size_t>;
      Counter counter(0UL);
      Counter expected_counter(
          sim.template n_primitives<::elastica::collision::SpherePrimitive>());

      sim.template for_each_primitive<
          tmpl::list<::elastica::collision::SpherePrimitive>>(
          [&](auto const&) { ++counter; });
      CHECK(counter == expected_counter);

      expected_counter +=
          sim.template n_primitives<::elastica::collision::CapsulePrimitive>();

      sim.template for_each_primitive<
          tmpl::list<::elastica::collision::CapsulePrimitive>>(
          [&](auto const&) { ++counter; });
      CHECK(counter == expected_counter);
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Environments.WithPrimitives.serialize",
      "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    using namespace elastica::io;
    // NOTHROW case
    {
      auto sim = setup_sim_with_state(4UL);
      std::stringstream ss;  // any stream can be used
      // std::size_t n_elements(16UL);

      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive
        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        // from sim
        auto another_sim = setup_sim_with_state(4UL);
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        CHECK_NOTHROW(another_sim.finalize());
        CHECK_NOTHROW(iarchive(another_sim));
      }
    }  // nothrow case
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environments.WithPrimitives.reset",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    auto sim = setup_sim_with_state(20UL);
    sim.finalize();
    if (sim.primitives_are_required()) {
      // it takes care of populating it
    } else {
      sim.populate_primitives();
    }

    // pre check
    {
      auto check_size = [](auto& container) { CHECK_FALSE(container.empty()); };
      // auto check_offset = [](std::size_t value) { CHECK_FALSE(value == 0); };

      std::apply(
          [=](auto&... containers) {
            EXPAND_PACK_LEFT_TO_RIGHT(check_size(containers));
          },
          sim.primitives().data());
      tuples::apply(
          [=](auto... values) {
            EXPAND_PACK_LEFT_TO_RIGHT(check_size(values));
          },
          sim.offsets().data());
    }

    // argument doesnt matter
    CHECK_NOTHROW(sim.reset_module(elastica::Time{0.0}));

    // post
    {
      auto check_size = [](auto& container) { CHECK(container.empty()); };
      // auto check_offset = [](std::size_t value) { CHECK(value == 0); };

      std::apply(
          [=](auto&... containers) {
            EXPAND_PACK_LEFT_TO_RIGHT(check_size(containers));
          },
          sim.primitives().data());
      tuples::apply(
          [=](auto... values) {
            EXPAND_PACK_LEFT_TO_RIGHT(check_size(values));
          },
          sim.offsets().data());
    }
  }

}  // namespace