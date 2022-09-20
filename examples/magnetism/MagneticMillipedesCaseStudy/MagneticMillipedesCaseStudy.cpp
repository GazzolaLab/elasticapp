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

#include <cmath>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Systems/CosseratRods/Utility.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Size.hpp>
#include <elastica/Utilities/TMPL.hpp>

#include "CustomConnection.cpp"

void MagneticMillipedesCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  const real_t dt(::elastica::get<dt>(input_parameters));

  const real_t density = 2.39e3;        // kg/m3
  const real_t E = 1.85e5;              // Pa
  const real_t shear_modulus = 6.16e4;  // Pa

  // Parameters are from Gu2020
  const real_t magnetic_field_strength = 80e-3;  //  80mT
  // MBAL2_EI is a non - dimensional number from Wang 2019
  const real_t MBAL2_EI =
      (3.82e-5 * magnetic_field_strength * 4e-3 /
       (1.85e5 * M_PI / 4 *
        std::pow(0.4e-3, 4)));  // Magnetization magnitude * B * Length/(EI)

  elastica::MaterialID wall_material = elastica::create_material(
      // material density
      8.94e-3,  // old elastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      E,
      // Shear modulus
      shear_modulus,
      // Properties below don't matter
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping_normal>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping_tangential>(input_parameters),
      // coefficient of static friction
      0.4,
      // coefficient of dynamic friction
      0.2);
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

  /* Connection: magnetic arm to backbones */
  // Orthogonal Connection
  MagneticMillipedesCaseStudy::orthogonal_connection(backbone_rod_list,
                                                     magnetic_rod_list);

  /* Connection: Connect backbones */
  // Parallel Connection
  MagneticMillipedesCaseStudy::group_parallel_connection(backbone_rod_list);
  MagneticMillipedesCaseStudy::group_parallel_connection(
      backbone_rod_second_layer_list);

  /* Connection: Connect first layer and second layer */
  MagneticMillipedesCaseStudy::crisscross_connection(
      backbone_rod_list, backbone_rod_second_layer_list);

  // ************************************************** //
  /* Damper: Analytical Damper */

  MagneticMillipedesCaseStudy::add_analytical_damper(backbone_rod_list,
                                                     input_parameters, dt);
  MagneticMillipedesCaseStudy::add_analytical_damper(
      backbone_rod_second_layer_list, input_parameters, dt);
  MagneticMillipedesCaseStudy::add_analytical_damper(magnetic_rod_list,
                                                     input_parameters, dt);

  // ************************************************** //

  /* Field: Magnetic field object */
  auto& magnetic_field =
      simulator
          ->get_environment<::elastica::environments::tags::MagneticFieldTag>();

  // The field profile was set in the hpp file, can be modified if needed here
  const real_t in_plane_amplitude = magnetic_field_strength / 2.0;
  const real_t out_of_plane_amplitude = magnetic_field_strength * 1e-2 / 2.0;
  const real_t angular_frequency =
      5.0 * M_PI / 180.0;  // angular frequency of the rotating magnetic field

  magnetic_field->profile().field_amplitude() = ::elastica::Vec3{
      in_plane_amplitude, out_of_plane_amplitude, in_plane_amplitude};
  magnetic_field->profile().field_angular_frequency() =
      ::elastica::Vec3{angular_frequency, 0.0, angular_frequency};
  magnetic_field->profile().field_phase_difference() =
      ::elastica::Vec3{0.0, M_PI_2, M_PI_2};

  // can be parametrized if needed
  magnetic_field->profile().rampup_profile().start_time() = 0.0;
  magnetic_field->profile().rampup_profile().interval() = 0.01;

  // ************************************************** //
  /* Half space: Bottom Plane */
  // Set the bottom plane at (0, 0, 0) and with normal along Z+
  const ::elastica::Vec3 plane_position_bottom{0.0, 0.0, 0.0};
  auto plane_normal_bottom = {0.0, 0.0, 1.0};
  auto plane_binormal_bottom = {0.0, 1.0, 0.0};
  auto plane_tangent_bottom = {-1.0, 0.0, 0.0};
  // implicit convention
  ::elastica::Rot3 Q_bottom = {plane_normal_bottom, plane_binormal_bottom,
                               plane_tangent_bottom};
  imposed_half_space_bottom_ref = simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          0UL, 0UL, true, plane_position_bottom, Q_bottom, wall_material));

  // ************************************************** //
  /* Collision */
  // set near-list update time
  //  auto& collision_solver =
  //      simulator
  //          ->get_environment<elastica::environments::tags::CollisionSystem>();
  //  collision_solver->time_interval_between_detection() =
  //      1000.0 * dt;

  // ************************************************** //
  /* Gravity */
  const ::elastica::real_t gravity_acc(-9.81 / 10.0);
  const ::elastica::Vec3 gravity_acc_vector{0.0, 0.0, gravity_acc};
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  // ************************************************** //

  // Finalize simulation configuration
  final_time = ::elastica::get<final_sim_time>(input_parameters);
  simulator->finalize();

  // Set the dt value
  set_dt(dt);

  // Thread-safe version
  auto& container = simulator->get_system_collection_container();
  auto const block_count = cpp17::apply(
      [](auto&... cont) {
        std::size_t block_count(0UL);
        EXPAND_PACK_LEFT_TO_RIGHT((block_count += cont.size()));
        return block_count;
      },
      container);
  std::cout << "Block Count: " << block_count << '\n';
}

