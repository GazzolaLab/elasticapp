//
// Created by Tejaswin Parthasarathy on 4/26/22.
//
#include <algorithm>
#include <vector>

#include "Framework/TestingFramework.hpp"
//
#include "Systems/CosseratRods.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Size.hpp"
//
#include "Simulator/Environments/Transfer.hpp"

namespace {

  template <typename Model>
  auto test_emplacement(Model const& model) {
    using Primitive = ::elastica::collision::CapsuleFromRodPrimitive;
    using Primitives = std::vector<Primitive>;

    // random values
    Primitives primitives{};

    for (auto slice : model) {
      const std::size_t n_elem = ::elastica::size(slice);

      std::for_each(
          elastica::make_counting_iterator(0UL),
          elastica::make_counting_iterator(n_elem), [&](std::size_t i) {
            primitives.emplace_back(i, 0UL, true, slice.get_material(i));
          });
    }

    std::for_each(std::cbegin(primitives), std::cend(primitives),
                  [](Primitive const& p) {
                    CHECK(p.get_material() == ::elastica::copper);
                  });
    return primitives;
  }

  template <typename Model>
  void test_transfer(Model& model) {
    auto primitives = test_emplacement(model);

    const std::size_t n_slices = ::blocks::n_units(model);
    const std::size_t n_elements_per_slice = ::elastica::size(model[0UL]);

    auto post_check = [](auto const& p) {
      CHECK(p.get_radius() == approx(0.2));
      // CHECK(p.get_position() == ::elastica::Vec3{1., 0., 0.});
      // don't check for entire matrix, only check for tangents
      CHECK(blaze::row<2UL>(p.get_rotation_matrix()) ==
            blaze::row<2UL>(::elastica::identity3()));
      CHECK(p.get_velocity() == ::elastica::Vec3{0., 0., 0.});
      CHECK(p.get_angular_velocity() == ::elastica::Vec3{0., 0., 0.});
    };

    std::for_each(elastica::make_counting_iterator(0UL),
                  elastica::make_counting_iterator(n_slices),
                  [&, begit = std::begin(primitives)](std::size_t i) mutable {
                    auto slice = ::blocks::slice(cpp17::as_const(model), i);
                    auto endit =
                        begit +
                        static_cast<typename decltype(begit)::difference_type>(
                            elastica::size(slice));
                    using namespace ::elastica::collision;
                    Transfer<decltype(slice)>::template do_transfer(
                        ::elastica::collision::from, slice,
                        ::elastica::collision::to, begit, endit);
                    std::exchange(begit, endit);
                  });

    std::for_each(std::begin(primitives), std::end(primitives), post_check);

    auto check_position = [=](std::size_t index, ::elastica::Vec3 pos) {
      const auto dl = ::elastica::real_t(1.0) / n_elements_per_slice;
      // centers is at half the intervals!
      const ::elastica::Vec3 expected =
          ::elastica::Vec3{0.0, 0.0, 0.0} +
          ::elastica::Vec3{
              0.0, 0.0, static_cast<::elastica::real_t>(index) * dl + 0.5 * dl};
      CHECK(pos == expected);
    };
    std::for_each(elastica::make_counting_iterator(0UL),
                  elastica::make_counting_iterator(n_elements_per_slice),
                  [&, check_position](std::size_t index) {
                    // use index to check position
                    check_position(index, primitives.at(index).get_position());
                  });

    std::for_each(std::begin(primitives), std::end(primitives), [](auto& p) {
      p.add_force_at_position(::elastica::Vec3{1.0, 2.0, 3.0},
                              ::elastica::Vec3{1.0, 2.0, 3.0});
    });

    auto pre_transfer_check_block = [](Model& m) {
      CHECK(::blocks::get<::elastica::tags::ExternalLoads>(m) == 0.0);
      CHECK(::blocks::get<::elastica::tags::ExternalTorques>(m) == 0.0);
    };
    auto post_transfer_check_block = [](Model& m) {
      CHECK_FALSE(::blocks::get<::elastica::tags::ExternalLoads>(m) == 0.0);
      CHECK_FALSE(::blocks::get<::elastica::tags::ExternalTorques>(m) == 0.0);
    };
    pre_transfer_check_block(model);

    std::for_each(elastica::make_counting_iterator(0UL),
                  elastica::make_counting_iterator(n_slices),
                  [&, begit = std::cbegin(primitives)](std::size_t i) mutable {
                    auto slice = ::blocks::slice(model, i);
                    auto endit =
                        begit +
                        static_cast<typename decltype(begit)::difference_type>(
                            elastica::size(slice));
                    using namespace ::elastica::collision;
                    Transfer<decltype(slice)>::template do_transfer(
                        ::elastica::collision::to, slice,
                        ::elastica::collision::from, begit, endit);
                    std::exchange(begit, endit);
                  });
    post_transfer_check_block(model);
  }

  ELASTICA_TEST_CASE("Unit.Systems.Customization.CosseratRod",
                     "[Systems][Unit]") {
    using Plugin = ::elastica::cosserat_rod::CosseratRod;
    using Model = ::blocks::Block<Plugin>;
    Model model;
    const std::size_t n_slices = 2UL;
    const std::size_t n_elements_per_slice = 20UL;

    // refactor? : using straight initializer
    std::for_each(
        elastica::make_counting_iterator(0UL),
        elastica::make_counting_iterator(n_slices), [&](std::size_t) {
          using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
          Init initializer{Init::NElement{n_elements_per_slice},
                           Init::Material{::elastica::copper},
                           Init::Radius{0.2},
                           Init::Length{1.0},
                           Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
                           Init::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
                           Init::Normal{::elastica::Vec3{1.0, 0.0, 0.0}},
                           Init::ForceDampingRate{0.2},
                           Init::TorqueDampingRate{0.2}};

          model.emplace(initializer.initialize<Plugin>());
        });

    SECTION("emplacement") { test_emplacement(model); }
    SECTION("transfer") { test_transfer(model); }
  }

  ELASTICA_TEST_CASE("Unit.Systems.Customization.CosseratRodWithoutDamping",
                     "[Systems][Unit]") {
    using Plugin = ::elastica::cosserat_rod::CosseratRodWithoutDamping;
    using Model = ::blocks::Block<Plugin>;
    Model model;
    const std::size_t n_slices = 2UL;
    const std::size_t n_elements_per_slice = 20UL;

    // refactor? : using straight initializer
    std::for_each(elastica::make_counting_iterator(0UL),
                  elastica::make_counting_iterator(n_slices), [&](std::size_t) {
                    using Init = ::elastica::cosserat_rod::
                        StraightCosseratRodWithoutDampingInitializer;
                    Init initializer{
                        Init::NElement{n_elements_per_slice},
                        Init::Material{::elastica::copper},
                        Init::Radius{0.2},
                        Init::Length{1.0},
                        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
                        Init::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
                        Init::Normal{::elastica::Vec3{1.0, 0.0, 0.0}},
                    };

                    model.emplace(initializer.initialize<Plugin>());
                  });

    SECTION("emplacement") { test_emplacement(model); }
    SECTION("transfer") { test_transfer(model); }
  }

}  // namespace
