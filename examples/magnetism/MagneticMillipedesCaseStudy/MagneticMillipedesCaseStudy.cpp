//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic cilia carpet study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale- All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "MagneticMillipedesCaseStudy.hpp"
#include "CustomConnection.cpp"

#include <cmath>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>

void MagneticMillipedesCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  const real_t density = 2.39e3;        // kg/m3
  const real_t E = 1.85e5;              // Pa
  const real_t shear_modulus = 6.16e4;  // Pa

  elastica::MaterialID backbone_material = elastica::create_material(
      // material density
      density / 5,  // kg/m3, pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      E,  // Pa
      // Shear modulus
      shear_modulus,  // Pa
      // <!-- Properties below don't matter -->
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // coefficient of dynamic friction
      0.1);
  elastica::MaterialID magnetic_rod_material = elastica::create_material(
      // material density
      density,  // kg/m3, pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      E,  // Pa
      // Shear modulus
      shear_modulus,  // Pa
      // <!-- Properties below don't matter -->
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // coefficient of dynamic friction
      0.1);

  const Vec3 direction{0.0, 0.0, 1.0};
  const Vec3 normal{0.0, 1.0, 0.0};
  const Vec3 binormal{1.0, 0.0, 0.0};
  const Vec3& spacing_direction_btw_magnetic_rods_in_one_backbone = binormal;
  const Vec3& spacing_direction_btw_backbone_rods = normal;
  const std::size_t n_magnetic_rod_per_backbone = 6UL;
  const std::size_t n_backbone_rods = 3 * 4 + 1;

  const std::size_t n_elem_magnetic_rod(
      ::elastica::get<n_elements>(input_parameters));
  const real_t base_length_magnetic_rods = 3.0 / 4;  // m
  const real_t base_radius_backbone_rod = 0.15;      // m
  const real_t base_radius_magnetic_rod = 0.15;      // m

  const real_t spacing_btw_magnetic_rods_in_one_backbone =
      base_length_magnetic_rods * 2;  // following Gu2020
  const real_t spacing_btw_backbone_rods = 2 * base_radius_backbone_rod;

  real_t base_length_back_bone = (n_magnetic_rod_per_backbone - 1) *
                                 spacing_btw_magnetic_rods_in_one_backbone;
  const std::size_t n_elem_back_bone =
      base_length_back_bone / (2 * base_radius_backbone_rod);
  base_length_back_bone =
      static_cast<real_t>(n_elem_back_bone * 2.0 * base_radius_backbone_rod);
  const Vec3& normal_back_bone = direction;
  const Vec3& direction_back_bone = binormal;

  blaze::DynamicMatrix<real_t> start_collection_magnetic_rods(
      3UL, n_magnetic_rod_per_backbone);
  /* Create first set of backbones */
  for (std::size_t backbone_idx(0UL); backbone_idx < n_backbone_rods;
       ++backbone_idx) {
    // Compute start collection magnetic rods
    for (std::size_t i(0UL); i < n_magnetic_rod_per_backbone; ++i) {
      blaze::column(start_collection_magnetic_rods, i) =
          (i * spacing_btw_magnetic_rods_in_one_backbone *
               spacing_direction_btw_magnetic_rods_in_one_backbone +
           backbone_idx * spacing_btw_backbone_rods *
               spacing_direction_btw_backbone_rods);
    }

    // Create backbone
    const Vec3 start_back_bone =
        (blaze::column(start_collection_magnetic_rods, 0UL) +
         direction * (base_length_magnetic_rods + base_radius_backbone_rod));
    RodInitializer rod_initializer{
        // Rod Initializer Wrapper
        RodInitializer::NElement{n_elem_back_bone},
        RodInitializer::Material{backbone_material},
        RodInitializer::Radius{base_radius_backbone_rod},
        RodInitializer::Length{base_length_back_bone},
        RodInitializer::Origin{start_back_bone},
        RodInitializer::Direction{direction_back_bone},
        RodInitializer::Normal{normal_back_bone}};
    CRRef back_bone_rod = simulator->emplace_back<CosseratRod>(
        rod_initializer.initialize<CosseratRod>());
    backbone_rod_list.push_back(back_bone_rod);

    // Create magnetic rods
    const real_t base_area = M_PI * std::pow(base_radius_magnetic_rod, 2);
    const real_t volume = base_area * base_length_magnetic_rods;
    const real_t moment_of_inertia =
        M_PI / 4 * std::pow(base_radius_magnetic_rod, 4);

    // Parameters are from Gu2020
    const real_t angular_frequency =
        5.0 * M_PI / 180.0;  // angular frequency of the rotating magnetic field
    const real_t magnetic_field_strength = 80e-3;  //  80mT
    // MBAL2_EI is a non - dimensional number from Wang 2019
    const real_t MBAL2_EI =
        (3.82e-5 * magnetic_field_strength * 4e-3 /
         (1.85e5 * M_PI / 4 *
          std::pow(0.4e-3, 4)));  // Magnetization magnitude * B * Length/(EI)
    const real_t magnetization_density =
        (MBAL2_EI * E * moment_of_inertia /
         (volume * magnetic_field_strength * base_length_magnetic_rods));
    const real_t carpet_length = spacing_btw_magnetic_rods_in_one_backbone *
                                 (n_magnetic_rod_per_backbone - 1);
    const real_t spatial_magnetization_wavelength = carpet_length / 1;
    const real_t spatial_magnetisation_phase_diff = M_PI;
    blaze::DynamicVector<real_t, blaze::rowVector> magnetization_angle =
        spatial_magnetisation_phase_diff +
        (2 * M_PI * blaze::row(start_collection_magnetic_rods, 0UL) /
         spatial_magnetization_wavelength);

    // Magnetic rod
    if (backbone_idx % 4)
      continue;
    // Magnetic rods are not attached to every backbone.We are putting two empty
    //    backbones between every
    // magnetic rod - backbone pair.
    for (std::size_t i(0UL); i < n_magnetic_rod_per_backbone; ++i) {
      const Vec3 magnetization_direction{std::sin(magnetization_angle[i]), 0.0,
                                         std::cos(magnetization_angle[i])};

      // Connect magnetic rod to element of backbone
      blaze::DynamicMatrix<real_t> backbone_element_position =
          Traits::Operations::average_kernel(
              back_bone_rod.get().get_position());

      // clang-format off
      const std::size_t backbone_connection_idx =
          (i != 0) ?
            static_cast<std::size_t>(n_elem_back_bone /
            (n_magnetic_rod_per_backbone - 1)) * i - 1
            :
            0;
      // clang-format on

      Vec3 start_magnetic_rod =
          blaze::column(backbone_element_position, backbone_connection_idx) -
          blaze::dot(
              blaze::column(backbone_element_position, backbone_connection_idx),
              direction) *
              direction;

      MagneticRodInitializer magnetic_rod_initializer{
          // Magnetic Rod Initializer Wrapper
          MagneticRodInitializer::NElement{n_elem_magnetic_rod},
          MagneticRodInitializer::Material{magnetic_rod_material},
          MagneticRodInitializer::Radius{base_radius_magnetic_rod},
          MagneticRodInitializer::Length{base_length_magnetic_rods},
          MagneticRodInitializer::Origin{start_magnetic_rod},
          MagneticRodInitializer::Direction{direction},
          MagneticRodInitializer::Normal{normal},
          MagneticRodInitializer::MagnetizationDensity{magnetization_density},
          MagneticRodInitializer::UniformMagnetizationDirectionInWorldFrame{
              magnetization_direction}};
      MCRRef magnetic_rod = simulator->emplace_back<MagnetizedCosseratRod>(
          magnetic_rod_initializer.initialize<MagnetizedCosseratRod>());
      magnetic_rod_list.push_back(magnetic_rod);
    }
  }

  /* Create second set of backbones */
  const Vec3& direction_back_bone_second_layer = normal;
  const Vec3& normal_back_bone_second_layer = direction_back_bone;
  const std::size_t n_elem_back_bone_second_layer = backbone_rod_list.size();
  for (std::size_t i(0UL); i < n_elem_back_bone; ++i) {
    const Vec3 start_back_bone_second_layer =
        (0.5 * (backbone_rod_list[0UL].get().get_position(i) +
                backbone_rod_list[0UL].get().get_position(i + 1)) +
         direction * base_radius_backbone_rod * 2 -
         direction_back_bone_second_layer * base_radius_backbone_rod);
    const real_t back_bone_length =
        (blaze::norm(backbone_rod_list.back().get().get_position(i) -
                     backbone_rod_list.front().get().get_position(i)) +
         2 * base_radius_backbone_rod);

    RodInitializer rod_initializer{
        // Rod Initializer Wrapper
        RodInitializer::NElement{n_elem_back_bone_second_layer},
        RodInitializer::Material{backbone_material},
        RodInitializer::Radius{base_radius_backbone_rod},
        RodInitializer::Length{base_length_back_bone},
        RodInitializer::Origin{start_back_bone_second_layer},
        RodInitializer::Direction{direction_back_bone_second_layer},
        RodInitializer::Normal{normal_back_bone_second_layer}};
    CRRef back_bone_rod = simulator->emplace_back<CosseratRod>(
        rod_initializer.initialize<CosseratRod>());
    backbone_rod_second_layer_list.push_back(back_bone_rod);
  }

  /* Connection: Connect magnetic rods with their backbone */
  // Orthogonal Connection
  for (auto const& back_bone_rod_ref : backbone_rod_list){
    auto back_bone_rod = back_bone_rod_ref.get();
    blaze::DynamicMatrix<real_t> back_bone_rod_element_position =
        Traits::Operations::average_kernel(back_bone_rod.get_position());
    const std::size_t magnetic_rod_connection_index = n_elem_magnetic_rod - 1;

    for (auto const& magnetic_rod_ref : magnetic_rod_list){
      auto magnetic_rod = magnetic_rod_ref.get();
      const Vec3 magnetic_rod_tip_element_position = blaze::column(
          Traits::Operations::average_kernel(magnetic_rod.get_position()),
          magnetic_rod_connection_index);

      auto distance_btw_rods = Traits::Operations::batch_norm(
          back_bone_rod_element_position -
          blaze::expand(magnetic_rod_tip_element_position,
                        back_bone_rod_element_position.columns()));
      blaze::DynamicVector<real_t> magnetic_rod_rest_length =
          ::elastica::get<::elastica::tags::ReferenceElementLength>(
              magnetic_rod);
      distance_btw_rods -= *(magnetic_rod_rest_length.end() - 1) / 2 +
                           back_bone_rod.get_element_dimension(0UL);

      // In this case rods are far from each other so no need for connection.
      if (blaze::min(distance_btw_rods) > 1e-8) continue;

      const std::size_t back_bone_rod_connection_index =
          blaze::argmin(distance_btw_rods);

      /* TODO: Orthogonal connection #316 */
//      (
//          rod_one_direction_vec_in_material_frame,
//              rod_two_direction_vec_in_material_frame,
//              offset_btw_rods,
//      ) = get_connection_vector_for_perpendicular_rods(
//          back_bone_rod,
//          magnetic_rod,
//          rod_one_index=back_bone_rod_connection_index,
//          rod_two_index=magnetic_rod_connection_index,  # Since we are connecting at its tip we know the index
//      )
//      magnetic_decapot_simulator.connect(
//          first_rod=back_bone_rod,
//          second_rod=magnetic_rod,
//          first_connect_idx=back_bone_rod_connection_index,
//          second_connect_idx=magnetic_rod_connection_index,
//      ).using(
//          PerpendicularRodsConnection,
//              k=1e6 / 10,  # * 10,
//          nu=0.1,
//          k_repulsive=1e4,
//          kt=1e4,  # * 10 * 10,
//          rod_one_direction_vec_in_material_frame=rod_one_direction_vec_in_material_frame,
//          rod_two_direction_vec_in_material_frame=rod_two_direction_vec_in_material_frame,
//          offset_btw_rods=offset_btw_rods,
//      )
    }
  }

  /* Connection: Connect backbones (first layer) */
  // Parallel Connection

  /* Connection: Connect backbones (second layer) */
  // Parallel Connection

  /* Connection: Connect first layer and second layer */

  // ************************************************** //

  auto& magnetic_field =
      simulator
          ->get_environment<::elastica::environments::tags::MagneticFieldTag>();

  // The field profile was set in the hpp file, can be modified if needed here
  const auto in_plane_amplitude =
      ::elastica::get<magnetic_field_in_plane_amplitude>(input_parameters);
  const auto out_of_plane_amplitude =
      ::elastica::get<magnetic_field_out_of_plane_amplitude>(input_parameters);
  const auto frequency =
      ::elastica::get<magnetic_field_angular_frequency>(input_parameters);

  magnetic_field->profile().field_amplitude() = ::elastica::Vec3{
      in_plane_amplitude, out_of_plane_amplitude, in_plane_amplitude};
  magnetic_field->profile().field_angular_frequency() =
      ::elastica::Vec3{frequency, 0.0, frequency};
  magnetic_field->profile().field_phase_difference() =
      ::elastica::Vec3{0.0, M_PI_2, M_PI_2};

  // can be parametrized if needed
  magnetic_field->profile().rampup_profile().start_time() = 0.0;
  magnetic_field->profile().rampup_profile().interval() =
      ::elastica::get<rampup_interval>(input_parameters);

  final_time = ::elastica::get<final_sim_time>(input_parameters);

  const real_t dt(::elastica::get<dt>(input_parameters));

  MagneticMillipedesCaseStudy::add_analytical_damper(input_parameters, dt);

  simulator->finalize();

  // Set the dt value
  set_dt(dt);
}

