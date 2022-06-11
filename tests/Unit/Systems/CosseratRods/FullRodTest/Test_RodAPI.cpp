//
// Created by Tejaswin Parthasarathy on 12/14/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Materials.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp"  // TODO : replace later
#include "Utilities/AsConst.hpp"

namespace {

  template <class Rod>
  void check_member_API(Rod& cosserat_rod) {
    /// [rod_member_api]
    cosserat_rod.get_position();
    cosserat_rod.get_director();
    //
    cosserat_rod.get_reference_curvature();
    cosserat_rod.get_element_length();
    cosserat_rod.get_reference_element_length();
    cosserat_rod.get_reference_curvature();
    cosserat_rod.get_reference_shear_stretch_strain();
    cosserat_rod.get_tangent();
    //
    cosserat_rod.get_element_dimension();
    cosserat_rod.get_element_volume();
    //
    cosserat_rod.get_velocity();
    cosserat_rod.get_angular_velocity();
    //
    cosserat_rod.get_material();
    cosserat_rod.get_mass();
    cosserat_rod.get_external_loads();
    cosserat_rod.get_external_torques();
    //
    cosserat_rod.get_acceleration();
    cosserat_rod.get_angular_acceleration();
    /// [rod_member_api]

    /// [rod_member_index_api]
    const std::size_t index = 3UL;  // consider n_voronoi into account
    cosserat_rod.get_position(index);
    cosserat_rod.get_director(index);

    cosserat_rod.get_reference_curvature(index);
    cosserat_rod.get_element_length(index);
    cosserat_rod.get_reference_element_length(index);
    cosserat_rod.get_reference_curvature(index);
    cosserat_rod.get_reference_shear_stretch_strain(index);
    cosserat_rod.get_tangent(index);

    cosserat_rod.get_element_dimension(index);
    cosserat_rod.get_element_volume(index);

    cosserat_rod.get_velocity(index);
    cosserat_rod.get_angular_velocity(index);

    cosserat_rod.get_material(index);
    cosserat_rod.get_mass(index);

    cosserat_rod.get_external_loads(index);
    cosserat_rod.get_external_torques(index);
    /// [rod_member_index_api]
  }

  // TODO : Refactor into its own plugin
  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using Plugin = elastica::cosserat_rod::CosseratRodPlugin<
      Traits, blocks::Block,
      elastica::cosserat_rod::component::WithCircularCosseratRod,
      elastica::cosserat_rod::component::
          WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;
  using CosseratRod = blocks::Block<Plugin>;

  using real_type = typename Traits::DataType::real_type;

  ELASTICA_TEST_CASE("Unit.CosseratRods.API.member_functions",
                     "[Systems][Integrated]") {
    CosseratRod cosserat_rod{};

    // this will form
    auto add_a_rod = [&cosserat_rod]() {
      elastica::MaterialID myMaterial = elastica::create_material(
          7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);
      using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
      Init straight_initializer{
          // Initialization Wrapper
          Init::NElement{5UL},
          Init::Material{myMaterial},
          Init::Radius{0.007522},
          Init::Length{0.18},
          Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
          Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
          Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
          Init::ForceDampingRate{0.2},
          Init::TorqueDampingRate{0.2},
      };
      return cosserat_rod.emplace_back(
          straight_initializer.initialize<Plugin>());
    };
    auto first_it = add_a_rod();
    auto second_it = add_a_rod();

    SECTION("Block API") {
      EMPTY_TEST();

      check_member_API(cosserat_rod);
      check_member_API(cpp17::as_const(cosserat_rod));
    }
    SECTION("Slice API") {
      EMPTY_TEST();

      check_member_API(first_it);
      check_member_API(cpp17::as_const(first_it));
      check_member_API(second_it);
      check_member_API(cpp17::as_const(second_it));
    }

    CHECK(true);
  }

  template <class Rod>
  void check_free_API(Rod& cosserat_rod) {
    /// [rod_free_api]
    ::elastica::position(cosserat_rod);
    ::elastica::director(cosserat_rod);
    ::elastica::velocity(cosserat_rod);
    ::elastica::angular_velocity(cosserat_rod);
    ::elastica::external_loads(cosserat_rod);
    ::elastica::external_torques(cosserat_rod);
    /// [rod_free_api]

    /// [rod_free_index_api]
    const std::size_t index = 4UL;
    ::elastica::position(cosserat_rod, index);
    ::elastica::director(cosserat_rod, index);
    ::elastica::velocity(cosserat_rod, index);
    ::elastica::angular_velocity(cosserat_rod, index);
    ::elastica::external_loads(cosserat_rod, index);
    ::elastica::external_torques(cosserat_rod, index);
    /// [rod_free_index_api]
  }

  ELASTICA_TEST_CASE("Unit.CosseratRods.API.free_functions",
                     "[Systems][Integrated]") {
    CosseratRod cosserat_rod{};

    // this will form
    auto add_a_rod = [&cosserat_rod]() {
      elastica::MaterialID myMaterial = elastica::create_material(
          7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);
      using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
      Init straight_initializer{
          // Initialization Wrapper
          Init::NElement{5UL},
          Init::Material{myMaterial},
          Init::Radius{0.007522},
          Init::Length{0.18},
          Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
          Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
          Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
          Init::ForceDampingRate{0.2},
          Init::TorqueDampingRate{0.2},
      };
      return cosserat_rod.emplace_back(
          straight_initializer.initialize<Plugin>());
    };
    auto first_it = add_a_rod();
    auto second_it = add_a_rod();

    SECTION("Block API") {
      EMPTY_TEST();

      check_free_API(cosserat_rod);
      check_free_API(cpp17::as_const(cosserat_rod));
    }
    SECTION("Slice API") {
      EMPTY_TEST();

      check_free_API(first_it);
      check_free_API(cpp17::as_const(first_it));
      check_free_API(second_it);
      check_free_API(cpp17::as_const(second_it));
    }
  }

}  // namespace