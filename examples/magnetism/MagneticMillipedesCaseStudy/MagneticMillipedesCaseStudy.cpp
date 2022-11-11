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
#include <elastica/Utilities/StdHelpers.hpp>
#include <elastica/Utilities/TMPL.hpp>

#include "CustomConnection.hpp"
#include "IsotropicFrictionPlane.hpp"

void MagneticMillipedesCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  const real_t dt(::elastica::get<dt>(input_parameters));

  const real_t density = 2.39e3;        // kg/m3
  const real_t E = 1.85e5;              // Pa
  const real_t shear_modulus = 6.16e4;  // Pa

  // Parameters are from Gu2020
  const real_t magnetic_field_strength =
      ::elastica::get<mag_strength>(input_parameters);  //  80mT
  // MBAL2_EI is a non - dimensional number from Wang 2019
  const real_t MBAL2_EI =
      (3.82e-5 * magnetic_field_strength * 4e-3 /
       (1.85e5 * M_PI / 4 *
        std::pow(0.4e-3, 4)));  // Magnetization magnitude * B * Length/(EI)

  const ::elastica::real_t gravity_acc(
      ::elastica::get<acc_gravity>(input_parameters));
  // Pyelastica values seem unnaturally high.
  const ::elastica::real_t friction_coefficient(
      ::elastica::get<friction_coeff>(input_parameters));

#define CRISS_RODS
#define CROSS_RODS

  elastica::MaterialID wall_material = elastica::create_material(
      // material density
      8.94e3,  // old elastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      E,
      // Shear modulus
      shear_modulus,
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping_normal>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping_tangential>(input_parameters),
      // coefficient of static friction
      1.5 * friction_coefficient,
      // coefficient of dynamic friction
      friction_coefficient);
  elastica::MaterialID backbone_material = elastica::create_material(
      // material density
      density / ::elastica::get<density_magnetic_to_backbone>(
                    input_parameters),  // kg/m3, pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      E / ::elastica::get<stiffness_magnetic_to_backbone>(
              input_parameters),  // Pa
      // Shear modulus
      shear_modulus,  // Pa
      // <!-- Properties below don't matter -->
      // Contact stiffness, intentionally small so that the ball does not move
      1e4,
      // normal contact damping
      10.0,
      // tangent contact damping
      10.0,
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
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping_normal>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping_tangential>(input_parameters),
      // coefficient of static friction
      1.5 * friction_coefficient,
      // coefficient of dynamic friction
      friction_coefficient);

#if SPHERES_INCLUDED
  elastica::MaterialID sphere_material = elastica::create_material(
      // material density
      density * ::elastica::get<density_sphere_to_magnetic>(
                    input_parameters),  // kg/m3, pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      E,  // Pa
      // Shear modulus
      shear_modulus,  // Pa
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping_normal>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping_tangential>(input_parameters),
      // coefficient of static friction
      1.5 * friction_coefficient,
      // coefficient of dynamic friction
      friction_coefficient);
