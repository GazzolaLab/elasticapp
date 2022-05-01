
#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Transfer/Transfer.hpp"

/*
 * Integrated test masequerading as unit test
 * needs the emplacement test to pass
 */

//
#include "Simulator/EnableReset.hpp"
#include "Simulator/Interface/Environments.hpp"
#include "Simulator/Simulator.hpp"
//
#include "Simulator/Configuration.hpp"
//
#include "Systems/Systems.hpp"
//
#include "Simulator/Environments/Transfer/Transferer.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  // copied from test_emplacer
  template <typename Block>
  using GetPluginFromBlock = typename Block::PluginType;

  template <typename ListOfPlugins, typename T, typename U>
  inline auto make_configuration(T across, U over) {
    using namespace ::elastica::configuration;
    using ListOfImposedPrimitives = tmpl::list<>;

    auto env_config =
        make_environments_configuration<ListOfImposedPrimitives>(seq);

    auto sys_config = make_systems_configuration<ListOfPlugins>(
        // this does not matter
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
        make_iteration_policy(iterate_across_system_types_in(std::move(across)),
                              // for testing iteration policy
                              iterate_over_systems_in(std::move(over))));
    return make_simulator_configuration(
        std::move(sys_config), DefaultDomainConfiguration{},
        make_parallel_configuration(4UL), std::move(env_config));
  }

  template <typename... Args>
  inline decltype(auto) make_integrated_simulator(Args... args) {
    //
    using Systems =
        tmpl::list<::elastica::cosserat_rod::CosseratRod,
                   ::elastica::cosserat_rod::CosseratRodWithoutDamping,
                   ::elastica::rigid_body::Sphere>;
    return elastica::make_simulator<::elastica::modules::Environments,
                                    ::elastica::modules::EnableReset>(
        make_configuration<Systems>(std::move(args)...));
  }

  template <typename... Args>
  inline decltype(auto) setup_integrated_sim_with_state(
      const std::size_t n_units, const std::size_t n_elements_per_slice,
      Args... args) {
    auto sim = make_integrated_simulator(std::move(args)...);
    sim->reset();
    {
      using Plugin = ::elastica::cosserat_rod::CosseratRod;

      using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;

      for (auto i = 0UL; i < n_units; ++i) {
        Init initializer{Init::NElement{n_elements_per_slice},
                         Init::Material{::elastica::copper},
                         Init::Radius{0.2},
                         Init::Length{1.0},
                         Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
                         Init::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
                         Init::Normal{::elastica::Vec3{1.0, 0.0, 0.0}},
                         Init::ForceDampingCoefficient{0.2},
                         Init::TorqueDampingCoefficient{0.2}};

        sim->template emplace_back<Plugin>(initializer.initialize<Plugin>());
      }
    }

    {
      using Plugin = ::elastica::cosserat_rod::CosseratRodWithoutDamping;

      using Init = ::elastica::cosserat_rod::
          StraightCosseratRodWithoutDampingInitializer;

      for (auto i = 0UL; i < n_units; ++i) {
        Init initializer{Init::NElement{n_elements_per_slice},
                         Init::Material{::elastica::iron},
                         Init::Radius{0.2},
                         Init::Length{1.0},
                         Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
                         Init::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
                         Init::Normal{::elastica::Vec3{1.0, 0.0, 0.0}}};

        sim->template emplace_back<Plugin>(initializer.initialize<Plugin>());
      }
    }

    {
      using Plugin = ::elastica::rigid_body::Sphere;
      // this will result in 2 blocks with the Restrictionpolcy we have in place

      std::for_each(
          elastica::make_counting_iterator(0UL),
          elastica::make_counting_iterator(n_elements_per_slice),
          [&](std::size_t) {
            auto initializer = ::blocks::initialize_block<Plugin>(
                ::blocks::initialize<::elastica::tags::Position>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::Director>(
                    []() -> ::elastica::Rot3 {
                      return 2.0 * ::elastica::identity3();
                    }),
                ::blocks::initialize<::elastica::tags::Radius>(
                    []() { return 0.3; }),
                ::blocks::initialize<::elastica::tags::Velocity>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::AngularVelocity>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::Acceleration>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::AngularAcceleration>(
                    []() {
                      return ::elastica::Vec3{1., 0., 0.};
                    }),
                ::blocks::initialize<::elastica::tags::Material>(
                    []() { return ::elastica::fir; }));

            sim->template emplace_back<Plugin>(std::move(initializer));
          });
    }

    return sim;
  }

  template <typename... Args>
  void test_integrated_transferer(Args... args) {
    const std::size_t n_units = 4UL;
    const std::size_t n_elements_per_slice = 24UL;
    auto sim = setup_integrated_sim_with_state(n_units, n_elements_per_slice,
                                               std::move(args)...);

    // now check for emplaced values
    using ::elastica::collision::get_collection;
    using CRP = ::elastica::collision::CapsuleFromRodPrimitive;
    using SP = ::elastica::collision::SpherePrimitive;

    auto& primitives = sim->primitives();

    auto& crp = get_collection<CRP>(primitives);
    CHECK(::elastica::size(crp) == 0UL);

    auto& sp = get_collection<SP>(primitives);
    CHECK(::elastica::size(sp) == 0UL);

    sim->finalize();
    // because the environments do not need support for primitives, they are not
    // initialized, so we initialize them
    if (sim->primitives_are_required()){
      // it takes care of populating it
    } else{
      sim->populate_primitives();
    }

    {  // size check
      // *2 for both types of rod
      CHECK(::elastica::size(crp) == n_units * n_elements_per_slice * 2);
      CHECK(::elastica::size(sp) == n_elements_per_slice);
    }

    // after this, emplacement should have occurred. We can now transfer
    // contents and check
    using ::elastica::collision::Transferer;

    {
      Transferer from_transferer{};

      from_transferer(::elastica::collision::from, *sim);

      // post from transfer
      auto post_capsule_check = [](auto const& p) {
        CHECK(p.get_radius() == approx(0.2));
        // CHECK(p.get_position() == ::elastica::Vec3{1., 0., 0.});
        CHECK(p.get_rotation_matrix() == (::elastica::identity3()));
        CHECK(p.get_velocity() == ::elastica::Vec3{0., 0., 0.});
        CHECK(p.get_angular_velocity() == ::elastica::Vec3{0., 0., 0.});
      };

      std::for_each(std::cbegin(crp), std::cend(crp), post_capsule_check);

      auto check_capsule_position = [=](std::size_t index,
                                        ::elastica::Vec3 pos) {
        const auto dl = ::elastica::real_t(1.0) / n_elements_per_slice;
        // centers is at half the intervals!
        const ::elastica::Vec3 expected =
            ::elastica::Vec3{0.0, 0.0, 0.0} +
            ::elastica::Vec3{
                0.0, 0.0,
                static_cast<::elastica::real_t>(index) * dl + 0.5 * dl};
        CHECK(pos == expected);
      };

      std::for_each(elastica::make_counting_iterator(0UL),
                    elastica::make_counting_iterator(n_elements_per_slice),
                    [&, check_capsule_position](std::size_t index) {
                      // use index to check position
                      check_capsule_position(index,
                                             crp.at(index).get_position());
                    });
    }

    {  // post to transfer
      Transferer to_transferer{};

      std::for_each(std::begin(crp), std::end(crp), [](auto& p) {
        p.add_force_at_position(::elastica::Vec3{1.0, 2.0, 3.0},
                                ::elastica::Vec3{1.0, 2.0, 3.0});
      });

      std::for_each(std::begin(sp), std::end(sp), [](auto& p) {
        p.add_force_at_position(::elastica::Vec3{1.0, 2.0, 3.0},
                                ::elastica::Vec3{1.0, 2.0, 3.0});
      });

      auto pre_transfer_check_block = [](auto& m) {
        CHECK(::blocks::get<::elastica::tags::ExternalLoads>(m) == 0.0);
        CHECK(::blocks::get<::elastica::tags::ExternalTorques>(m) == 0.0);
      };
      auto post_transfer_check_block = [](auto& m) {
        CHECK_FALSE(::blocks::get<::elastica::tags::ExternalLoads>(m) == 0.0);
        CHECK_FALSE(::blocks::get<::elastica::tags::ExternalTorques>(m) == 0.0);
      };

      //
      sim->for_each_block([=](auto& m) { pre_transfer_check_block(m); });
      to_transferer(::elastica::collision::to, *sim);
      sim->for_each_block([=](auto& m) { post_transfer_check_block(m); });
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.z.Integrated.Environments.Transferer",
                     "[Integrated][Simulator][Interface][Environments]") {
    // clang-format on
    using Configurations =
        tmpl::list<tmpl::pair<::elastica::configuration::sequential_policy,
                              ::elastica::configuration::sequential_policy>,
                   tmpl::pair<::elastica::configuration::parallel_policy,
                              ::elastica::configuration::sequential_policy>,
                   tmpl::pair<::elastica::configuration::sequential_policy,
                              ::elastica::configuration::parallel_policy>,
                   tmpl::pair<::elastica::configuration::parallel_policy,
                              ::elastica::configuration::parallel_policy>>;
    tmpl::for_each<Configurations>([](auto conf) {
      using C = tmpl::type_from<decltype(conf)>;
      SECTION(::pretty_type::get_name<C>()) {
        test_integrated_transferer(typename C::first_type{},
                                   typename C::second_type{});
      }
    });
  }

}  // namespace
