//
// Created by Tejaswin Parthasarathy on 9/25/21.
//

#include <iostream>
#include <string>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Materials.hpp"

namespace {
  using namespace elastica;

  ELASTICA_TEST_CASE("Unit.Simulator.Materials", "[Unit][Simulator]") {
    // goddamn many parameters
    std::string name("my_material");
    const real_t density(2.54);
    const real_t cor(0.8);
    const real_t poisson(0.5);
    const real_t youngs_modulus(80.0);
    const real_t shear_modulus(60.0);
    const real_t stiffness(100.0);
    const real_t normal_damping(10.0);
    const real_t tangential_damping(10.0);
    const real_t forward_static_friction_coefficient(0.1);
    const real_t backward_static_friction_coefficient(0.2);
    const real_t lateral_static_friction_coefficient(0.15);
    const real_t forward_dynamic_friction_coefficient(0.05);
    const real_t backward_dynamic_friction_coefficient(0.1);
    const real_t lateral_dynamic_friction_coefficient(0.075);

    SECTION("Full constructor") {
      Material my_material(name, density, cor, poisson, youngs_modulus,
                           shear_modulus, stiffness, normal_damping,
                           tangential_damping,
                           forward_static_friction_coefficient,
                           backward_static_friction_coefficient,
                           lateral_static_friction_coefficient,
                           forward_dynamic_friction_coefficient,
                           backward_dynamic_friction_coefficient,
                           lateral_dynamic_friction_coefficient);

      CHECK(my_material.get_name() == name);
      CHECK(my_material.get_density() == approx(density));
      CHECK(my_material.get_restitution() == approx(cor));
      CHECK(my_material.get_poisson_ratio() == approx(poisson));
      CHECK(my_material.get_youngs_modulus() == approx(youngs_modulus));
      CHECK(my_material.get_shear_modulus() == approx(shear_modulus));
      CHECK(my_material.get_stiffness() == approx(stiffness));
      CHECK(my_material.get_normal_damping() == approx(normal_damping));
      CHECK(my_material.get_tangential_damping() == approx(tangential_damping));
      CHECK(my_material.get_static_friction() ==
            approx(forward_static_friction_coefficient));
      CHECK(my_material.get_forward_static_friction() ==
            approx(forward_static_friction_coefficient));
      CHECK(my_material.get_backward_static_friction() ==
            approx(backward_static_friction_coefficient));
      CHECK(my_material.get_lateral_static_friction() ==
            approx(lateral_static_friction_coefficient));
      CHECK(my_material.get_dynamic_friction() ==
            approx(forward_dynamic_friction_coefficient));
      CHECK(my_material.get_forward_dynamic_friction() ==
            approx(forward_dynamic_friction_coefficient));
      CHECK(my_material.get_backward_dynamic_friction() ==
            approx(backward_dynamic_friction_coefficient));
      CHECK(my_material.get_lateral_dynamic_friction() ==
            approx(lateral_dynamic_friction_coefficient));
    }
    SECTION("Forwarding constructor") {
      Material my_material(name, density, cor, poisson, youngs_modulus,
                           shear_modulus, stiffness, normal_damping,
                           tangential_damping,
                           forward_static_friction_coefficient,
                           forward_dynamic_friction_coefficient);

      CHECK(my_material.get_name() == name);
      CHECK(my_material.get_density() == approx(density));
      CHECK(my_material.get_restitution() == approx(cor));
      CHECK(my_material.get_poisson_ratio() == approx(poisson));
      CHECK(my_material.get_youngs_modulus() == approx(youngs_modulus));
      CHECK(my_material.get_shear_modulus() == approx(shear_modulus));
      CHECK(my_material.get_stiffness() == approx(stiffness));
      CHECK(my_material.get_normal_damping() == approx(normal_damping));
      CHECK(my_material.get_tangential_damping() == approx(tangential_damping));
      CHECK(my_material.get_static_friction() ==
            approx(forward_static_friction_coefficient));
      CHECK(my_material.get_forward_static_friction() ==
            approx(forward_static_friction_coefficient));
      CHECK(my_material.get_backward_static_friction() ==
            approx(forward_static_friction_coefficient));
      CHECK(my_material.get_lateral_static_friction() ==
            approx(forward_static_friction_coefficient));
      CHECK(my_material.get_dynamic_friction() ==
            approx(forward_dynamic_friction_coefficient));
      CHECK(my_material.get_forward_dynamic_friction() ==
            approx(forward_dynamic_friction_coefficient));
      CHECK(my_material.get_backward_dynamic_friction() ==
            approx(forward_dynamic_friction_coefficient));
      CHECK(my_material.get_lateral_dynamic_friction() ==
            approx(forward_dynamic_friction_coefficient));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Materials.methods", "[Unit][Simulator]") {
    SECTION("create_material : fully detailed") {
      // goddamn many parameters
      std::string name("material_first");
      const real_t density(2.54);
      const real_t cor(0.8);
      const real_t youngs_modulus(80.0);
      const real_t shear_modulus(60.0);
      const real_t stiffness(100.0);
      const real_t normal_damping(10.0);
      const real_t tangential_damping(10.0);
      const real_t forward_static_friction_coefficient(0.1);
      const real_t backward_static_friction_coefficient(0.2);
      const real_t lateral_static_friction_coefficient(0.15);
      const real_t forward_dynamic_friction_coefficient(0.05);
      const real_t backward_dynamic_friction_coefficient(0.1);
      const real_t lateral_dynamic_friction_coefficient(0.075);
      auto first_mID =
          create_material(name, density, cor, youngs_modulus, shear_modulus,
                          stiffness, normal_damping, tangential_damping,
                          forward_static_friction_coefficient,
                          backward_static_friction_coefficient,
                          lateral_static_friction_coefficient,
                          forward_dynamic_friction_coefficient,
                          backward_dynamic_friction_coefficient,
                          lateral_dynamic_friction_coefficient);

      CHECK(Material::get_name(first_mID) == name);
      CHECK(Material::get_density(first_mID) == approx(density));
      CHECK(Material::get_restitution(first_mID) == approx(cor));
      CHECK(Material::get_poisson_ratio(first_mID) == approx(0.5));
      CHECK(Material::get_youngs_modulus(first_mID) == approx(youngs_modulus));
      CHECK(Material::get_shear_modulus(first_mID) == approx(shear_modulus));
      CHECK(Material::get_stiffness(first_mID) == approx(stiffness));
      CHECK(Material::get_normal_damping(first_mID) == approx(normal_damping));
      CHECK(Material::get_tangential_damping(first_mID) ==
            approx(tangential_damping));
      CHECK(Material::get_static_friction(first_mID) ==
            approx(forward_static_friction_coefficient));
      CHECK(Material::get_forward_static_friction(first_mID) ==
            approx(forward_static_friction_coefficient));
      CHECK(Material::get_backward_static_friction(first_mID) ==
            approx(backward_static_friction_coefficient));
      CHECK(Material::get_lateral_static_friction(first_mID) ==
            approx(lateral_static_friction_coefficient));
      CHECK(Material::get_dynamic_friction(first_mID) ==
            approx(forward_dynamic_friction_coefficient));
      CHECK(Material::get_forward_dynamic_friction(first_mID) ==
            approx(forward_dynamic_friction_coefficient));
      CHECK(Material::get_backward_dynamic_friction(first_mID) ==
            approx(backward_dynamic_friction_coefficient));
      CHECK(Material::get_lateral_dynamic_friction(first_mID) ==
            approx(lateral_dynamic_friction_coefficient));
    }

    SECTION("create_material : Partially detailed") {
      std::string name("material_second");
      const real_t density(2.54);
      const real_t cor(0.8);
      const real_t youngs_modulus(80.0);
      const real_t shear_modulus(60.0);
      const real_t stiffness(100.0);
      const real_t normal_damping(10.0);
      const real_t tangential_damping(10.0);
      const real_t static_friction_coefficient(0.1);
      const real_t dynamic_friction_coefficient(0.05);

      auto second_mID = create_material(
          name, density, cor, youngs_modulus, shear_modulus, stiffness,
          normal_damping, tangential_damping, static_friction_coefficient,
          dynamic_friction_coefficient);

      CHECK(Material::get_name(second_mID) == name);
      CHECK(Material::get_density(second_mID) == approx(density));
      CHECK(Material::get_restitution(second_mID) == approx(cor));
      CHECK(Material::get_poisson_ratio(second_mID) == approx(0.5));
      CHECK(Material::get_youngs_modulus(second_mID) == approx(youngs_modulus));
      CHECK(Material::get_shear_modulus(second_mID) == approx(shear_modulus));
      CHECK(Material::get_stiffness(second_mID) == approx(stiffness));
      CHECK(Material::get_normal_damping(second_mID) == approx(normal_damping));
      CHECK(Material::get_tangential_damping(second_mID) ==
            approx(tangential_damping));
      CHECK(Material::get_static_friction(second_mID) ==
            approx(static_friction_coefficient));
      CHECK(Material::get_forward_static_friction(second_mID) ==
            approx(static_friction_coefficient));
      CHECK(Material::get_backward_static_friction(second_mID) ==
            approx(static_friction_coefficient));
      CHECK(Material::get_lateral_static_friction(second_mID) ==
            approx(static_friction_coefficient));
      CHECK(Material::get_dynamic_friction(second_mID) ==
            approx(dynamic_friction_coefficient));
      CHECK(Material::get_forward_dynamic_friction(second_mID) ==
            approx(dynamic_friction_coefficient));
      CHECK(Material::get_backward_dynamic_friction(second_mID) ==
            approx(dynamic_friction_coefficient));
      CHECK(Material::get_lateral_dynamic_friction(second_mID) ==
            approx(dynamic_friction_coefficient));
    }

    SECTION("find materials") {
      CHECK_NOTHROW(Material::find("material_first"));
      CHECK_NOTHROW(Material::find("material_second"));
    }

    SECTION("find materials by prefix") {
      auto v = Material::find_prefix("material");
      // two materials need to be there
      CHECK(v.size() == 2UL);
    }

    SECTION("pairwise functions") {
      auto fid = Material::find("material_first");
      auto sid = Material::find("material_second");

      // TODO : detailed checking if possible
      CHECK(Material::get_static_friction(fid, sid) > 0.0);
      CHECK(Material::get_forward_static_friction(fid, sid) > 0.0);
      CHECK(Material::get_backward_static_friction(fid, sid) > 0.0);
      CHECK(Material::get_lateral_static_friction(fid, sid) > 0.0);

      CHECK(Material::get_dynamic_friction(fid, sid) > 0.0);
      CHECK(Material::get_forward_dynamic_friction(fid, sid) > 0.0);
      CHECK(Material::get_backward_dynamic_friction(fid, sid) > 0.0);
      CHECK(Material::get_lateral_dynamic_friction(fid, sid) > 0.0);

      CHECK(Material::get_restitution(fid, sid) > 0.0);
      CHECK(Material::get_youngs_modulus(fid, sid) > 0.0);

      // contact measures
      CHECK(Material::get_stiffness(fid, sid) > 0.0);
      CHECK(Material::get_normal_damping(fid, sid) > 0.0);
      CHECK(Material::get_tangential_damping(fid, sid) > 0.0);

      // set functions
      Material::set_static_friction(fid, sid, 0.2);
      Material::set_backward_static_friction(fid, sid, 0.3);
      Material::set_lateral_static_friction(fid, sid, 0.4);

      CHECK(Material::get_static_friction(fid, sid) == approx(0.2));
      CHECK(Material::get_forward_static_friction(fid, sid) == approx(0.2));
      CHECK(Material::get_backward_static_friction(fid, sid) == approx(0.3));
      CHECK(Material::get_lateral_static_friction(fid, sid) == approx(0.4));

      Material::set_dynamic_friction(fid, sid, 0.2);
      Material::set_backward_dynamic_friction(fid, sid, 0.3);
      Material::set_lateral_dynamic_friction(fid, sid, 0.4);

      CHECK(Material::get_dynamic_friction(fid, sid) == approx(0.2));
      CHECK(Material::get_forward_dynamic_friction(fid, sid) == approx(0.2));
      CHECK(Material::get_backward_dynamic_friction(fid, sid) == approx(0.3));
      CHECK(Material::get_lateral_dynamic_friction(fid, sid) == approx(0.4));

      Material::set_restitution(fid, sid, 0.5);
      CHECK(Material::get_restitution(fid, sid) == approx(0.5));
    }
  }

}  // namespace