#endif

  const Vec3 normal{0.0, 1.0, 0.0};
  const Vec3 binormal{-1.0, 0.0, 0.0};
  const Vec3 direction{0.0, 0.0, 1.0};

  const Vec3& spacing_direction_btw_magnetic_rods_in_one_backbone = -binormal;
  const Vec3& spacing_direction_btw_backbone_rods = normal;

  const std::size_t n_magnetic_rod_per_backbone(
      ::elastica::get<number_magnetic_rods_per_backbone>(input_parameters));
  const std::size_t n_magnetic_rods_spanwise(
      ::elastica::get<number_magnetic_rods_spanwise>(input_parameters));
  const std::size_t n_backbone_rods_for_every_magnetic_rod(
      ::elastica::get<number_backbone_rods_for_every_magnetic_rod>(
          input_parameters));
  // First and last are alwyas attached
  const std::size_t n_backbone_rods =
      (n_magnetic_rods_spanwise - 1UL) *
          n_backbone_rods_for_every_magnetic_rod +
      1;

  const std::size_t n_elem_magnetic_rod(
      ::elastica::get<n_elements>(input_parameters));
  const real_t base_length_magnetic_rods(
      ::elastica::get<length_magnetic>(input_parameters));  // m
  // TEJA : NOTE this was 0.15 but its length is 0.75. The ratio 0.75 /0.15 = 5
  // is not slender enough to work with our collision system, so I had to change
  // these values.
  const real_t base_radius_backbone_rod =
      ::elastica::get<radius_backbone>(input_parameters);  // m
  const real_t base_radius_magnetic_rod =
      ::elastica::get<radius_magnetic>(input_parameters);  // m

  const real_t spacing_btw_magnetic_rods_in_one_backbone =
      base_length_magnetic_rods * ::elastica::get<magnetic_rod_spacing_ratio>(
                                      input_parameters);  // following Gu2020
  const real_t spacing_btw_backbone_rods = 2 * base_radius_backbone_rod;

  real_t base_length_back_bone = (n_magnetic_rod_per_backbone - 1) *
                                 spacing_btw_magnetic_rods_in_one_backbone;
  auto const n_elem_back_bone = static_cast<std::size_t>(
      base_length_back_bone / (2.0 * base_radius_backbone_rod));
  base_length_back_bone =
      static_cast<real_t>(n_elem_back_bone) * 2.0 * base_radius_backbone_rod;

  const Vec3& normal_back_bone = direction;
  const Vec3& direction_back_bone =
      spacing_direction_btw_magnetic_rods_in_one_backbone;

  blaze::DynamicMatrix<real_t> start_collection_magnetic_rods(
      3UL, n_magnetic_rod_per_backbone);
  /* Create first set of backbones */
  for (std::size_t backbone_idx(0UL); backbone_idx < n_backbone_rods;
       ++backbone_idx) {
    // Compute start collection magnetic rods
    for (std::size_t i(0UL); i < n_magnetic_rod_per_backbone; ++i) {
      blaze::column(start_collection_magnetic_rods, i) =
          (real_t(i) * spacing_btw_magnetic_rods_in_one_backbone *
               spacing_direction_btw_magnetic_rods_in_one_backbone +
           real_t(backbone_idx) * spacing_btw_backbone_rods *
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
#if defined(CRISS_RODS)
    auto back_bone_rod = simulator->emplace_back<BackBoneCrissRod>(
        rod_initializer.initialize<BackBoneCrissRod>());
    backbone_criss_rods.push_back(back_bone_rod);
#endif

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
    if (backbone_idx % n_backbone_rods_for_every_magnetic_rod)
      continue;

    auto access = [&](auto t) {
      return std::get<tmpl::type_from<decltype(t)>>(rod_initializer.data())
          .value();
    };

    // Connect magnetic rod to element of backbone
    auto backbone_element_position =
        [lg = ::elastica::linspace_generator(
             access(tmpl::type_<typename RodInitializer::Origin>{}),
             elastica::Vec3{
                 access(tmpl::type_<typename RodInitializer::Origin>{}) +
                 access(tmpl::type_<typename RodInitializer::Length>{}) *
                     access(tmpl::type_<typename RodInitializer::Direction>{})},
             access(tmpl::type_<typename RodInitializer::NElement>{}) + 1UL)](
            std::size_t backbone_connection_idx) {
          return elastica::Vec3{0.5 * (lg(backbone_connection_idx) +
                                       lg(backbone_connection_idx + 1))};
        };

    // blaze::DynamicMatrix<real_t> backbone_element_position =
    //     Traits::Operations::average_kernel(
    //         back_bone_rod.get().get_position());

    // Magnetic rods are not attached to every backbone.We are putting two empty
    //    backbones between every
    // magnetic rod - backbone pair.
    for (std::size_t i(0UL); i < n_magnetic_rod_per_backbone; ++i) {
      const Vec3 magnetization_direction{std::sin(magnetization_angle[i]), 0.0,
                                         std::cos(magnetization_angle[i])};

      // clang-format off
      const std::size_t backbone_connection_idx =
          (i != 0) ?
            static_cast<std::size_t>(n_elem_back_bone /
            (n_magnetic_rod_per_backbone - 1)) * i - 1
            :
            0;
      // clang-format on

      Vec3 start_magnetic_rod =
          backbone_element_position(backbone_connection_idx) -
          blaze::dot(backbone_element_position(backbone_connection_idx),
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
  const Vec3& direction_back_bone_second_layer =
      blaze::cross(normal_back_bone, direction_back_bone);
  const Vec3& normal_back_bone_second_layer = direction_back_bone;
  const std::size_t n_elem_back_bone_second_layer = backbone_criss_rods.size();
#if defined(CROSS_RODS)
  for (std::size_t i(0UL); i < n_elem_back_bone; ++i) {
    const Vec3 start_back_bone_second_layer =
        (0.5 * (backbone_criss_rods[0UL].get().get_position(i) +
                backbone_criss_rods[0UL].get().get_position(i + 1)) +
         direction * base_radius_backbone_rod * 2 -
         direction_back_bone_second_layer * base_radius_backbone_rod);
    const real_t back_bone_length =
        (blaze::norm(backbone_criss_rods.back().get().get_position(i) -
                     backbone_criss_rods.front().get().get_position(i)) +
         2 * base_radius_backbone_rod);

    RodInitializer rod_initializer{
        // Rod Initializer Wrapper
        RodInitializer::NElement{n_elem_back_bone_second_layer},
        RodInitializer::Material{backbone_material},
        RodInitializer::Radius{base_radius_backbone_rod},
        RodInitializer::Length{back_bone_length},
        RodInitializer::Origin{start_back_bone_second_layer},
        RodInitializer::Direction{direction_back_bone_second_layer},
        RodInitializer::Normal{normal_back_bone_second_layer}};
    auto back_bone_rod = simulator->emplace_back<BackBoneCrossRod>(
        rod_initializer.initialize<BackBoneCrossRod>());
    backbone_cross_rods.push_back(back_bone_rod);
  }
#endif

#if SPHERES_INCLUDED
  {
    auto initialize_sphere = [=](::elastica::Vec3 origin,
                                 ::elastica::real_t rad) {
      return ::blocks::initialize_block<Sphere>(
          ::blocks::initialize<::elastica::tags::Position>(
              [=]() { return origin; }),
          ::blocks::initialize<::elastica::tags::Director>(
              []() { return ::elastica::identity3(); }),
          ::blocks::initialize<::elastica::tags::Radius>([=]() { return rad; }),
          ::blocks::initialize<::elastica::tags::Velocity>([&]() {
            return ::elastica::Vec3{0.0, 0.0, 0.0};
          }),
          ::blocks::initialize<::elastica::tags::AngularVelocity>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Acceleration>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::AngularAcceleration>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Material>(
              [=]() { return sphere_material; }));
    };
    auto const sphere_radius =
        ::elastica::get<sphere_radius_to_length_ratio>(input_parameters) *
        base_length_magnetic_rods;
    auto const sphere_offset =
        ::elastica::get<sphere_offset_to_length_ratio>(input_parameters) *
        base_length_magnetic_rods;

    elastica::Vec3 sphere_origin{
        // spanwise middle
        real_t(n_backbone_rods / 2) * spacing_btw_backbone_rods *
            spacing_direction_btw_backbone_rods +
        // put to end along which the bot moves forward
        (base_length_back_bone + sphere_offset) *
            spacing_direction_btw_magnetic_rods_in_one_backbone +
        // displace up from ground
        sphere_radius * direction};
    spheres_list.push_back(simulator->template emplace_back<Sphere>(
        initialize_sphere(sphere_origin, sphere_radius)));

    // Reduce velocity of sphere explicitly so that there is a damping force
    // acting on it
    namespace eco = ::elastica::constraints;
    auto sphere_constraint = eco::make_constraint(
        eco::make_value_constraint(
            [](auto /*time*/, auto& /*rod_system*/, std::size_t /*index*/) {}),
        eco::make_rate_constraint(
            [translational_damping_coeff = std::exp(-elastica::get<damping_constant_backbone>(input_parameters) * dt)](
                auto /*time*/, auto& rod_system, std::size_t) {
              ::elastica::velocity(rod_system) *=
                  translational_damping_coeff;
            }));
    simulator->constrain(spheres_list.back()).at(0UL).with(sphere_constraint());
  }

#endif

  /* Connection: magnetic arm to backbones */
  // Orthogonal Connection
#if defined(CRISS_RODS)
  MagneticMillipedesCaseStudy::orthogonal_connection(backbone_criss_rods,
                                                     magnetic_rod_list);
#endif

  /* Connection: Connect backbones */
  // Parallel Connection

#if defined(CRISS_RODS)
  MagneticMillipedesCaseStudy::group_parallel_connection(backbone_criss_rods);
#endif

#if defined(CROSS_RODS)
  MagneticMillipedesCaseStudy::group_parallel_connection(backbone_cross_rods);
#endif

#if defined(CRISS_RODS) && defined(CROSS_RODS)
  /* Connection: Connect first layer and second layer */
  MagneticMillipedesCaseStudy::crisscross_connection(backbone_criss_rods,
                                                     backbone_cross_rods);
#endif

  // ************************************************** //
  /* Damper: Analytical Damper */

#if defined(CRISS_RODS)
  MagneticMillipedesCaseStudy::add_analytical_damper(
      backbone_criss_rods,
      elastica::get<damping_constant_backbone>(input_parameters), dt);
#endif

#if defined(CROSS_RODS)
  MagneticMillipedesCaseStudy::add_analytical_damper(
      backbone_cross_rods,
      elastica::get<damping_constant_backbone>(input_parameters), dt);
#endif
  MagneticMillipedesCaseStudy::add_analytical_damper(
      magnetic_rod_list,
      elastica::get<damping_constant_magnetic>(input_parameters), dt);

  // ************************************************** //

#if MAGNETIC_FIELD_ON
  /* Field: Magnetic field object */
  auto& magnetic_field =
      simulator
          ->get_environment<::elastica::environments::tags::MagneticFieldTag>();

  // The field profile was set in the hpp file, can be modified if needed here
  const real_t in_plane_amplitude = magnetic_field_strength / 2.0;
  const real_t out_of_plane_amplitude = magnetic_field_strength * 1e-2 / 2.0;
  const real_t angular_frequency(
      elastica::get<mag_angular_frequency>(input_parameters));
  const real_t field_time_period = 2.0 * M_PI / angular_frequency;

  magnetic_field->profile().field_amplitude() = ::elastica::Vec3{
      in_plane_amplitude, out_of_plane_amplitude, in_plane_amplitude};
  magnetic_field->profile().field_angular_frequency() =
      ::elastica::Vec3{angular_frequency, 0.0, angular_frequency};
  magnetic_field->profile().field_phase_difference() =
      ::elastica::Vec3{0.0, M_PI_2, M_PI_2};

  // can be parametrized if needed
  magnetic_field->profile().rampup_profile().start_time() = -2.0;
  // 0.5 * field_time_period;  // FIXME : revisit
  magnetic_field->profile().rampup_profile().interval() = 0.01;
#endif

  // ************************************************** //
  /* Half space: Bottom Plane */
  // Set the bottom plane at (0, 0, 0) and with normal along Z+
  namespace ek = elastica::kinematics;

  auto always_zero_ = [](::elastica::real_t) noexcept -> ::elastica::real_t {
    return 0.0;
  };

  auto translations_buffer = ek::make_translations(
      ek::translate_along<elastica::EulerianFrame::X>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Y>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Z>(
          // The COM along z axis is not at 0 (the anchor position), but rather
          // inside the halfspace, so we displace it by anchor_to_com.
          ek::mark_position(ImposedHalfSpace::anchor_to_com_n(always_zero_)),
          ek::mark_velocity(always_zero_)));

  const ::elastica::Vec3 plane_position_bottom{0.0, 0.0, 0.0};
  auto plane_normal_bottom = {0.0, 0.0, 1.0};
  auto plane_binormal_bottom = {0.0, 1.0, 0.0};
  auto plane_tangent_bottom = {-1.0, 0.0, 0.0};
  // implicit convention
  ::elastica::Rot3 Q_bottom = {plane_normal_bottom, plane_binormal_bottom,
                               plane_tangent_bottom};
  imposed_half_space_bottom_ref = simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          0UL, 0UL, true, plane_position_bottom, Q_bottom, wall_material),
      std::move(translations_buffer));

  //  std::cout << "Pos" << imposed_half_space_bottom_ref.get().get_position()
  //  << std::endl; std::cout << "Normal" <<
  //  imposed_half_space_bottom_ref.get().get_normal() << std::endl; std::cout
  //  << "aABB" << imposed_half_space_bottom_ref.get().get_AABB() << std::endl;

  // ************************************************** //
  /* Collision */
  //  auto& collision_solver =
  //      simulator
  //          ->get_environment<elastica::environments::tags::CollisionSystem>();
  //  collision_solver->time_interval_between_detection() =
  //      10.0 * dt;

  // ************************************************** //
  /* Gravity */
  const ::elastica::Vec3 gravity_acc_vector{0.0, 0.0, gravity_acc};
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  // ************************************************** //

  namespace efo = ::elastica::forcing;

  auto force_generator = [=]() {
    elastica::real_t const k = 1e3;
    elastica::real_t const nu = 0.1;
    elastica::Vec3 const plane_origin = plane_position_bottom;
    elastica::Vec3 const plane_normal = plane_normal_bottom;
    elastica::real_t const surface_tol = 1e-4;
    elastica::real_t const slip_velocity_tol = 1e-8;
    elastica::real_t period = 1.0;
    elastica::real_t froude = 0.1;
    elastica::real_t const mumu =
        base_length_back_bone / (period * period * (9.80665) * froude) / 40.0;
    elastica::real_t const kinetic_mu = mumu;
    elastica::real_t const static_mu = mumu * 1.5;

    return efo::make_forcing(
        efo::make_force([=](auto, auto& rod_system, std::size_t) {
          calculate_force(rod_system, k, nu, plane_origin, plane_normal,
                          surface_tol, slip_velocity_tol, kinetic_mu,
                          static_mu);
        }),
        efo::make_torque([](auto /*time*/, auto&, std::size_t) {}))();
  };

  for (auto mag_rod : magnetic_rod_list) {
    simulator->force(mag_rod).at(0UL).with(force_generator());
  }

  // Finalize simulation configuration
  final_time = ::elastica::get<final_sim_time>(input_parameters);
  simulator->finalize();

  // Set the dt value
  set_dt(dt);

  // Thread-safe version
  auto& container = simulator->get_system_collection_container();
  auto const block_counts = cpp17::apply(
      [](auto&... cont) { return std::make_tuple(cont.size()...); }, container);
  std::cout << "Block Count: " << block_counts << '\n';

#if BACKBONE_CONNECTIONS
  std::cout << "Connection Count: " << simulator->n_connections() << '\n';
#endif
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

      auto const connection_info =
          OrthogonalConnection::get_connection_vector_for_perpendicular_rods(
              back_bone_rod, magnetic_rod, back_bone_rod_connection_index,
              magnetic_rod_connection_index);

      //      std::cout << "---------------------------------------" <<
      //      std::endl; std::cout << back_bone_rod_connection_index <<
      //      std::endl; std::cout << magnetic_rod_connection_index <<
      //      std::endl; std::cout << rod_one_direction_vec_in_material_frame <<
      //      std::endl; std::cout << rod_two_direction_vec_in_material_frame <<
      //      std::endl; std::cout << offset_btw_rods << std::endl;

      auto connection_generator =
          OrthogonalConnection::perpendicular_connection_generator(
              k, nu, k_repulsive, kt, connection_info);
#if BACKBONE_CONNECTIONS
      simulator->connect(back_bone_rod_ref, at(back_bone_rod_connection_index))
          .to(magnetic_rod_ref, at(magnetic_rod_connection_index))
          .with(connection_generator());
#endif
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
#if BACKBONE_CONNECTIONS
        simulator->connect(rod_list_one[rod_one_idx], at(rod_two_idx))
            .to(rod_list_two[rod_two_idx], at(i))
            .with(connection_generator());
#endif
      }
    }
}