void MagneticMillipedesCaseStudy::add_analytical_damper(
    InputTypes<This> const& input_parameters, const real_t& dt) {
  // Since all rods are same we create this once and reuse for all rods
  // ===========ANALYTICAL DAMPING CODE START============================
  const real_t damping_coeff =
      ::elastica::get<damping_constant>(input_parameters);
  const real_t translational_mode_damping_coeff =
      blaze::exp(-damping_coeff * dt);

  auto rod = rod_refs[0UL].get();
  auto& mass = ::elastica::get<elastica::tags::Mass>(rod);
  blaze::DynamicVector<real_t> elemental_mass =
      Traits::Operations::average_kernel(mass);
  // correct elemental mass at ends
  elemental_mass[0UL] += 0.5 * mass[0UL];
  elemental_mass[::elastica::cosserat_rod::get_batch_elems(elemental_mass) -
                 1UL] +=
      0.5 * mass[::elastica::cosserat_rod::get_batch_elems(mass) - 1UL];
  blaze::DynamicMatrix<real_t> rotational_mode_damping_coeff = blaze::exp(
      Traits::Operations::transpose(Traits::Operations::expand_for_broadcast(
          elemental_mass * -damping_coeff * dt)));
  constexpr std::size_t dim{3UL};
  for (std::size_t row_idx = 0; row_idx < dim; row_idx++) {
    for (std::size_t column_idx = 0;
         column_idx < ::elastica::cosserat_rod::get_batch_elems(elemental_mass);
         column_idx++) {
      rotational_mode_damping_coeff(row_idx, column_idx) = std::pow(
          rotational_mode_damping_coeff(row_idx, column_idx),
          ::elastica::get<::elastica::tags::InvMassSecondMomentOfInertia>(rod)(
              row_idx, row_idx, column_idx));
    }
  }
  // setting analytical damper via constraint
  namespace eco = ::elastica::constraints;
  auto analytical_linear_damper = eco::make_constraint(
      eco::make_value_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {}),
      eco::make_rate_constraint(
          [translational_mode_damping_coeff, rotational_mode_damping_coeff](
              auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system) =
                ::elastica::velocity(rod_system) *
                translational_mode_damping_coeff;
            ::elastica::angular_velocity(rod_system) =
                ::elastica::angular_velocity(rod_system) %
                rotational_mode_damping_coeff;
          }));
  // ===========ANALYTICAL DAMPING CODE END============================
  for (auto& rod_ref : rod_refs) {
    simulator->constrain(rod_ref).at(0UL).with(analytical_linear_damper());
  }
}

void MagneticMillipedesCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void MagneticMillipedesCaseStudy::run() {
  enable_floating_point_exceptions();

  const std::size_t n_steps = (final_time / current_dt());

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();
  }
}