template <typename RodTypeOne, typename RodTypeTwo>
void MagneticMillipedesCaseStudy::orthogonal_connection(
    std::vector<RodTypeOne>& rod_list_one,
    std::vector<RodTypeTwo>& rod_list_two) {
  /* connection configuration */
  const real_t k = 1e5;
  const real_t nu = 0.1;
  const real_t k_repulsive = 1e4;
  const real_t kt = 1e4;

  /* Connection: Connect magnetic rods with their backbone */
  // Orthogonal Connection
  for (auto const& back_bone_rod_ref : rod_list_one) {
    auto back_bone_rod = back_bone_rod_ref.get();
    blaze::DynamicMatrix<real_t> back_bone_rod_element_position =
        Traits::Operations::average_kernel(back_bone_rod.get_position());

    for (auto const& magnetic_rod_ref : rod_list_two) {
      auto magnetic_rod = magnetic_rod_ref.get();
      const std::size_t magnetic_rod_connection_index = magnetic_rod.size() - 1;
      const Vec3 magnetic_rod_tip_element_position = blaze::column(
          Traits::Operations::average_kernel(magnetic_rod.get_position()),
          magnetic_rod_connection_index);

      blaze::DynamicVector<real_t> distance_btw_rods =
          Traits::Operations::batch_norm(
              back_bone_rod_element_position -
              blaze::expand(magnetic_rod_tip_element_position,
                            back_bone_rod_element_position.columns()));
      blaze::DynamicVector<real_t> magnetic_rod_rest_length =
          ::elastica::get<::elastica::tags::ReferenceElementLength>(
              magnetic_rod);
      distance_btw_rods -=
          magnetic_rod_rest_length[magnetic_rod.size() - 1] / 2 +
          back_bone_rod.get_element_dimension(0UL);

      // In this case rods are far from each other so no need for connection.
      if (blaze::min(distance_btw_rods) > 1e-8)
        continue;

      const std::size_t back_bone_rod_connection_index =
          blaze::argmin(distance_btw_rods);

      Vec3 rod_one_direction_vec_in_material_frame;
      Vec3 rod_two_direction_vec_in_material_frame;
      real_t offset_btw_rods;
      std::tie(rod_one_direction_vec_in_material_frame,
               rod_two_direction_vec_in_material_frame, offset_btw_rods) =
          OrthogonalConnection::get_connection_vector_for_perpendicular_rods(
              back_bone_rod, magnetic_rod, back_bone_rod_connection_index,
              magnetic_rod_connection_index);

      auto connection_generator =
          OrthogonalConnection::perpendicular_connection_generator(
              k, nu, k_repulsive, kt, rod_one_direction_vec_in_material_frame,
              rod_two_direction_vec_in_material_frame, offset_btw_rods);
      simulator->connect(back_bone_rod_ref, at(back_bone_rod_connection_index))
          .to(magnetic_rod_ref, at(magnetic_rod_connection_index))
          .with(connection_generator());
    }
  }
}

