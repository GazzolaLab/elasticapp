//==============================================================================
/*!
//  \file
//  \brief HDF5 example
//
//  See LICENSE for details.
*/
//==============================================================================

#include "Simulator/IO/H5.hpp"
#include "Simulator/Materials.hpp"
#include "Simulator/Simulator.hpp"
// CosseratRod is a default system now, so we include it
#include "Systems/CosseratRods.hpp"
// We also include Spheres
#include "Systems/Sphere.hpp"
#include "Time/Time.hpp"

int main() {
  using ::elastica::cosserat_rod::CosseratRod;
  using ::elastica::rigid_body::Sphere;

  namespace ec = elastica::configuration;
  auto system_config =
      ec::make_systems_configuration<tmpl::list<CosseratRod, Sphere>>(
          // so that we have more than one block for testing purposes
          ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{30UL}),
          ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                    ec::iterate_over_systems_in(ec::seq)));

  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{});

  auto simulator = elastica::make_simulator<>(simulator_config);

  /// [setup_state]
  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  // We first create a material for passing into the initializer, see
  // documentation of elastica::create_material
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      7.874,
      // coefficient of restitution
      0.24,
      // Young's modulus
      200,
      // Shear modulus
      200.0 / 1.24,
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // cofficient of dynamic friction
      0.1);

  StraightInit straight_initializer{
      StraightInit::NElement{10UL},
      StraightInit::Material{my_material},
      StraightInit::Radius{0.007522},
      StraightInit::Length{0.18},
      StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
      StraightInit::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
      StraightInit::ForceDampingRate{0.2},
      StraightInit::TorqueDampingRate{0.2},
  };
  StraightInit another_straight_initializer{
      StraightInit::Radius{0.02},
      StraightInit::Length{0.9},
      StraightInit::NElement{16UL},
      StraightInit::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
      StraightInit::ForceDampingRate{0.2},
      StraightInit::Material{my_material},
      StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
      StraightInit::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::TorqueDampingRate{0.2},
  };

  simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());
  simulator->emplace_back<CosseratRod>(
      another_straight_initializer.initialize<CosseratRod>());
  simulator->emplace_back<CosseratRod>(
      straight_initializer
          .but_with(StraightInit::Origin{::elastica::Vec3{1.0, 0.0, 0.0}})
          .initialize<CosseratRod>());
  simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>(
          ::blocks::initialize<::elastica::tags::Velocity>(
              [](std::size_t index) {
                return 0.1 + 0.002 * static_cast<double>(index);
              })));

  // Add spheres to the simluator
  auto sphere_initializer = ::blocks::initialize_block<Sphere>(
      ::blocks::initialize<::elastica::tags::Position>([&]() {
        return ::elastica::Vec3{0.0, 0.0, 2.0};
      }),
      ::blocks::initialize<::elastica::tags::Director>(
          []() { return ::elastica::identity3(); }),
      ::blocks::initialize<::elastica::tags::Radius>([]() { return 0.001; }),
      ::blocks::initialize<::elastica::tags::Velocity>([&]() {
        return ::elastica::Vec3{1.0, 0.0, 0.0};
      }),
      ::blocks::initialize<::elastica::tags::AngularVelocity>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Acceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::AngularAcceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Material>(
          []() { return ::elastica::copper; }));
  simulator->emplace_back<Sphere>(std::move(sphere_initializer));

  for (auto idx = 0U; idx < 10U; ++idx) {
    auto another_sphere_initializer = ::blocks::initialize_block<Sphere>(
        ::blocks::initialize<::elastica::tags::Position>([=]() {
          return ::elastica::Vec3{2.0, 3.0, 6.0 + static_cast<double>(idx)};
        }),
        ::blocks::initialize<::elastica::tags::Director>(
            []() { return ::elastica::identity3(); }),
        ::blocks::initialize<::elastica::tags::Radius>(
            [=]() { return 5.0 + static_cast<double>(idx); }),
        ::blocks::initialize<::elastica::tags::Velocity>([]() {
          return ::elastica::Vec3{1.0, 0.0, 0.0};
        }),
        ::blocks::initialize<::elastica::tags::AngularVelocity>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::Acceleration>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::AngularAcceleration>(
            []() { return ::elastica::Vec3{}; }),
        ::blocks::initialize<::elastica::tags::Material>(
            []() { return ::elastica::iron; }));
    simulator->emplace_back<Sphere>(std::move(another_sphere_initializer));
  }

  simulator->finalize();
  /// [setup_state]

  ::elastica::io::H5Metadata metadata{};

  ::elastica::TimeStepID id{1};
  for (int i = 0; i < 50; ++i)
    ++id;

  // Export/import only works only for single-threaded \elastica applications
  // See this issue for progress on fix:
  // https://github.com/tp5uiuc/elasticapp/issues/413
  simulator->for_each_system_sequential(metadata.write_at(
      {::elastica::Time{50}, ::elastica::TimeDelta{0.02}, id}));

  for (int i = 0; i < 50; ++i)
  ++id;

  simulator->for_each_system_sequential(metadata.write_at(
      {::elastica::Time{100}, ::elastica::TimeDelta{0.02}, id}));

}