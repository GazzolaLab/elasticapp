//
// Created by Tejaswin Parthasarathy on 7/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Simulator/IO/H5/Systems/RigidBody.hpp"
//
#include "Systems/Sphere.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.Sphere", "[Unit][Simulator][IO]") {
    // Setup
    using ::elastica::rigid_body::Sphere;
    using Block = ::elastica::rigid_body::detail::SphereBlock;
    Block block{};

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

    auto slice = block.emplace(std::move(sphere_initializer));

    // Clobber data
    Block another_block{};
    auto another_sphere_initializer = ::blocks::initialize_block<Sphere>(
        ::blocks::initialize<::elastica::tags::Position>([&]() {
          return ::elastica::Vec3{2.0, 3.0, 6.0};
        }),
        ::blocks::initialize<::elastica::tags::Director>(
            []() { return ::elastica::identity3(); }),
        ::blocks::initialize<::elastica::tags::Radius>([]() { return 5.0; }),
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
            []() { return ::elastica::iron; }));

    auto another_slice =
        another_block.emplace(std::move(another_sphere_initializer));

    SECTION("Slice") {
      CHECK_FALSE(slice.data() == another_slice.data());

      auto group = generate_h5("SphereSlice");
      // Write and flush
      elastica::io::write_data(group, slice, "data");
      group.getFile().flush();

      // Read data back
      elastica::io::read_data(group, another_slice, "data");

      // Post check
      CHECK(slice.data() == another_slice.data());
    }
    SECTION("VariableCache") {
      using Subset =
          tmpl::list<elastica::tags::Position, elastica::tags::Velocity>;

      auto group = generate_h5("SphereVariableCache");
      auto subset = blocks::cache<Subset>(slice);
      // Write and flush
      elastica::io::write_data(group, subset, "data");
      group.getFile().flush();

      // Read data back
      auto another_subset = blocks::cache<Subset>(another_slice);
      elastica::io::read_data(group, another_subset, "data");

      // Post check
      CHECK(subset.data() == another_subset.data());
      CHECK_FALSE(slice.data() == another_slice.data());
    }
  }

}  // namespace