//
// Created by Tejaswin Parthasarathy on 4/20/22.
//

#include <algorithm>
#include <memory>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration.hpp"  // all configuration
#include "Simulator/EnableReset.hpp"  // needed for with_primitives, DEMsolver, primitives
#include "Simulator/Environments.hpp"  // needed for with_primitives, DEMsolver, primitives
#include "Simulator/Materials.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/Sphere.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TypeTraits/IsA.hpp"

// Instead of declaring dummy classes, primitives and transfer here

template <std::size_t I>
struct MySystem {
  int radius[6UL];
  int get_radius(std::size_t idx) const& { return radius[idx]; }
  int& get_radius(std::size_t idx) & { return radius[idx]; }
};

namespace {

  namespace detail {

    // primary template handles types that have no nested ::ForceModel
    // member:
    template <class, class = cpp17::void_t<>>
    struct HasValidForceModelMember : std::false_type {};

    // specialization recognizes types that do have a nested ::ForceModel
    // member:
    template <class T>
    struct HasValidForceModelMember<T, cpp17::void_t<typename T::ForceModel>>
        : std::true_type {};

  }  // namespace detail

  template <class T>
  constexpr bool has_valid_force_model_member_v =
      detail::HasValidForceModelMember<cpp20::remove_cvref_t<T>>::value;

  template <typename Traits>
  struct MockField : public elastica::Singleton<MockField<Traits>>
  /*: public ::tt::ConformsTo<::elastica::protocols::Environments>*/ {
   public:
    template <typename Simulator>
    void resolve(Simulator const&, typename Traits::TimeType) {}
    template <typename Simulation>
    void initialize(Simulation const&) {}
  };

  // overloaded functions
  template <typename Traits>
  inline void reset(MockField<Traits>&, typename Traits::TimeType) noexcept {};
  template <typename Traits, typename CorrespondingEnvironmentConfiguration>
  inline void initialize_parameters(
      MockField<Traits>&,
      CorrespondingEnvironmentConfiguration const&) noexcept {};

  struct MockFieldTag {
    using requires_primitives = std::false_type;

    template <typename Traits>
    using type = MockField<Traits>;
  };

  template <typename EnvironmentTags = tmpl::list<MockFieldTag>>
  auto make_configuration() {
    using namespace ::elastica::configuration;
    using ImposedPrimitives =
        tmpl::list<::elastica::collision::ImposedPrimitiveAdapter<
            ::elastica::collision::SpherePrimitive>>;
    using Systems = tmpl::list<::elastica::rigid_body::Sphere,
                               ::elastica::cosserat_rod::CosseratRod>;
    auto collision_config = make_collision_configuration(seq, seq);

    static_assert(has_valid_force_model_member_v<decltype(collision_config)>,
                  "Meta-failure");

    auto env_config =
        make_environments_configuration<EnvironmentTags, ImposedPrimitives>(
            seq, collision_config);

    static_assert(has_valid_force_model_member_v<decltype(env_config)>,
                  "Meta-failure");

    auto config = make_simulator_configuration(
        DefaultDomainConfiguration{},
        make_systems_configuration<Systems>(DefaultBlockingPolicy{},
                                            DefaultIterationPolicy{}),
        DefaultParallelConfiguration{}, env_config);

    static_assert(has_valid_force_model_member_v<decltype(config)>,
                  "Meta-failure");

    using C = decltype(config);
    struct Configuration : public C {
      Configuration(C c) : C(std::move(c)) {}
      // for non-integrated test
      using AdmissiblePrimitives =
          tmpl::list<::elastica::collision::SpherePrimitive,
                     ::elastica::collision::CapsuleFromRodPrimitive>;
    };

    static_assert(has_valid_force_model_member_v<Configuration>,
                  "Meta-failure");

    return Configuration{std::move(config)};
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Environments,
                                    ::elastica::modules::EnableReset>(
        make_configuration<>());
  }

