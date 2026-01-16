//
// Created by Tejaswin Parthasarathy on 4/22/22.
//

#include <algorithm>
#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Systems/Sphere.hpp"
//
#include "Utilities/AsConst.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/End.hpp"
#include "Utilities/Math/Identity.hpp"

namespace {
  using namespace elastica;

  template <typename Model>
  void test_empty(Model& model) {
    // check if members work for an empty block
    CHECK(model.n_units() == 0UL);
    CHECK(::blocks::n_units(model) == 0UL);  // free function check
    CHECK(::blocks::size(model) == 0UL);     // free function check
    CHECK(model.empty());

    // block check
    CHECK_THROWS_AS(index_check(model, 0UL), std::out_of_range);
    CHECK_THROWS_AS(index_check(model, end - 1UL), std::out_of_range);

    CHECK_THROWS_WITH(index_check(model, 0UL), Catch::Contains("Index to be "));
    CHECK_THROWS_WITH(index_check(model, end - 1UL),
                      Catch::Contains("Index to be "));
  }

  ELASTICA_TEST_CASE("Unit.Systems.Sphere.Block", "[Systems][Unit]") {
    using Plugin = ::elastica::rigid_body::Sphere;
    using Model = ::blocks::Block<Plugin>;
    // using Variables = typename Model::Variables;

    using Traits = typename Model::Traits;
    using ElementsVector = typename Traits::DataType::Index::type;
    // using ElementsVectorSlice = typename Traits::DataType::Index::slice_type;
    // using ElementsVectorConstSlice =
    //     typename Traits::DataType::Index::const_slice_type;

    SECTION("Empty model") {
      Model model{};
      test_empty(model);
    }
    SECTION("Resizing to 1") {
      Model model{};
      CHECK(model.n_elements().size() == 0UL);

      CHECK(model.get_start_of_slice(0UL) == 0UL);
      CHECK(model.get_size_of_slice(0UL) == 1UL);

      auto initializer = ::blocks::initialize_block<Plugin>(
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

      using ExpectedSlice = ::elastica::rigid_body::detail::SphereBlockSlice;

      auto sliced_model = model.emplace(std::move(initializer));
      static_assert(std::is_same_v<decltype(sliced_model), ExpectedSlice>,
                    "Block slice failure with Spheres!");

      // check the base model
      CHECK(model.n_units() == 1UL);
      CHECK(::blocks::n_units(model) == 1UL);  // free function check
      CHECK(::blocks::size(model) == 1UL);     // free function check

      CHECK(model.n_elements() == ElementsVector{1UL});
      CHECK(model.size() == 1UL);
      CHECK(not model.empty());

      CHECK(index_check(model, 0UL) == 0UL);
      CHECK(index_check(model, end - 1UL) == 0UL);

      CHECK_THROWS_AS(index_check(model, 1UL), std::out_of_range);
      CHECK_THROWS_AS(index_check(model, end - 2UL), std::out_of_range);

      CHECK_THROWS_WITH(index_check(model, 1UL),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(index_check(model, end - 2UL),
                        Catch::Contains("Index to be "));

      // check the sliced model
      CHECK(static_cast<decltype(sliced_model) const&>(sliced_model)
                .n_elements(0UL) == 1UL);
      CHECK(sliced_model.size() == 1UL);
      CHECK(::blocks::size(sliced_model) == 1UL);  // free function check

      CHECK(not sliced_model.empty());
      CHECK(index_check(sliced_model, 0UL) == 0UL);
      CHECK(index_check(sliced_model, end - 1UL) == 0UL);

      CHECK_THROWS_AS(index_check(sliced_model, 2UL), std::out_of_range);
      CHECK_THROWS_AS(index_check(sliced_model, end - 2UL), std::out_of_range);

      CHECK_THROWS_WITH(index_check(sliced_model, 2UL),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(index_check(sliced_model, end - 2UL),
                        Catch::Contains("Index to be "));

      // noe check if a new slice matches the old slice
      CHECK(blocks::slice(model, 0UL) == sliced_model);
      // same thing really
      CHECK(model[0UL] == sliced_model);

      // check for a new slice of type const
      const auto const_sliced_model(blocks::slice(std::as_const(model), 0UL));
      CHECK((const_sliced_model).n_elements(0UL) == 1UL);
      CHECK(const_sliced_model.size() == 1UL);
      CHECK(::blocks::size(const_sliced_model) == 1UL);  // free function check
      CHECK(not const_sliced_model.empty());
      CHECK(index_check(const_sliced_model, 0UL) == 0UL);
      CHECK(index_check(const_sliced_model, end - 1UL) == 0UL);

      CHECK_THROWS_AS(index_check(const_sliced_model, 1UL), std::out_of_range);
      CHECK_THROWS_AS(index_check(const_sliced_model, end - 1UL - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(index_check(const_sliced_model, 1UL),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(index_check(const_sliced_model, end - 1UL - 1UL),
                        Catch::Contains("Index to be "));
    }
    {
      Model model{};  // common

      auto initializer = ::blocks::initialize_block<Plugin>(
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

      model.emplace(std::move(initializer));

      auto another_initializer = ::blocks::initialize_block<Plugin>(
          ::blocks::initialize<::elastica::tags::Position>([]() {
            return ::elastica::Vec3{1., 1., 1.};
          }),
          ::blocks::initialize<::elastica::tags::Director>(
              []() { return ::elastica::identity3(); }),
          ::blocks::initialize<::elastica::tags::Radius>([]() { return 2.0; }),
          ::blocks::initialize<::elastica::tags::Velocity>([]() {
            return ::elastica::Vec3{1., 1., 1.};
          }),
          ::blocks::initialize<::elastica::tags::AngularVelocity>([]() {
            return ::elastica::Vec3{1., 1., 1.};
          }),
          ::blocks::initialize<::elastica::tags::Acceleration>([]() {
            return ::elastica::Vec3{1., 1., 1.};
          }),
          ::blocks::initialize<::elastica::tags::AngularAcceleration>([]() {
            return ::elastica::Vec3{1., 1., 1.};
          }),
          ::blocks::initialize<::elastica::tags::Material>(
              []() { return ::elastica::copper; }));

      model.emplace(std::move(another_initializer));

      SECTION("blocks::get") {
        // check blocks first
        SECTION("Blocks") {
          auto check_matrix = [](auto& data) {
            CHECK(data(0, 0) == approx(0.0));
            CHECK(data(1, 0) == approx(0.0));
            CHECK(data(2, 0) == approx(0.0));

            // bad, use symbols instead
            CHECK(data(0, 1) == approx(1.0));
            CHECK(data(1, 1) == approx(1.0));
            CHECK(data(2, 1) == approx(1.0));
          };

          {
            using Tag = tags::NElement;
            auto& data = ::blocks::get<Tag>(model);
            CHECK(data[0UL] == 1UL);
            CHECK(data[1UL] == 1UL);
          }

          {
            using Tag = tags::Position;
            auto& data = ::blocks::get<Tag>(model);
            check_matrix(data);
          }
          {
            using Tag = tags::Director;
            auto& data = ::blocks::get<Tag>(model);
            for (auto i = 0UL; i < 3UL; ++i) {
              for (auto j = 0UL; j < 3UL; ++j) {
                for (auto k = 0UL; k < 2UL; ++k) {
                  if (i == j) {
                    CHECK(data(i, j, k) == approx(1.0));
                  } else {
                    CHECK(data(i, j, k) == approx(0.0));
                  }
                }
              }
            }
          }
          {
            using Tag = tags::Radius;
            auto& data = ::blocks::get<Tag>(model);
            CHECK(data[0UL] == approx(0.3));
            CHECK(data[1UL] == approx(2.0));
          }
          {
            using Tag = tags::Velocity;
            auto& data = ::blocks::get<Tag>(model);
            check_matrix(data);
          }
          {
            using Tag = tags::AngularVelocity;
            auto& data = ::blocks::get<Tag>(model);
            check_matrix(data);
          }
          {
            using Tag = tags::Acceleration;
            auto& data = ::blocks::get<Tag>(model);
            check_matrix(data);
          }
          {
            using Tag = tags::AngularAcceleration;
            auto& data = ::blocks::get<Tag>(model);
            check_matrix(data);
          }
          {
            using Tag = tags::Material;
            auto& data = ::blocks::get<Tag>(model);
            CHECK(data[0UL] == ::elastica::iron);
            CHECK(data[1UL] == ::elastica::copper);
          }
          {
            using Tag = tags::ExternalLoads;
            auto& data = ::blocks::get<Tag>(model);
            CHECK(data == approx(0.0));
          }
          {
            using Tag = tags::ExternalTorques;
            auto& data = ::blocks::get<Tag>(model);
            CHECK(data == approx(0.0));
          }
          {
            using Tag = tags::Mass;
            auto& data = ::blocks::get<Tag>(model);
            CHECK(data[0UL] ==
                  approx(::elastica::Material::get_density(::elastica::iron) *
                         4.0 / 3.0 * M_PI * 0.3 * 0.3 * 0.3));
            CHECK(data[1UL] ==
                  approx(::elastica::Material::get_density(::elastica::copper) *
                         4.0 / 3.0 * M_PI * 2.0 * 2.0 * 2.0));
          }
          {
            using Tag = tags::InvMassSecondMomentOfInertia;
            auto& data = ::blocks::get<Tag>(model);

            std::array<real_t, 2UL> radius{0.3, 2.0};
            std::array<real_t, 2UL> mass{
                ::elastica::Material::get_density(::elastica::iron) * 4.0 /
                    3.0 * M_PI * 0.3 * 0.3 * 0.3,
                ::elastica::Material::get_density(::elastica::copper) * 4.0 /
                    3.0 * M_PI * 2.0 * 2.0 * 2.0};

            for (auto i = 0UL; i < 3UL; ++i) {
              for (auto j = 0UL; j < 3UL; ++j) {
                if (i == j) {
                  auto index = 0UL;
                  CHECK(data(i, j, index) ==
                        approx(1.0 / (2.0 / 5.0 * mass[index] * radius[index] *
                                      radius[index])));
                  index = 1UL;
                  CHECK(data(i, j, index) ==
                        approx(1.0 / (2.0 / 5.0 * mass[index] * radius[index] *
                                      radius[index])));
                } else {
                  auto index = 0UL;
                  CHECK(data(i, j, index) == approx(0.0));
                  index = 1UL;
                  CHECK(data(i, j, index) == approx(0.0));
                }
              }
            }
          }
        }
        SECTION("BlockSlices") {
          // just check if the matrix is the same for now
          using Variables = typename Model::Variables;
          using Tags =
              tmpl::transform<Variables,
                              tmpl::bind<::blocks::parameter_t, tmpl::_1>>;

          auto first_slice = ::blocks::slice(model, 0UL);
          auto second_slice = ::blocks::slice(model, 1UL);

          tmpl::for_each<Tags>([&](auto tag) {
            using Tag = tmpl::type_from<decltype(tag)>;
            // Relies on blaze interface
            CHECK(std::addressof(::blocks::get<Tag>(first_slice).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
            CHECK(std::addressof(::blocks::get<Tag>(second_slice).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
          });
        }

        SECTION("ConstSlice") {
          // just check if the matrix is the same for now
          using Variables = typename Model::Variables;
          using Tags =
              tmpl::transform<Variables,
                              tmpl::bind<::blocks::parameter_t, tmpl::_1>>;

          auto first_slice = ::blocks::slice(std::as_const(model), 0UL);
          auto second_slice = ::blocks::slice(std::as_const(model), 1UL);

          tmpl::for_each<Tags>([&, &model = std::as_const(model)](auto tag) {
            using Tag = tmpl::type_from<decltype(tag)>;
            // Relies on blaze interface
            CHECK(std::addressof(::blocks::get<Tag>(first_slice).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
            CHECK(std::addressof(::blocks::get<Tag>(second_slice).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
          });
        }

        SECTION("BlockView") {
          // just check if the matrix is the same for now
          using Variables = typename Model::Variables;
          using Tags =
              tmpl::transform<Variables,
                              tmpl::bind<::blocks::parameter_t, tmpl::_1>>;

          auto first_view = ::blocks::slice(model, 0UL, 1UL);
          auto second_view = ::blocks::slice(model, 1UL, 2UL);

          tmpl::for_each<Tags>([&](auto tag) {
            using Tag = tmpl::type_from<decltype(tag)>;
            // Relies on blaze interface
            CHECK(std::addressof(::blocks::get<Tag>(first_view).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
            CHECK(std::addressof(::blocks::get<Tag>(second_view).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
          });
        }

        SECTION("ConstBlockView") {
          // just check if the matrix is the same for now
          using Variables = typename Model::Variables;
          using Tags =
              tmpl::transform<Variables,
                              tmpl::bind<::blocks::parameter_t, tmpl::_1>>;

          auto first_view = ::blocks::slice(std::as_const(model), 0UL, 1UL);
          auto second_view = ::blocks::slice(std::as_const(model), 1UL, 2UL);

          tmpl::for_each<Tags>([&](auto tag) {
            using Tag = tmpl::type_from<decltype(tag)>;
            // Relies on blaze interface
            CHECK(std::addressof(::blocks::get<Tag>(first_view).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
            CHECK(std::addressof(::blocks::get<Tag>(second_view).operand()) ==
                  std::addressof(::blocks::get<Tag>(model)));
          });
        }
      }

      auto check_member_API = [](auto& sphere_like) {
        auto index = 0UL;

        (void)(sphere_like.get_position());
        (void)(sphere_like.get_position(index));

        (void)(sphere_like.get_director());
        (void)(sphere_like.get_director(index));

        (void)(sphere_like.get_radius());
        (void)(sphere_like.get_radius(index));

        (void)(sphere_like.get_velocity());
        (void)(sphere_like.get_velocity(index));

        (void)(sphere_like.get_angular_velocity());
        (void)(sphere_like.get_angular_velocity(index));

        (void)(sphere_like.get_acceleration());
        (void)(sphere_like.get_angular_acceleration());

        (void)(sphere_like.get_external_loads());
        (void)(sphere_like.get_external_loads(index));

        (void)(sphere_like.get_external_torques());
        (void)(sphere_like.get_external_torques(index));

        (void)(sphere_like.get_material());
        (void)(sphere_like.get_material(index));
      };

      SECTION("member functions") {
        EMPTY_TEST();

        check_member_API(model);
        check_member_API(std::as_const(model));

        auto slice = ::blocks::slice(model, 0UL);
        check_member_API(slice);
        check_member_API(std::as_const(slice));

        auto const_slice = ::blocks::slice(std::as_const(model), 0UL);
        check_member_API(const_slice);
        check_member_API(std::as_const(const_slice));
      }

      auto check_access_API = [](auto& sphere_like) {
        using ::elastica::position;
        using ::elastica::director;
        using ::elastica::velocity;
        using ::elastica::angular_velocity;
        using ::elastica::external_loads;
        using ::elastica::external_torques;

        auto index = 0UL;

        (void)(position(sphere_like));
        (void)(position(sphere_like, index));

        (void)(director(sphere_like));
        (void)(director(sphere_like, index));

        (void)(velocity(sphere_like));
        (void)(velocity(sphere_like, index));

        (void)(angular_velocity(sphere_like));
        (void)(angular_velocity(sphere_like, index));

        (void)(external_loads(sphere_like));
        (void)(external_loads(sphere_like, index));

        (void)(external_torques(sphere_like));
        (void)(external_torques(sphere_like, index));
      };

      SECTION("Access") {
        EMPTY_TEST();

        check_access_API(model);
        check_access_API(std::as_const(model));

        auto slice = ::blocks::slice(model, 0UL);
        check_access_API(slice);
        check_access_API(std::as_const(slice));

        auto const_slice = ::blocks::slice(std::as_const(model), 0UL);
        check_access_API(const_slice);
        check_access_API(std::as_const(const_slice));
      }

      SECTION("Iteration") {
        EMPTY_TEST();

        for (auto slice : model) {
          check_member_API(slice);
          check_member_API(std::as_const(slice));
        }
        for (const auto slice : model) {
          check_member_API(slice);
          check_member_API(std::as_const(slice));
        }
      }
    }

    SECTION("Multiple emplacement") {
      Model model{};
      const std::size_t n_slices = 20UL;

      std::for_each(
          elastica::make_counting_iterator(0UL),
          elastica::make_counting_iterator(n_slices), [&](std::size_t) {
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
                    []() { return ::elastica::copper; }));

            model.emplace(std::move(initializer));
          });

      {
        std::for_each(elastica::make_counting_iterator(0UL),
                      elastica::make_counting_iterator(n_slices),
                      [&](std::size_t i) {
                        CHECK(model.get_material(i) == ::elastica::copper);

                        auto slice = ::blocks::slice(std::as_const(model), i);
                        CHECK(slice.get_material(0UL) == ::elastica::copper);
                      });
      }

      {
        std::for_each(elastica::make_counting_iterator(0UL),
                      elastica::make_counting_iterator(n_slices),
                      [&](std::size_t i) {
                        CHECK(model.get_radius(i) == approx(0.3));
                        auto slice = ::blocks::slice(std::as_const(model), i);
                        CHECK(slice.get_radius(0UL) == approx(0.3));
                      });
      }
    }
  }

}  // namespace