template <typename RodType>
void MagneticMillipedesCaseStudy::group_parallel_connection(
    std::vector<RodType>& rod_list) {
  const std::size_t n_rods = rod_list.size();
  /* connection configuration */
  const real_t k = 1e5;
  const real_t nu = 0.1;
  const real_t k_repulsive = 1e6;

#define LINE_BY_LINE 0

#if LINE_BY_LINE
  for (std::size_t rod_one_idx(0UL); rod_one_idx < n_rods; ++rod_one_idx)
    for (std::size_t rod_two_idx(rod_one_idx + 1); rod_two_idx < n_rods;
         ++rod_two_idx) {
      auto rod_one = rod_list[rod_one_idx].get();
      auto rod_two = rod_list[rod_two_idx].get();

      // Backbone rods do not have same number of elements
      if (rod_one.size() != rod_two.size()) {
        throw std::runtime_error("What size no match");
      }

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

#if BACKBONE_CONNECTIONS
        simulator->connect(rod_list[rod_one_idx], at(e))
            .to(rod_list[rod_two_idx], at(e))
            .with(connection_generator());
#endif
      }
    }
#else
  // Assume single block for now to not lose sanity
  const bool same_block_constraint_satisfied =
      std::all_of(rod_list.cbegin(), rod_list.cend(),
                  [b = rod_list[0].parent()](auto& rod_ref) {
                    return rod_ref.parent() == b;
                  });
  if (not same_block_constraint_satisfied) {
    throw std::runtime_error("Not same block");
  }

  const bool same_size_constraint_satisfied =
      std::all_of(rod_list.cbegin(), rod_list.cend(),
                  [s = cpp17::size(rod_list[0].get())](auto& rod_ref) {
                    return cpp17::size(rod_ref.get()) == s;
                  });
  if (not same_size_constraint_satisfied) {
    throw std::runtime_error("Not same size");
  }

  auto parent_ref = rod_list[0].parent();
  auto v_one_ref = blocks::slice(parent_ref, 0UL, n_rods - 1UL);
  auto v_two_ref = blocks::slice(parent_ref, 1UL, n_rods);

  auto rod_one = v_one_ref.get();
  auto rod_two = v_two_ref.get();

  const std::size_t n_dofs = cpp17::size(rod_one);

  blaze::DynamicMatrix<real_t> rod_one_direction_vec_in_material_frame(3UL,
                                                                       n_dofs);
  blaze::DynamicMatrix<real_t> rod_two_direction_vec_in_material_frame(3UL,
                                                                       n_dofs);
  blaze::DynamicVector<real_t> offset_btw_rods(n_dofs);
  std::tie(rod_one_direction_vec_in_material_frame,
           rod_two_direction_vec_in_material_frame, offset_btw_rods) =
      ParallelConnection::get_connection_vector_straight_straight_rod(
          rod_one, rod_two, 0, n_dofs, 0, n_dofs);

  //  for (std::size_t idx = 0UL; idx < n_dofs; ++idx){
  //    std::cout << blaze::column(rod_one_direction_vec_in_material_frame, idx)
  //    << std::endl; std::cout <<
  //    blaze::column(rod_two_direction_vec_in_material_frame, idx) <<
  //    std::endl; std::cout << offset_btw_rods[idx] << std::endl;
  //  }

  auto connection_generator = ParallelConnection::parallel_connection_generator(
      0, n_dofs, k, nu, k_repulsive, rod_one_direction_vec_in_material_frame,
      rod_two_direction_vec_in_material_frame, offset_btw_rods);