  template <typename S>
  auto setup_state(S& simulator) {
    // This is a copy paste of the previous
    using ::elastica::collision::CapsuleFromRodPrimitive;
    using ::elastica::collision::SpherePrimitive;
    using ImposedSphere = ::elastica::collision::ImposedPrimitiveAdapter<
        ::elastica::collision::SpherePrimitive>;

    ::elastica::Vec3 first_position{0.0, 0.0, 0.0};
    ::elastica::real_t first_radius{0.3};

    std::for_each(
        elastica::make_counting_iterator(0UL),
        elastica::make_counting_iterator(20UL), [&](std::size_t i) {
          // there is always some contact
          ::elastica::Vec3 pos =
              first_position +
              // This incurs a collision with the spheres added later on
              ::elastica::Vec3{static_cast<::elastica::real_t>(0.6 + i), 0.,
                               0.};
          auto ref = simulator->template emplace_back_imposed<ImposedSphere>(
              i, 0UL, false, pos, first_radius, ::elastica::iron);
          // TODO : need proper initialization syntax for imposed
          // primitives
          ref.get().transfer(first_radius, std::move(pos),
                             ::elastica::identity3(), ::elastica::Vec3{},
                             ::elastica::Vec3{});
        });

    using Sys = ::elastica::rigid_body::Sphere;
    auto initializer = ::blocks::initialize_block<Sys>(
        ::blocks::initialize<::elastica::tags::Position>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::Director>(
            []() { return ::elastica::identity3(); }),
        ::blocks::initialize<::elastica::tags::Radius>([]() { return 0.3; }),
        ::blocks::initialize<::elastica::tags::Velocity>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::AngularVelocity>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::Acceleration>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::AngularAcceleration>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::Material>(
            []() { return ::elastica::iron; }));
    // First sphere intersects one imposed sphere
    auto sphere_ref =
        simulator->template emplace_back<Sys>(std::move(initializer));

    // Second sphere intersects first sphere
    auto another_initializer = ::blocks::initialize_block<Sys>(
        ::blocks::initialize<::elastica::tags::Position>([]() {
          return ::elastica::Vec3{-0.4, 0.0, 0.0};
        }),
        ::blocks::initialize<::elastica::tags::Director>(
            []() { return ::elastica::identity3(); }),
        ::blocks::initialize<::elastica::tags::Radius>([]() { return 0.2; }),
        ::blocks::initialize<::elastica::tags::Velocity>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::AngularVelocity>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::Acceleration>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::AngularAcceleration>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::Material>(
            []() { return ::elastica::iron; }));
    auto another_sphere_ref =
        simulator->template emplace_back<Sys>(std::move(another_initializer));

    // Add some cosserat rods here

    // 1. Add cosserat rod that intersect second sphere alone
    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{::elastica::copper},
        Init::Radius{0.01},  // 0.01 : aspect ratio of 4
        // three elements of CosseratRod will intersect sphere
        Init::Length{0.2},  // 0.04 each length
        // slightly above 0.1 to ensure 4th element does not touch it
        Init::Origin{::elastica::Vec3{-0.4, 0.11, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},  // straight up
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingRate{0.2},
        Init::TorqueDampingRate{0.2},
    };

    using ::elastica::cosserat_rod::CosseratRod;
    auto rod_ref = simulator->template emplace_back<CosseratRod>(
        straight_initializer.initialize<CosseratRod>());

    // add another cosserat rod touching the previous cosserat rod at the tip
    // for creating a CR-CR contact
    using ::elastica::cosserat_rod::CosseratRod;
    auto another_rod_ref = simulator->template emplace_back<CosseratRod>(
        straight_initializer
            .but_with(Init::Origin{::elastica::Vec3{-0.41, 0.31, 0.0}},
                      Init::Direction{::elastica::Vec3{-1.0, 0.0, 0.0}})
            .initialize<CosseratRod>());

    // add another cosserat rod touching an imposed sphere rod at the tip
    // for creating a CR-IS contact
    auto yet_another_rod_ref = simulator->template emplace_back<CosseratRod>(
        straight_initializer
            .but_with(Init::Origin{::elastica::Vec3{1.6, 0.2, 0.0}},
                      Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}})
            .initialize<CosseratRod>());

    return std::make_tuple(sphere_ref, another_sphere_ref, rod_ref,
                           another_rod_ref, yet_another_rod_ref);
  }

  template <typename CollisionSolverPtr>
  void pre_collision_check(CollisionSolverPtr& solver) {
    using ::elastica::collision::CapsuleFromRodPrimitive;
    using ::elastica::collision::SpherePrimitive;
    using ImposedSphere = ::elastica::collision::ImposedPrimitiveAdapter<
        ::elastica::collision::SpherePrimitive>;

    auto& ss_cache = solver->contact_cache()
                         .template data<SpherePrimitive, SpherePrimitive>();
    CHECK(ss_cache.empty());

    auto& sis_cache =
        solver->contact_cache().template data<SpherePrimitive, ImposedSphere>();
    // need a better API
    CHECK(sis_cache.empty());

    auto& scr_cache =
        solver->contact_cache()
            .template data<SpherePrimitive, CapsuleFromRodPrimitive>();
    CHECK(scr_cache.empty());

    auto& crcr_cache =
        solver->contact_cache()
            .template data<CapsuleFromRodPrimitive, CapsuleFromRodPrimitive>();
    CHECK(crcr_cache.empty());

    auto& cris_cache =
        solver->contact_cache()
            .template data<CapsuleFromRodPrimitive, ImposedSphere>();
    CHECK(cris_cache.empty());

    auto& contacts = solver->contacts();
    CHECK(contacts.empty());

    auto& batches = solver->batches();
    CHECK(batches.empty());
  }

  template <typename Refs>
  void pre_collision_check_states(Refs const& refs) {
    auto sphere_ref = std::get<0UL>(refs);
    auto another_sphere_ref = std::get<1UL>(refs);
    auto rod_ref = std::get<2UL>(refs);
    auto another_rod_ref = std::get<3UL>(refs);
    auto yet_another_rod_ref = std::get<4UL>(refs);

    CHECK(sphere_ref.get().get_external_loads(0UL) == ::elastica::Vec3{});
    CHECK(sphere_ref.get().get_external_torques(0UL) == ::elastica::Vec3{});

    CHECK(another_sphere_ref.get().get_external_loads(0UL) ==
          ::elastica::Vec3{});
    CHECK(another_sphere_ref.get().get_external_torques(0UL) ==
          ::elastica::Vec3{});

    CHECK(rod_ref.get().get_external_loads() == 0.0);
    CHECK(rod_ref.get().get_external_torques() == 0.0);

    CHECK(another_rod_ref.get().get_external_loads() == 0.0);
    CHECK(another_rod_ref.get().get_external_torques() == 0.0);

    CHECK(yet_another_rod_ref.get().get_external_loads() == 0.0);
    CHECK(yet_another_rod_ref.get().get_external_torques() == 0.0);
  }

  template <typename CollisionSolverPtr>
  void post_collision_check(CollisionSolverPtr& solver) {
    using ::elastica::collision::CapsuleFromRodPrimitive;
    using ::elastica::collision::SpherePrimitive;
    using ImposedSphere = ::elastica::collision::ImposedPrimitiveAdapter<
        ::elastica::collision::SpherePrimitive>;

    auto& ss_cache = solver->contact_cache()
                         .template data<SpherePrimitive, SpherePrimitive>();
    auto& sis_cache =
        solver->contact_cache().template data<SpherePrimitive, ImposedSphere>();
    auto& scr_cache =
        solver->contact_cache()
            .template data<SpherePrimitive, CapsuleFromRodPrimitive>();
    auto& crcr_cache =
        solver->contact_cache()
            .template data<CapsuleFromRodPrimitive, CapsuleFromRodPrimitive>();
    auto& cris_cache =
        solver->contact_cache()
            .template data<CapsuleFromRodPrimitive, ImposedSphere>();
    auto& contacts = solver->contacts();
    auto& batches = solver->batches();

    // check if sphere-imposed sphere is registered in potential contacts
    // using elastica::collision::get_collection;
    // get_collection<>
    CHECK_FALSE(ss_cache.empty());
    CHECK(::elastica::size(ss_cache) == 1UL);

    CHECK_FALSE(sis_cache.empty());
    CHECK(::elastica::size(sis_cache) == 1UL);

    CHECK_FALSE(scr_cache.empty());
    CHECK(::elastica::size(scr_cache) == 3UL);

    // still O(n)
    CHECK_FALSE(crcr_cache.empty());
    CHECK(::elastica::size(crcr_cache) == 4UL + 4UL + 4UL + 1UL);

    CHECK_FALSE(cris_cache.empty());
    CHECK(::elastica::size(cris_cache) == 3UL);

    CHECK_FALSE(contacts.empty());

    std::array<std::size_t, 5UL> n_expected_contacts{0UL};
    constexpr std::size_t ss = 0UL;
    constexpr std::size_t scr = 1UL;
    constexpr std::size_t crcr = 2UL;
    constexpr std::size_t sis = 3UL;
    constexpr std::size_t cris = 4UL;

    std::get<ss>(n_expected_contacts) = 1UL;
    std::get<scr>(n_expected_contacts) = 3UL;
    std::get<crcr>(n_expected_contacts) = 1UL;
    std::get<sis>(n_expected_contacts) = 1UL;
    std::get<cris>(n_expected_contacts) = 3UL;

    const auto total_n_expected_contacts = std::accumulate(
        std::cbegin(n_expected_contacts), std::cend(n_expected_contacts), 0UL);
    CHECK(::elastica::size(contacts) == total_n_expected_contacts);

    CHECK_FALSE(batches.empty());
    if (tt::is_a<::elastica::collision::batches::UnionFind,
                 typename CollisionSolverPtr::element_type::
                     ContactBatchingStep::type::BatchingAlgorithm>{}) {
      CHECK(::elastica::size(batches) == 3UL);
      CHECK(::elastica::size(batches.at(0UL)) ==
            total_n_expected_contacts - std::get<crcr>(n_expected_contacts) -
                std::get<cris>(n_expected_contacts));
      // second batch only contains the CR-CR connection
      CHECK(::elastica::size(batches.at(1UL)) ==
            std::get<crcr>(n_expected_contacts));
      CHECK(::elastica::size(batches.at(2UL)) ==
            std::get<cris>(n_expected_contacts));
      CHECK(&batches.at(0UL).at(0UL).get() == &contacts.at(0UL));
    } else {
      CHECK(::elastica::size(batches) == 1UL);
      CHECK(::elastica::size(batches.at(0UL)) == total_n_expected_contacts);
      CHECK(&batches.at(0UL).at(0UL).get() == &contacts.at(0UL));
    }
  }

  template <typename Refs>
  void post_collision_check_states(Refs const& refs) {
    auto sphere_ref = std::get<0UL>(refs);
    auto another_sphere_ref = std::get<1UL>(refs);
    auto rod_ref = std::get<2UL>(refs);
    auto another_rod_ref = std::get<3UL>(refs);
    auto yet_another_rod_ref = std::get<4UL>(refs);

    // some value is there
    CHECK_FALSE(sphere_ref.get().get_external_loads(0UL) == ::elastica::Vec3{});
    // Sphere has normal penetration, so no torques are generated
    // CHECK_FALSE(sphere_ref.get().get_external_torques(0UL) ==
    //             ::elastica::Vec3{});

    CHECK_FALSE(another_sphere_ref.get().get_external_loads(0UL) ==
                ::elastica::Vec3{});

    // element distributes to nodes, so last node will also get 1
    for (auto idx = 0UL; idx < (3UL + 1UL); ++idx) {
      CHECK(rod_ref.get().get_external_loads(idx)[0] == approx(0.0));
      CHECK_FALSE(rod_ref.get().get_external_loads(idx)[1] == approx(0.0));
      CHECK(rod_ref.get().get_external_loads(idx)[2] == approx(0.0));
    }
    for (auto idx = 4UL; idx < (5UL + 1UL); ++idx) {
      CAPTURE(idx);
      // it only has x contact
      CHECK_FALSE(rod_ref.get().get_external_loads(idx)[0] == approx(0.0));
      CHECK(rod_ref.get().get_external_loads(idx)[1] == approx(0.0));
      CHECK(rod_ref.get().get_external_loads(idx)[2] == approx(0.0));
    }
    // loads is on nodes so go till n + 1

    for (auto idx = 0UL; idx < (1UL + 1UL); ++idx) {
      CAPTURE(idx);
      // it only has x contact
      CHECK_FALSE(another_rod_ref.get().get_external_loads(idx)[0] ==
                  approx(0.0));
      CHECK(another_rod_ref.get().get_external_loads(idx)[1] == approx(0.0));
      CHECK(another_rod_ref.get().get_external_loads(idx)[2] == approx(0.0));
    }

    for (auto idx = 2UL; idx < (5UL + 1UL); ++idx) {
      CHECK(another_rod_ref.get().get_external_loads(idx) ==
            ::elastica::Vec3{});
    }

    // CHECK(another_rod_ref.get().get_external_loads(0UL) ==
    // ::elastica::Vec3{});
  }

  template <typename C, typename S>
  class TestSolver : public ::elastica::collision::CollisionSystem<C, S> {
   protected:
    using P = ::elastica::collision::CollisionSystem<C, S>;
    using This = TestSolver<C, S>;
    using T = std::shared_ptr<This>;

   public:
    using P::detection_is_to_be_performed;
    using P::time_of_last_detection_call_;
    using typename P::TimeType;

    static T instance() {
      static T object(std::static_pointer_cast<This>(P::instance()));
      return object;
    }

    auto time_of_last_detection() & noexcept -> TimeType& {
      return time_of_last_detection_call_;
    }
  };

  ELASTICA_TEST_CASE("Unit.Environments.CollisionSystem.DEMSolver",
                     "[Unit][Environments]") {
    auto config = make_configuration();
    using Solver =
        TestSolver<decltype(config), ::elastica::collision::CollisionConfig>;
    static_assert(::tt::assert_conforms_to<
                      Solver, ::elastica::environments::protocols::Environment>,
                  "Meta-failure");

    auto solver = Solver::instance();
    auto simulator = make_simulator();
    // needs to be done for maps etc to be updated
    simulator->reset();

    SECTION("Test interface") {
      SECTION("Free API") {
        using ::elastica::configuration::make_collision_configuration;
        using ::elastica::environments::initialize_parameters;
        using ::elastica::environments::reset;

        using Model = ::elastica::collision::response::LinearSpringDashpot;
        auto config = make_collision_configuration(
            ::elastica::configuration::seq, ::elastica::configuration::seq,
            Model{});
        initialize_parameters(*solver, config);
        // CHECK(cpp17::as_const(solver)->get_response_solver().force_model() ==
        // my_gravity);
        reset(*solver, ::elastica::Time{0.0});
      }
      SECTION("Member API") {
        auto check_member_API = [](auto& sol) {
          (void)sol->get_algorithms();
          (void)sol->get_coarse_detector();
          (void)sol->get_fine_detector();
          (void)sol->get_batcher();
          (void)sol->get_contact_resolver();
          (void)sol->contacts();
          (void)sol->batches();
          (void)sol->contact_cache();
          (void)sol->timers();
          (void)sol->memory_meters();
          (void)sol->time_interval_between_detection();
          (void)sol->clear();
          (void)sol->reset(::elastica::Time{0.0});
        };
        check_member_API(solver);
        check_member_API(cpp17::as_const(solver));
      }
    }

    SECTION("Test Utility") {
      auto refs = setup_state(simulator);

      auto sphere_ref = std::get<0UL>(refs);
      auto another_sphere_ref = std::get<1UL>(refs);
      auto rod_ref = std::get<2UL>(refs);
      auto another_rod_ref = std::get<3UL>(refs);
      auto yet_another_rod_ref = std::get<4UL>(refs);

      simulator->finalize();
      // because the environments do not need support for primitives, they are
      // not initialized, so we initialize them
      if (simulator->primitives_are_required()) {
        // it takes care of populating it
      } else {
        simulator->populate_primitives();
      }

      solver->initialize(*simulator);

      pre_collision_check(solver);
      pre_collision_check_states(refs);
      solver->resolve(*simulator, decltype(config)::TimeType{0.0});
      post_collision_check(solver);
      post_collision_check_states(refs);
    }

    SECTION("Coarse detection skipping") {
      {
        // needs to be ordered top to bottom because time subtraction cant occur
        // from earlier times
        CHECK(solver->detection_is_to_be_performed(::elastica::Time{0.0}));
        CHECK(solver->detection_is_to_be_performed(::elastica::Time{1.234567}));
        CHECK(solver->detection_is_to_be_performed(::elastica::Time{100.0}));
      }
      {
        // reset it to default state
        solver->time_of_last_detection() =
            ::elastica::Time{-2.0};  // > 1.0 to trigger a detection
        solver->time_interval_between_detection() = 1.0;
        // time_interval_between_detection() < 0 - (-2.0), so should trigger
        CHECK(solver->detection_is_to_be_performed(::elastica::Time{0.0}));
        // last tim is now 0.0, interval is 1.0, so should not trigger
        CHECK_FALSE(
            solver->detection_is_to_be_performed(::elastica::Time{0.5}));
        // last time is 0.0, interval is 1.0, so should trigger
        CHECK(solver->detection_is_to_be_performed(
            ::elastica::Time{1.0 + 1e-10}));
      }
    }
  }

}  // namespace