template <typename RodTypeOne, typename RodTypeTwo>
void MagneticMillipedesCaseStudy::crisscross_connection(
    std::vector<RodTypeOne>& rod_list_one,
    std::vector<RodTypeTwo>& rod_list_two) {
  /* connection configuration */
  const real_t k = 1e5;
  const real_t nu = 0.1;
  const real_t k_repulsive = 1e6;

  for (std::size_t rod_one_idx(0UL); rod_one_idx < rod_list_one.size();
       ++rod_one_idx)
    for (std::size_t rod_two_idx(0UL); rod_two_idx < rod_list_two.size();
         ++rod_two_idx) {
      auto rod_one = rod_list_one[rod_one_idx].get();
      auto rod_two = rod_list_two[rod_two_idx].get();
      for (std::size_t i(0UL); i < rod_two.size(); ++i) {
        const std::size_t rod_one_elem_idx = rod_two_idx;
        const std::size_t rod_two_elem_idx = i;

        blaze::DynamicMatrix<real_t> rod_one_direction_vec_in_material_frame(
            3UL, 1UL);
        blaze::DynamicMatrix<real_t> rod_two_direction_vec_in_material_frame(
            3UL, 1UL);
        blaze::DynamicVector<real_t> offset_btw_rods(1UL);
        std::tie(rod_one_direction_vec_in_material_frame,
                 rod_two_direction_vec_in_material_frame, offset_btw_rods) =
            ParallelConnection::get_connection_vector_straight_straight_rod(
                rod_one, rod_two, rod_one_elem_idx, rod_one_elem_idx + 1,
                rod_two_elem_idx, rod_two_elem_idx + 1);

        if (blaze::max(offset_btw_rods) > 1e-13)
          continue;

        auto connection_generator =
            ParallelConnection::parallel_connection_generator(
                k, nu, k_repulsive,
                blaze::column(rod_one_direction_vec_in_material_frame, 0),
                blaze::column(rod_two_direction_vec_in_material_frame, 0),
                offset_btw_rods[0]);
        simulator->connect(rod_list_one[rod_one_idx], at(rod_two_idx))
            .to(rod_list_two[rod_two_idx], at(i))
            .with(connection_generator());
      }
    }
}

template <typename RodType>
void MagneticMillipedesCaseStudy::group_parallel_connection(
    std::vector<RodType>& rod_list) {
  const std::size_t size = rod_list.size();
  /* connection configuration */
  const real_t k = 1e5;
  const real_t nu = 0.1;
  const real_t k_repulsive = 1e6;

  for (std::size_t rod_one_idx(0UL); rod_one_idx < size; ++rod_one_idx)
    for (std::size_t rod_two_idx(rod_one_idx + 1); rod_two_idx < size;
         ++rod_two_idx) {
      auto rod_one = rod_list[rod_one_idx].get();
      auto rod_two = rod_list[rod_two_idx].get();

      // Backbone rods do not have same number of elements
      assert(rod_one.size() != rod_two.size());

      const std::size_t n_elem = rod_one.size();
      blaze::DynamicMatrix<real_t> rod_one_direction_vec_in_material_frame(
          3UL, n_elem);
      blaze::DynamicMatrix<real_t> rod_two_direction_vec_in_material_frame(
          3UL, n_elem);
      blaze::DynamicVector<real_t> offset_btw_rods(n_elem);
      std::tie(rod_one_direction_vec_in_material_frame,
               rod_two_direction_vec_in_material_frame, offset_btw_rods) =
          ParallelConnection::get_connection_vector_straight_straight_rod(
              rod_one, rod_two, 0, n_elem, 0, n_elem);

      // spacing_btw_magnetic_rods_in_one_backbone - 2 * rod_one.radius[0]:#
      // 1e-8: In this case rods are far from each other so no need for
      // connection.
      if (blaze::max(offset_btw_rods) > 1e-8)
        continue;

      for (std::size_t e(0UL); e < n_elem; ++e) {
        auto connection_generator =
            ParallelConnection::parallel_connection_generator(
                k, nu, k_repulsive,
                blaze::column(rod_one_direction_vec_in_material_frame, e),
                blaze::column(rod_two_direction_vec_in_material_frame, e),
                offset_btw_rods[e]);
        simulator->connect(rod_list[rod_one_idx], at(e))
            .to(rod_list[rod_two_idx], at(e))
            .with(connection_generator());
      }
    }
}

template <typename RodType>
void MagneticMillipedesCaseStudy::add_analytical_damper(
    std::vector<RodType>& rod_list, InputTypes<This> const& input_parameters,
    const real_t& dt) {
  // Since all rods are same we create this once and reuse for all rods
  // ===========ANALYTICAL DAMPING CODE START============================
  const real_t damping_coeff =
      ::elastica::get<damping_constant>(input_parameters);
  const real_t translational_mode_damping_coeff =
      blaze::exp(-damping_coeff * dt);

  auto rod = rod_list[0UL].get();
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
          [](auto /*time*/, auto& /*rod_system*/, std::size_t /*index*/) {}),
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
  for (auto& rod_ref : rod_list)
    simulator->constrain(rod_ref).at(0UL).with(analytical_linear_damper());
}

void MagneticMillipedesCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void MagneticMillipedesCaseStudy::run() {
  enable_floating_point_exceptions();

  // Logging Configuration
  const ::elastica::real_t fps = 60;
  const std::size_t n_steps = (final_time / current_dt());
  const std::size_t write_interval =
      n_steps / (final_time * fps);  // 1000000UL;

  float current_progress = 0.0;
  auto stime = std::chrono::system_clock::now();

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    // Progress Bar
    float progress = float(i) / float(n_steps);
    progress_bar(progress, current_progress, stime);

    // Logging
    if (not(i % write_interval)) {
      log_rod_position(backbone_rod_list, i,
                       "data/backbone_rod_coordinates_%010d.csv");
      log_rod_position(backbone_rod_second_layer_list, i,
                       "data/backbone_rod_second_layer_coordinates_%010d.csv");
      log_rod_position(magnetic_rod_list, i,
                       "data/magnetic_rod_coordinates_%010d.csv");
      log_penetration(i);
    }

    // Iterate
    step();
  }
}

inline void MagneticMillipedesCaseStudy::progress_bar(
    float progress, float& current_progress,
    std::chrono::time_point<std::chrono::system_clock> start_time) {
  constexpr float progress_step =
      0.001;                     // Update estimated time every 0.001 progress
  constexpr int bar_width = 70;  // Progress bar width
  constexpr float min_progress =
      0.0001;  // Display after 0.0001 progress (to avoid divide by zero)

  std::cout << std::fixed << std::setprecision(1);
  auto current_time = std::chrono::system_clock::now();
  if (current_progress <= progress && progress > min_progress) {
    std::cout << "[";
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
      if (i < pos)
        std::cout << "=";
      else if (i == pos)
        std::cout << ">";
      else
        std::cout << " ";
    }
    std::chrono::duration<int> elapsed_time =
        std::chrono::duration_cast<std::chrono::seconds>(current_time -
                                                         start_time);
    std::time_t etd = std::chrono::system_clock::to_time_t(
        start_time +
        std::chrono::seconds(int(float(elapsed_time.count()) / progress)));
    std::cout << "]" << progress * 100.0 << "%"
              << " [elapsed: " << elapsed_time.count() << " sec]"
              << " [etd: " << std::strtok(std::ctime(&etd), "\n") << "]\r";
    std::cout.flush();
    current_progress += progress_step;
  }
}

template <typename RodRefType>
inline void MagneticMillipedesCaseStudy::log_rod_position(
    std::vector<RodRefType>& rod_refs, const int i, const char* tag) {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  using RodCoordinates =
      std::tuple<::elastica::real_t, ::elastica::real_t, ::elastica::real_t>;
  std::vector<RodCoordinates> rod_coordinates;

  //  explicitly get the system
  constexpr auto x_axis = 0UL;
  constexpr auto y_axis = 1UL;
  constexpr auto z_axis = 2UL;
  const auto n_nodes = ::elastica::size(rod_refs[0].get()) + 1UL;
  const auto n_rod = rod_refs.size();

  rod_coordinates.reserve(n_rod * n_nodes);

  char file_name[100];
  std::ofstream temporal_file;

  // record rod position
  for (auto const& rod_ref : rod_refs) {
    for (std::size_t k = 0; k < n_nodes; ++k) {
      rod_coordinates.emplace_back(
          ::elastica::position(rod_ref.get(), k)[x_axis],
          ::elastica::position(rod_ref.get(), k)[y_axis],
          ::elastica::position(rod_ref.get(), k)[z_axis]);
    }
  }
  // TODO : replace with a csv writer
  sprintf(file_name, tag, i);
  temporal_file.open(file_name);
  for (auto const& rod_coord : rod_coordinates) {
    temporal_file << std::setprecision(16) << std::get<x_axis>(rod_coord) << ","
                  << std::get<y_axis>(rod_coord) << ","
                  << std::get<z_axis>(rod_coord) << "\n";
  }
  temporal_file << std::endl;
  temporal_file.close();
  rod_coordinates.clear();
}

inline void MagneticMillipedesCaseStudy::log_penetration(const int i) {
  // Record Interpenetration
  auto& collision_system =
      simulator
          ->get_environment<elastica::environments::tags::CollisionSystem>();
  auto& contacts = collision_system->contacts();
  using Penetrations = std::vector<elastica::real_t>;
  Penetrations penetrations;
  penetrations.reserve(contacts.size());

  char file_name[100];
  std::ofstream temporal_file;

  std::transform(std::cbegin(contacts), std::cend(contacts),
                 std::back_inserter(penetrations),
                 [](auto& c) { return -c.get_distance(); });
  sprintf(file_name, "data/penetrations_%010d.csv", i);
  temporal_file.open(file_name);
  for (auto const& depth : penetrations) {
    temporal_file << std::setprecision(16) << depth << '\n';
  }
  temporal_file.close();
  penetrations.clear();
}