#if BACKBONE_CONNECTIONS
  simulator->connect(v_one_ref)
      .to(v_two_ref)
      .with(connection_generator())
      .at(elastica::interface::pair(elastica::defer, elastica::defer));
#endif

#endif
}

template <typename RodType>
void MagneticMillipedesCaseStudy::add_analytical_damper(
    std::vector<RodType>& rod_list, real_t damping_coeff, const real_t& dt) {
  // setting analytical damper via constraint
  namespace eco = ::elastica::constraints;

  // Since all rods are same we create this once and reuse for all rods
  // ===========ANALYTICAL DAMPING CODE START============================
  auto analytical_linear_damper = [=](auto rod) {
    const real_t translational_mode_damping_coeff =
        blaze::exp(-damping_coeff * dt);
    constexpr std::size_t dim{3UL};
    const auto n_elem(cpp17::size(rod));

    auto&& mass = ::elastica::get<elastica::tags::Mass>(rod);
    blaze::DynamicVector<real_t> elemental_mass =
        Traits::Operations::average_kernel(mass);
    // correct elemental mass at ends
    elemental_mass[0UL] += 0.5 * mass[0UL];
    elemental_mass[n_elem - 1UL] += 0.5 * mass[n_elem];

    blaze::DynamicMatrix<real_t> rotational_mode_damping_coeff(dim, n_elem);

    for (std::size_t row_idx = 0; row_idx < dim; row_idx++) {
      for (std::size_t column_idx = 0; column_idx < n_elem; column_idx++) {
        rotational_mode_damping_coeff(row_idx, column_idx) = std::exp(
            -damping_coeff * dt * elemental_mass[column_idx] *
            ::elastica::get<::elastica::tags::InvMassSecondMomentOfInertia>(
                rod)(row_idx, column_idx));
      }
    }

    return eco::make_constraint(
        eco::make_value_constraint(
            [](auto /*time*/, auto& /*rod_system*/, std::size_t /*index*/) {}),
        eco::make_rate_constraint([translational_mode_damping_coeff,
                                   rotational_mode_damping_coeff](
                                      auto /*time*/, auto& rod_system,
                                      std::size_t) {
          ::elastica::velocity(rod_system) *= translational_mode_damping_coeff;
          // Easier to encode as powers
          auto const n_elem(cpp17::size(rod_system));
          auto&& omega = ::elastica::angular_velocity(rod_system);
          auto&& dil =
              ::blocks::get<elastica::tags::ElementDilatation>(rod_system);

          for (auto idx_elem = 0UL; idx_elem < n_elem; ++idx_elem) {
            omega(0UL, idx_elem) *= std::pow(
                rotational_mode_damping_coeff(0UL, idx_elem), dil[idx_elem]);
            omega(1UL, idx_elem) *= std::pow(
                rotational_mode_damping_coeff(1UL, idx_elem), dil[idx_elem]);
            omega(2UL, idx_elem) *= std::pow(
                rotational_mode_damping_coeff(2UL, idx_elem), dil[idx_elem]);
          }
        }))();
  };
  // ===========ANALYTICAL DAMPING CODE END============================

  for (auto& rod_ref : rod_list)
    simulator->constrain(rod_ref).at(0UL).with(
        analytical_linear_damper(rod_ref.get()));
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
      log_rod_position(backbone_criss_rods, i,
                       "data/backbone_rod_coordinates_%010d.csv");
      log_rod_position(backbone_cross_rods, i,
                       "data/backbone_rod_second_layer_coordinates_%010d.csv");
      log_rod_position(magnetic_rod_list, i,
                       "data/magnetic_rod_coordinates_%010d.csv");
      //      log_penetration(i);
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

/*
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
*/