namespace {

  template <typename EnvironmentTags =
                tmpl::list<elastica::environments::tags::CollisionSystemTag>>
  static inline auto make_integrated_configuration() {
    using namespace ::elastica::configuration;
    using ImposedPrimitives =
        tmpl::list<::elastica::collision::ImposedPrimitiveAdapter<
            ::elastica::collision::SpherePrimitive>>;
    using Systems = tmpl::list<::elastica::rigid_body::Sphere,
                               ::elastica::cosserat_rod::CosseratRod>;
    auto collision_config = make_collision_configuration(seq, seq);

    auto env_config =
        make_environments_configuration<EnvironmentTags, ImposedPrimitives>(
            seq, collision_config);

    auto config = make_simulator_configuration(
        DefaultDomainConfiguration{},
        make_systems_configuration<Systems>(DefaultBlockingPolicy{},
                                            DefaultIterationPolicy{}),
        DefaultParallelConfiguration{}, env_config);

    return config;
  }

  static inline auto make_integrated_simulator() {
    return elastica::make_simulator<::elastica::modules::Environments,
                                    ::elastica::modules::EnableReset>(
        make_integrated_configuration<>());
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.Environments.CollisionSystem.DEMSolver",
                     "[Integrated][Environments]") {
    auto simulator = make_integrated_simulator();
    simulator->reset();
    auto refs = setup_state(simulator);

    auto& solver = simulator->get_environment<
        elastica::environments::tags::CollisionSystemTag>();

    // restore solver to base state was before, so that the integrated tests
    // won't buckle
    solver->reset(::elastica::Time{});

    simulator->finalize();
    // primitives are filled up

    SECTION("Check DEMsolver") {
      SECTION("Check first instance of coarse detection always works") {
        pre_collision_check(solver);
        pre_collision_check_states(refs);
        solver->time_interval_between_detection() = 1.0;  // anything > 0.0
        simulator->apply_environmental_effects(::elastica::Time{0.0});
        post_collision_check(solver);
        post_collision_check_states(refs);

        // needs the setup from above
        SECTION("Check skip of coarse detection, after first skip") {
          // reset quantities
          solver->clear();
          pre_collision_check(solver);
          // < 1.0
          simulator->apply_environmental_effects(::elastica::Time{0.5});
          // state should not have changed here
          pre_collision_check(solver);
        }

        // needs the setup from above
        SECTION("Check no-skip of coarse detection for long intervals") {
          // reset quantities
          solver->clear();
          pre_collision_check(solver);
          // > 1.0
          simulator->apply_environmental_effects(::elastica::Time{1.5});
          // changes states
          post_collision_check(solver);
        }
      }
    }
  }

}  // namespace
