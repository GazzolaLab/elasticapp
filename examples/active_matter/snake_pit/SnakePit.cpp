//==============================================================================
/*!
//  \file
//  \brief Snake on plane
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "SnakePit.hpp"

#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Forcing/InternalTorquesAdapter.hpp>
#include <elastica/Simulator/IO/H5.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Identity.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Rampup.hpp>

#include "Activation/Funcs.hpp"
//
#include <cmath>
#include <random>

void SnakePit::setup(InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      ::elastica::get<density>(input_parameters),
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // coefficient of static friction
      1e-10,
      // coefficient of dynamic friction
      1e-10);

  const ::elastica::Vec3 anchor{0., 0., 0.};

  const auto inclination(0.0);
  // Normal along y, Binormal along -x, Tangent along z
  //  const ::elastica::Vec3 direction{0.0, std::sin(inclination),
  //                                   std::cos(inclination)};
  //  const ::elastica::Vec3 normal{0.0, std::cos(inclination),
  //                                -std::sin(inclination)};
  //  // From (x, y, z) -> (z, x, y)
  //  const ::elastica::Rot3 to_plane_coordinates = {
  //      {0., 0., 1.}, {1., 0., 0.}, {0., 1., 0.}};

  const ::elastica::Vec3 direction{0.0, std::cos(inclination),
                                   std::sin(inclination)};
  const ::elastica::Vec3 normal{0.0, -std::sin(inclination),
                                std::cos(inclination)};

  // const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const ::elastica::real_t base_radius{
      ::elastica::get<radius>(input_parameters)};
  const auto number_of_snakes = ::elastica::get<n_snakes>(input_parameters);
  const std::size_t n_elem(::elastica::get<n_elements>(input_parameters));

  // half-space
  const ::elastica::Vec3 half_space_anchor{0.0, 0.0, 0.0};
  auto half_space_normal = normal;
  auto half_space_tangent = direction;
  // -x
  ::elastica::Vec3 half_space_binormal =
      cross(half_space_tangent, half_space_normal);

  // implicit convention
  ::elastica::Rot3 Q = {
      {half_space_normal[0], half_space_normal[1], half_space_normal[2]},
      {half_space_binormal[0], half_space_binormal[1], half_space_binormal[2]},
      {half_space_tangent[0], half_space_tangent[1], half_space_tangent[2]}};

  namespace ek = elastica::kinematics;

  auto always_zero_ = [](::elastica::real_t) noexcept -> ::elastica::real_t {
    return 0.0;
  };
  auto displaced_by_radius =
      [=](::elastica::real_t) noexcept -> ::elastica::real_t {
    return -base_radius;
  };

  //  elastica::MaterialID plane_material = elastica::create_material(
  //      // material density
  //      ::elastica::get<density>(input_parameters),
  //      // coefficient of restitution
  //      0.24,
  //      // Young's modulus
  //      ::elastica::get<youngs_modulus>(input_parameters),
  //      // Shear modulus
  //      ::elastica::get<shear_modulus>(input_parameters),
  //      // Contact stiffness
  //      1e6, /
  //      // normal contact damping
  //      ::elastica::get<contact_damping>(input_parameters),
  //      // tangent contact damping
  //      ::elastica::get<contact_damping>(input_parameters),
  //      // coefficient of static friction
  //      1e-10,
  //      // coefficient of dynamic friction
  //      1e-10);

  auto translations_buffer = ek::make_translations(
      ek::translate_along<elastica::EulerianFrame::X>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Y>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Z>(
          // The COM along z axis is not at 0 (the anchor position), but rather
          // inside the halfspace, so we displace it by anchor_to_com.
          ek::mark_position(
              ImposedHalfSpace::anchor_to_com_n(displaced_by_radius)),
          ek::mark_velocity(always_zero_)));
  // Use same material for now.
  imposed_half_space_ref = simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          0UL, 0UL, true, half_space_anchor, Q, my_material),
      std::move(translations_buffer));

  auto const side_wall_ratio =
      ::elastica::get<wall_distance_ratio>(input_parameters);
  bool side_walls(side_wall_ratio > 0.0);

  if (side_walls) {
    // normal
    const elastica::real_t distance_between_walls =
        side_wall_ratio * base_length;

    // left wall
    {
      ::elastica::Vec3 wall_anchor =
          anchor - 0.5 * distance_between_walls * half_space_binormal;
      ::elastica::Rot3 wallQ = {
          // x
          {half_space_binormal[0], half_space_binormal[1],
           half_space_binormal[2]},
          {half_space_tangent[0], half_space_tangent[1], half_space_tangent[2]},
          {half_space_normal[0], half_space_normal[1], half_space_normal[2]}};

      simulator->emplace_back<ImposedHalfSpace>(
          ::elastica::collision::HalfSpacePrimitive(0UL, 1UL, true, wall_anchor,
                                                    wallQ, my_material));
    }
    // right wall
    {
      ::elastica::Vec3 wall_anchor =
          anchor + 0.5 * distance_between_walls * half_space_binormal;
      ::elastica::Rot3 wallQ = {
          // x
          {-half_space_binormal[0], -half_space_binormal[1],
           -half_space_binormal[2]},
          // z
          {-half_space_tangent[0], -half_space_tangent[1],
           -half_space_tangent[2]},
          {half_space_normal[0], half_space_normal[1], half_space_normal[2]}};

      simulator->emplace_back<ImposedHalfSpace>(
          ::elastica::collision::HalfSpacePrimitive(0UL, 2UL, true, wall_anchor,
                                                    wallQ, my_material));
    }
    // behind wall
    {
      ::elastica::Vec3 wall_anchor =
          anchor - 0.5 * distance_between_walls * half_space_tangent;
      ::elastica::Rot3 wallQ = {
          // x
          {half_space_tangent[0], half_space_tangent[1], half_space_tangent[2]},
          {half_space_normal[0], half_space_normal[1], half_space_normal[2]},
          {half_space_binormal[0], half_space_binormal[1],
           half_space_binormal[2]}};

      simulator->emplace_back<ImposedHalfSpace>(
          ::elastica::collision::HalfSpacePrimitive(0UL, 3UL, true, wall_anchor,
                                                    wallQ, my_material));
    }
    // ahead wall
    {
      ::elastica::Vec3 wall_anchor =
          anchor + 0.5 * distance_between_walls * half_space_tangent;
      ::elastica::Rot3 wallQ = {
          // x
          {-half_space_tangent[0], -half_space_tangent[1],
           -half_space_tangent[2]},
          {-half_space_normal[0], -half_space_normal[1], -half_space_normal[2]},
          {half_space_binormal[0], half_space_binormal[1],
           half_space_binormal[2]}};

      simulator->emplace_back<ImposedHalfSpace>(
          ::elastica::collision::HalfSpacePrimitive(0UL, 4UL, true, wall_anchor,
                                                    wallQ, my_material));
    }
  }

  using RodInitializer =
      ::elastica::cosserat_rod::StraightCosseratRodInitializer;

  //  RodInitializer straight_initializer{
  //      RodInitializer::NElement{n_elem},
  //      RodInitializer::Material{my_material},
  //      RodInitializer::Radius{base_radius},
  //      RodInitializer::Length{base_length}, RodInitializer::Origin{anchor},
  //      RodInitializer::Direction{direction}, RodInitializer::Normal{normal},
  //      // maybe needs to be scaled later
  //      RodInitializer::ForceDampingRate{1e-1},
  //      RodInitializer::TorqueDampingRate{1e-1}};

  auto initialize_a_snake = [&](::elastica::Vec3 orig, ::elastica::Vec3 dir) {
    RodInitializer straight_initializer{
        RodInitializer::NElement{n_elem}, RodInitializer::Material{my_material},
        RodInitializer::Radius{base_radius},
        RodInitializer::Length{base_length}, RodInitializer::Origin{orig},
        RodInitializer::Direction{dir}, RodInitializer::Normal{normal},
        // UNUSED
        RodInitializer::ForceDampingRate{1e-10},
        RodInitializer::TorqueDampingRate{1e-10}};
    return simulator->template emplace_back<CosseratRod>(
        straight_initializer.initialize<CosseratRod>());
  };

  auto lattice_initialization = [&]() {
    // Params
    const std::size_t lattice_size_in_one_direction = 2UL;
    const std::size_t lattice_size_in_z_direction = 2UL;
    const std::size_t n_snakes_in_one_direction = number_of_snakes;
    std::array<::elastica::Vec3, 2UL> const snake_directions{
        direction, -half_space_binormal};

    auto lex_indices = [=](std::size_t lattice_index) {
      auto t = (lattice_size_in_one_direction * lattice_size_in_one_direction);
      return std::make_tuple(
          lattice_index % lattice_size_in_one_direction,
          (lattice_index % t) / lattice_size_in_one_direction,
          lattice_index / t);
    };

    // Ugly hack, sufficies for now.
    auto lattice_arrangement = [&](std::size_t lattice_index) mutable {
      std::size_t x, y, z;
      std::tie(x, y, z) = lex_indices(lattice_index);
      // Z doesnt influence the arrangemnet
      return (x + y) % 2;
    };

    auto lattice_anchor = [&](std::size_t lattice_index) {
      ::elastica::real_t x, y, z;
      std::tie(x, y, z) = lex_indices(lattice_index);
      return ::elastica::Vec3{(x + 0.5) * base_length * snake_directions[0UL] +
                              (y + 0.5) * base_length * snake_directions[1UL] +
                              // Total lattice height is 4r
                              ((z + 2.0) * base_radius) * normal};
    };

    auto initialize_lattice_unit = [&](std::size_t lattice_index) {
      // Either 0 or 1
      auto lattice_type = bool(lattice_arrangement(lattice_index));
      auto anchor = lattice_anchor(lattice_index);
      // spacing is
      const ::elastica::real_t spacing =
          base_length / elastica::real_t(n_snakes_in_one_direction);

      //  const ::elastica::real_t spacing(3.0 *
      //                                   ::elastica::get<radius>(input_parameters));
      // along +x
      const ::elastica::Vec3 rod_plane_offset(spacing * -half_space_binormal);

      // Bottom snakes
      auto bottom_snake_anchor = elastica::Vec3{
          anchor - 0.5 * base_length * snake_directions[0UL] -
          0.5 * base_length * snake_directions[1UL] - base_radius * normal +
          0.5 * spacing * snake_directions[!lattice_type]};
      for (auto idx = 0UL; idx < n_snakes_in_one_direction; ++idx)
        cosserat_rod_refs.push_back(initialize_a_snake(
            bottom_snake_anchor +
                idx * spacing * snake_directions[!lattice_type],
            snake_directions[lattice_type]));

      // Top snakes
      auto top_snake_anchor = elastica::Vec3{
          anchor - 0.5 * base_length * snake_directions[0UL] -
          0.5 * base_length * snake_directions[1UL] + base_radius * normal +
          0.5 * spacing * snake_directions[lattice_type]};
      for (auto idx = 0UL; idx < n_snakes_in_one_direction; ++idx)
        cosserat_rod_refs.push_back(initialize_a_snake(
            top_snake_anchor + idx * spacing * snake_directions[lattice_type],
            snake_directions[!lattice_type]));
    };

    const auto lattice_size = lattice_size_in_one_direction *
                              lattice_size_in_one_direction *
                              lattice_size_in_z_direction;
    // 2 for bottom and top
    cosserat_rod_refs.reserve(2 * n_snakes_in_one_direction * lattice_size);

    for (auto lattice_idx = 0UL; lattice_idx < lattice_size; ++lattice_idx)
      initialize_lattice_unit(lattice_idx);
  };

  // Constraints return true if violated else false.
  auto min_height_constraint = [](::elastica::Vec3 const& end_point,
                                  ::elastica::real_t min_height) {
    constexpr auto z_axis = 2UL;
    return end_point[z_axis] < min_height;
  };

  auto with_cylinder_constraint = [](::elastica::Vec3 const& end_point,
                                     ::elastica::real_t max_radius,
                                     ::elastica::real_t max_height) -> bool {
    constexpr auto x_axis = 0UL;
    constexpr auto y_axis = 1UL;
    constexpr auto z_axis = 2UL;

    return (end_point[x_axis] * end_point[x_axis] +
            end_point[y_axis] * end_point[y_axis]) > max_radius * max_radius ||
           end_point[z_axis] > max_height;
  };

  auto with_cone_constraint = [](::elastica::Vec3 const& end_point,
                                 ::elastica::real_t max_radius,
                                 ::elastica::real_t max_height) -> bool {
    constexpr auto x_axis = 0UL;
    constexpr auto y_axis = 1UL;
    constexpr auto z_axis = 2UL;

    const auto h = end_point[z_axis];
    const auto r = (max_height - h) * max_radius / max_height;

    return (end_point[x_axis] * end_point[x_axis] +
            end_point[y_axis] * end_point[y_axis]) > r * r ||
           h > max_height;
  };

  auto random_initialization = [&](auto point_constraint) {
    auto& origin_point_constraint(point_constraint);
    auto& end_point_constraint(point_constraint);
    std::mt19937 mt(0);
    // const std::size_t n_layers = 10UL;
    const elastica::real_t R =
        ::elastica::get<packing_initial_radial_span_ratio>(input_parameters) *
        base_length;
    const elastica::real_t H =
        ::elastica::get<packing_initial_vertical_span_ratio>(input_parameters) *
        base_length;
    std::uniform_real_distribution<::elastica::real_t> dist_r(0.0, R);
    std::uniform_real_distribution<::elastica::real_t> dist_h(base_radius, H);
    // in-plane angle
    std::uniform_real_distribution<::elastica::real_t> dist_alpha(0.0,
                                                                  2 * M_PI);
    // out-of-plane angle
    constexpr auto is_cylinder_configuration(
        cpp17::is_same_v<decltype(point_constraint),
                         decltype(with_cylinder_constraint)>);
    // Code was non-symmetric distribution initially. Do not know if it matters
    // for self-organization. Hence it is now parametrized.
    // false definitely leads to self-organization, so we choose that for
    // cylinder
    constexpr auto symmetric_distribution(!is_cylinder_configuration);
    constexpr auto limit = is_cylinder_configuration
                               ? 0.2 * M_PI
                               // cone constraint cannot be initialized with low
                               // angles : lot of them go out of cone
                               : 0.5 * M_PI;
    std::uniform_real_distribution<::elastica::real_t> dist_beta(
        symmetric_distribution ? -limit : 0.0, limit);

    cosserat_rod_refs.reserve(number_of_snakes);

    RodInitializer straight_initializer{
        // RodInitializerialization Wrapper
        RodInitializer::NElement{n_elem}, RodInitializer::Material{my_material},
        RodInitializer::Radius{base_radius},
        RodInitializer::Length{base_length},
        RodInitializer::Origin{elastica::Vec3{0., 0., 0.}},
        RodInitializer::Direction{direction}, RodInitializer::Normal{normal},
        // UNUSED
        RodInitializer::ForceDampingRate{1e-10},
        RodInitializer::TorqueDampingRate{1e-10}};

    constexpr auto x_axis = 0UL;
    constexpr auto y_axis = 1UL;
    constexpr auto z_axis = 2UL;

    const auto is_violated = [](bool b) -> bool { return b; };

    for (std::size_t i = 0; i < number_of_snakes; ++i) {
      ::elastica::real_t origin_r = dist_r(mt);
      ::elastica::real_t origin_phi = dist_alpha(mt);
      ::elastica::real_t origin_h = dist_h(mt);

      ::elastica::Vec3 origin{origin_r * std::cos(origin_phi),
                              origin_r * std::sin(origin_phi), origin_h};

      while (is_violated(origin_point_constraint(origin, R, H)) ||
             is_violated(min_height_constraint(origin, base_radius))) {
        origin_r = dist_r(mt);
        origin_phi = dist_alpha(mt);
        origin_h = dist_h(mt);
        origin = ::elastica::Vec3{origin_r * std::cos(origin_phi),
                                  origin_r * std::sin(origin_phi), origin_h};
      }

      ::elastica::real_t alpha = dist_alpha(mt);
      ::elastica::real_t beta = dist_beta(mt);
      ::elastica::Vec3 relative_endpoint{
          base_length * std::cos(beta) * std::cos(alpha),
          base_length * std::cos(beta) * std::sin(alpha),
          base_length * std::sin(beta)};
      ::elastica::Vec3 end_point = origin + relative_endpoint;

      while (is_violated(end_point_constraint(end_point, R, H)) ||
             is_violated(min_height_constraint(end_point, base_radius))) {
        alpha = dist_alpha(mt);
        beta = dist_beta(mt);
        relative_endpoint =
            ::elastica::Vec3{base_length * std::cos(beta) * std::cos(alpha),
                             base_length * std::cos(beta) * std::sin(alpha),
                             base_length * std::sin(beta)};
        end_point = origin + relative_endpoint;
      }
      const ::elastica::Vec3 rod_direction{std::cos(beta) * std::cos(alpha),
                                           std::cos(beta) * std::sin(alpha),
                                           std::sin(beta)};
      const ::elastica::Vec3 normal_vector{
          rod_direction[z_axis], rod_direction[z_axis],
          (-rod_direction[x_axis] - rod_direction[y_axis])};
      const ::elastica::Vec3 rod_normal = ::elastica::normalize(normal_vector);
      cosserat_rod_refs.push_back(simulator->emplace_back<CosseratRod>(
          straight_initializer
              .but_with(RodInitializer::Origin{origin},
                        RodInitializer::Direction{rod_direction},
                        RodInitializer::Normal{rod_normal})
              .initialize<CosseratRod>()));
    }
  };

  auto iequals = [](std::string const& a, std::string const& b) -> bool {
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                      [](unsigned char a, unsigned char b) {
                        return std::tolower(a) == std::tolower(b);
                      });
  };

  if (iequals(::elastica::get<initialization_protocol>(input_parameters),
              initialization_protocol::random_cylinder()))
    random_initialization(with_cylinder_constraint);
  else if (iequals(::elastica::get<initialization_protocol>(input_parameters),
                   initialization_protocol::random_cone()))
    random_initialization(with_cone_constraint);
  else
    lattice_initialization();

  const auto T = ::elastica::get<time_period>(input_parameters);
  snake_time_period = T;
  const auto lambda = ::elastica::get<wave_length>(input_parameters);

  // Numbers from pyelastica snake.
  std::vector<double> X = {0.050000, 0.130000, 0.210000, 0.290000,
                           0.370000, 0.450000, 0.530000, 0.610000,
                           0.690000, 0.770000, 0.850000, 0.930000};
  std::vector<double> Y = {0.003394, 0.003474, 0.003604, 0.003697,
                           0.003673, 0.003525, 0.003330, 0.003170,
                           0.003126, 0.003219, 0.003371, 0.003489};

  namespace activation = ::elastica::forcing::activation;

  // Spatio-temporal actuation
  const auto omega = elastica::real_t(2.0 * M_PI) / T;
  const auto wavenumber = elastica::real_t(2.0 * M_PI) / lambda;
  auto const phase_shift(0.0);
  activation::Sinusoid wave{omega, wavenumber, phase_shift};
  auto spline_activation(activation::mark_spatial_activation(
      activation::InterpolatingSpline(std::move(X), std::move(Y))));
  // Start after Non-dimensional time periods
  auto ramp(activation::mark_temporal_activation(elastica::make_sin_rampup(
      ::elastica::get<activation_start_time_nd>(input_parameters) * T, T)));

  // copy it here (why not?)
  auto forcing_amplitude(activation::multiply(wave, spline_activation, ramp));

  auto forcing_func(
      [=, forcing_direction = normal](elastica::real_t s,
                                      elastica::real_t t) -> elastica::Vec3 {
        // 0.5 is here from legacy reasons : technically there is no need for
        // a 0.5 here, but keep it around to get the same results.
        return 0.5 * forcing_amplitude(s, t) * forcing_direction;
      });

  namespace efo = ::elastica::forcing;
  auto snake_forcing_gen = [&]() {
    return efo::as_internal_torques(forcing_func);
  };

  // Force them
  for (auto& rod_ref : cosserat_rod_refs)
    simulator->force(rod_ref)
        .at(::elastica::interface::range(0UL, n_elem))
        .with(snake_forcing_gen());

  // ===========ANALYTICAL DAMPING CODE START============================
  using Traits = ::elastica::cosserat_rod::DefaultCosseratRodTraits;
  const ::elastica::real_t damping_coeff =
      ::elastica::get<damping>(input_parameters);
  auto const time_step(::elastica::get<dt>(input_parameters));
  const ::elastica::real_t translational_mode_damping_coeff =
      blaze::exp(-damping_coeff * time_step);

  auto one_rod = cosserat_rod_refs[0UL].get();
  auto&& mass = ::elastica::get<elastica::tags::Mass>(one_rod);
  blaze::DynamicVector<::elastica::real_t> elemental_mass =
      Traits::Operations::average_kernel(mass);
  // correct elemental mass at ends
  elemental_mass[0UL] += 0.5 * mass[0UL];
  elemental_mass[::elastica::cosserat_rod::get_batch_elems(elemental_mass) -
                 1UL] +=
      0.5 * mass[::elastica::cosserat_rod::get_batch_elems(mass) - 1UL];
  blaze::DynamicMatrix<::elastica::real_t> rotational_mode_damping_coeff =
      blaze::exp(Traits::Operations::transpose(
          Traits::Operations::expand_for_broadcast(
              elemental_mass * -damping_coeff * time_step)));
  constexpr std::size_t dim{3UL};
  for (std::size_t row_idx = 0; row_idx < dim; row_idx++) {
    for (std::size_t column_idx = 0;
         column_idx < ::elastica::cosserat_rod::get_batch_elems(elemental_mass);
         column_idx++) {
      rotational_mode_damping_coeff(row_idx, column_idx) = std::pow(
          rotational_mode_damping_coeff(row_idx, column_idx),
          ::elastica::get<::elastica::tags::InvMassSecondMomentOfInertia>(
              one_rod)(row_idx, column_idx));
    }
  }
  // setting analytical damper via constraint
  namespace eco = ::elastica::constraints;
  auto analytical_linear_damper = eco::make_constraint(
      eco::make_value_constraint([](auto, auto& rod_system, std::size_t) {}),
      eco::make_rate_constraint([translational_mode_damping_coeff,
                                 rotational_mode_damping_coeff](
                                    auto, auto& rod_system, std::size_t) {
        ::elastica::velocity(rod_system) =
            ::elastica::velocity(rod_system) * translational_mode_damping_coeff;
        ::elastica::angular_velocity(rod_system) =
            ::elastica::angular_velocity(rod_system) %
            rotational_mode_damping_coeff;
      }));
  // ===========ANALYTICAL DAMPING CODE END============================

  for (auto& rod_ref : cosserat_rod_refs)
    simulator->constrain(rod_ref).at(0UL).with(analytical_linear_damper());

  const ::elastica::Vec3 gravity_acc_vector =
      ::elastica::get<gravitational_acc>(input_parameters) * normal;
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  simulator->finalize();

  // Set the dt value
  final_time = ::elastica::get<final_sim_time>(input_parameters);
  n_samples_per_period = ::elastica::get<samples_per_period>(input_parameters);
  set_dt(::elastica::get<dt>(input_parameters));
}

void SnakePit::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void SnakePit::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  // Seems to be triggered even if no nans.
  // enable_floating_point_exceptions();
  // const ::elastica::real_t final_time(1.0 * time_period::default_value());
  const auto n_steps = static_cast<std::size_t>(final_time / current_dt());

  const std::size_t n_steps_in_one_period(snake_time_period / current_dt());
  const std::size_t write_interval(n_steps_in_one_period /
                                   n_samples_per_period);
  ::elastica::io::H5Metadata metadata{};

  auto record_data = [&]() {
    auto writer = metadata.write_at({this->get_current_time(), this->get_dt(),
                                     this->get_current_iteration()});
    simulator->for_each_system_sequential([&](auto& slice) {
      using Subset = tmpl::list<elastica::tags::Position>;
      auto subset = blocks::cache<Subset>(slice);
      writer(subset);
    });
  };
  record_data();

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();

    if (not(current_iteration() % write_interval)) {
      record_data();
    }
  }
}
