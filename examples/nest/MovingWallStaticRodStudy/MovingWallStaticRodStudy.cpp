//==============================================================================
/*!
//  \file
//  \brief Free nest case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale,
//  Seung Hyun Kim - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "MovingWallStaticRodStudy.hpp"

// define to use TBB backend (for now)

#include <chrono>
#include <cmath>
#include <ctime>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Size.hpp>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <random>
#include <utility>  // pair

static const ::elastica::real_t rectified_time(1.0);  // wait til

void MovingWallStaticRodStudy::setup(InputTypes<This> const& input_parameters) {
  const ::elastica::real_t time_step(::elastica::get<dt>(input_parameters));

  wall_material = elastica::create_material(
      // material density
      8.94e-3,  // old elastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
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

  const ::elastica::real_t side_wall_dimension(
      ::elastica::get<packing_initial_radial_span>(  // width (square)
          input_parameters));
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const ::elastica::real_t base_radius =
      ::elastica::get<radius>(input_parameters);
  const std::size_t n_elem = ::elastica::get<n_elements>(input_parameters);

  // compute angle of inclination
  const ::elastica::real_t cos_alpha =
      2.0 * std::sqrt(2.0) * (side_wall_dimension - base_radius) / base_length;
  if (cos_alpha > 1.0)
    throw std::runtime_error("Invalid");

  const ::elastica::real_t alpha = std::acos(cos_alpha);
  //  const ::elastica::real_t R = ::elastica::get<packing_initial_radial_span>(
  //      input_parameters);  // controls radial extent of rod origin
  //      initialisation
  //  const ::elastica::real_t H =
  //  ::elastica::get<packing_initial_vertical_span>(
  //      input_parameters);  // controls vertical extent of rod origin

  // initialisation
  // Define rod ----Random rods
  //  std::random_device rd;
  //  std::mt19937 mt(0);  // fixed seed
  //  std::uniform_real_distribution<::elastica::real_t> dist_r(0.0, R);
  //  std::uniform_real_distribution<::elastica::real_t> dist_h(base_radius, H);
  //  std::uniform_real_distribution<::elastica::real_t> dist_ang(0.0, 2 *
  //  M_PI); constexpr auto x_axis = 0UL; constexpr auto y_axis = 1UL; constexpr
  //  auto z_axis = 2UL;
  //
  //  const ::elastica::real_t origin_r = dist_r(mt);
  //  const ::elastica::real_t origin_phi = dist_ang(mt);
  //  const ::elastica::real_t origin_h = dist_h(mt);
  //  const ::elastica::Vec3 origin{origin_r * std::cos(origin_phi),
  //                                origin_r * std::sin(origin_phi), origin_h};
  //
  //  ::elastica::real_t alpha = dist_ang(mt);
  //  ::elastica::real_t beta = dist_ang(mt);
  //  ::elastica::Vec3 relative_endpoint{
  //      base_length * std::cos(beta) * std::cos(alpha),
  //      base_length * std::cos(beta) * std::sin(alpha),
  //      base_length * std::sin(beta)};
  //  ::elastica::Vec3 end_point = origin + relative_endpoint;
  //  while ((end_point[x_axis] * end_point[x_axis] +
  //          end_point[y_axis] * end_point[y_axis]) > R * R ||
  //         end_point[z_axis] < base_radius || end_point[z_axis] > H) {
  //    alpha = dist_ang(mt);
  //    beta = dist_ang(mt);
  //    relative_endpoint =
  //        ::elastica::Vec3{base_length * std::cos(beta) * std::cos(alpha),
  //                         base_length * std::cos(beta) * std::sin(alpha),
  //                         base_length * std::sin(beta)};
  //    end_point = origin + relative_endpoint;
  //  }
  //  const ::elastica::Vec3 direction{std::cos(beta) * std::cos(alpha),
  //                                   std::cos(beta) * std::sin(alpha),
  //                                   std::sin(beta)};
  //  const ::elastica::Vec3 normal_vector{
  //      direction[z_axis], direction[z_axis],
  //      (-direction[x_axis] - direction[y_axis])};
  //  const ::elastica::Vec3 normal = ::elastica::normalize(normal_vector);

  const ::elastica::Vec3 origin{-side_wall_dimension + base_radius,
                                -side_wall_dimension + base_radius,
                                base_radius};
  const ::elastica::Vec3 direction{cos_alpha / std::sqrt(2.0),
                                   cos_alpha / std::sqrt(2.0), std::sin(alpha)};
  const ::elastica::Vec3 normal{1.0 / std::sqrt(2.0), -1.0 / std::sqrt(2.0),
                                0.0};

  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},    StraightInit::Material{wall_material},
      StraightInit::Radius{base_radius}, StraightInit::Length{base_length},
      StraightInit::Origin{origin},      StraightInit::Direction{direction},
      StraightInit::Normal{normal}
      //        StraightInit::ForceDampingRate{
      //            ::elastica::get<damping>(input_parameters)},
      //        StraightInit::TorqueDampingRate{
      //            ::elastica::get<damping>(input_parameters)},
      // TODO later refactor to use Cosserat rod without damping
      // StraightInit::ForceDampingRate{1e-10},
      // StraightInit::TorqueDampingRate{1e-10},
  };
  RodRef rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());
  rod_refs.push_back(rod_ref);

  // Since all rods are same we create this once and reuse for all rods
  // ===========ANALYTICAL DAMPING CODE START============================
  using Traits = ::elastica::cosserat_rod::DefaultCosseratRodTraits;
  const ::elastica::real_t damping_coeff =
      ::elastica::get<damping>(input_parameters);
  const ::elastica::real_t translational_mode_damping_coeff =
      blaze::exp(-damping_coeff * time_step);

  auto rod = rod_refs[0UL].get();
  auto& mass = ::elastica::get<elastica::tags::Mass>(rod);
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
  final_time = ::elastica::get<final_sim_time>(input_parameters);

  namespace ek = elastica::kinematics;

  //  using Spans = typename ImposedPlane::Spans;

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

  // Set the top plane at (0, 0, original_top_height) and with normal along Z-
  const ::elastica::real_t original_top_height(150);
  const ::elastica::Vec3 plane_position_top{0.0, 0.0, original_top_height};
  auto plane_normal_top = {0.0, 0.0, -1.0};
  auto plane_binormal_top = {0.0, -1.0, 0.0};
  auto plane_tangent_top = {1.0, 0.0, 0.0};
  ::elastica::Rot3 Q_top = {plane_normal_top, plane_binormal_top,
                            plane_tangent_top};

  // Moving Wall Configuration
  const ::elastica::real_t top_wall_speed(5.0);
  const ::elastica::real_t final_height_high(80);
  const ::elastica::real_t final_height_low(60);
  const ::elastica::real_t duration((final_height_high - final_height_low) /
                                    top_wall_speed);

  auto get_phase = [=](::elastica::real_t time_v) -> ::elastica::real_t {
    auto const shifted_time = (time_v - rectified_time);
    // duration is half-cycle : two durations is one cycle
    auto const oscillation_count = static_cast<int>(shifted_time / duration);

    bool const moving_upward(oscillation_count % 2);
    bool const moving_downward = !moving_upward;

    // 0.0 < phase < 1.0, phase within duration
    auto const phase =
        (shifted_time - (duration * ::elastica::real_t(oscillation_count))) /
        duration;
    // if moving downward return phase, else return 1.0 - phase
    return ::elastica::real_t(moving_downward) * phase +
           ::elastica::real_t(moving_upward) * (1.0 - phase);
  };

  auto top_moving_wall_position =
      [=](::elastica::real_t time_v) noexcept -> ::elastica::real_t {
    bool const is_static(time_v < rectified_time);
    // Discontinuity at time t = rectified_time
    return is_static
               ? original_top_height
               : (final_height_high -
                  ((final_height_high - final_height_low) * get_phase(time_v)));
  };
  auto top_moving_wall_velocity =
      [=](::elastica::real_t time_v) noexcept -> ::elastica::real_t {
    bool const is_static(time_v < rectified_time);
    auto const shifted_time = (time_v - rectified_time);
    // duration is half-cycle : two durations is one cycle
    auto const oscillation_count = static_cast<int>(shifted_time / duration);
    bool const moving_upward(oscillation_count % 2);

    return is_static ? 0.0 : (moving_upward ? top_wall_speed : -top_wall_speed);
  };

  auto always_zero_ = [](::elastica::real_t) noexcept -> ::elastica::real_t {
    return 0.0;
  };
  auto top_translations_buffer = ek::make_translations(
      ek::translate_along<elastica::EulerianFrame::X>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Y>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Z>(
          ek::mark_position(
              ImposedHalfSpace::anchor_to_com_p(top_moving_wall_position)),
          ek::mark_velocity(top_moving_wall_velocity)));
  imposed_half_space_top_ref = simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          1UL, 1UL, true, plane_position_top, Q_top, wall_material),
      std::move(top_translations_buffer));

  // STATIOC SIDE WALLS
  const ::elastica::real_t side_wall_dimension_with_gap = side_wall_dimension;
  // Set the left plane at (-side_wall_dimension_with_gap, 0, 0) and with
  // normal along X
  const ::elastica::Vec3 plane_position_left{-side_wall_dimension_with_gap, 0.0,
                                             0.0};
  auto plane_normal_left = {1.0, 0.0, 0.0};
  auto plane_binormal_left = {0.0, 0.0, 1.0};
  auto plane_tangent_left = {0.0, -1.0, 0.0};
  ::elastica::Rot3 Q_left = {plane_normal_left, plane_binormal_left,
                             plane_tangent_left};
  simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          2UL, 2UL, true, plane_position_left, Q_left, wall_material));

  // Set the right plane at (side_wall_dimension_with_gap, 0, 0) and with
  // normal along X-
  const ::elastica::Vec3 plane_position_right{side_wall_dimension_with_gap, 0.0,
                                              0.0};
  auto plane_normal_right = {-1.0, 0.0, 0.0};
  auto plane_binormal_right = {0.0, 0.0, -1.0};
  auto plane_tangent_right = {0.0, 1.0, 0.0};
  ::elastica::Rot3 Q_right = {plane_normal_right, plane_binormal_right,
                              plane_tangent_right};
  simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          3UL, 3UL, true, plane_position_right, Q_right, wall_material));

  // Set the back plane at (0, side_wall_dimension_with_gap, 0) and with
  // normal along Y-
  const ::elastica::Vec3 plane_position_back{0.0, side_wall_dimension_with_gap,
                                             0.0};
  auto plane_normal_back = {0.0, -1.0, 0.0};
  auto plane_binormal_back = {-1.0, 0.0, 0.0};
  auto plane_tangent_back = {0.0, 0.0, 1.0};
  ::elastica::Rot3 Q_back = {plane_normal_back, plane_binormal_back,
                             plane_tangent_back};
  simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          4UL, 4UL, true, plane_position_back, Q_back, wall_material));

  // Set the front plane at (0, -side_wall_dimension_with_gap, 0) and with
  // normal along Y
  const ::elastica::Vec3 plane_position_front{
      0.0, -side_wall_dimension_with_gap, 0.0};
  auto plane_normal_front = {0.0, 1.0, 0.0};
  auto plane_binormal_front = {1.0, 0.0, 0.0};
  auto plane_tangent_front = {0.0, 0.0, -1.0};
  ::elastica::Rot3 Q_front = {plane_normal_front, plane_binormal_front,
                              plane_tangent_front};
  simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          5UL, 5UL, true, plane_position_front, Q_front, wall_material));

  // set near-list update time
  auto& collision_solver =
      simulator
          ->get_environment<elastica::environments::tags::CollisionSystem>();
  collision_solver->time_interval_between_detection() =
      1000.0 * time_step;  // anything > 0.0

  const ::elastica::real_t gravity_acc(
      ::elastica::get<gravitational_acc>(input_parameters));
  const ::elastica::Vec3 gravity_acc_vector{0.0, 0.0, gravity_acc};
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  simulator->finalize();
  // Set the dt value
  set_dt(time_step);
}

void MovingWallStaticRodStudy::step() {
  // Reduce contact detection interval
  const std::uint64_t flag_iteration(rectified_time / current_dt());
  if (get_current_iteration().step() == flag_iteration) {
    auto& collision_solver =
        simulator
            ->get_environment<elastica::environments::tags::CollisionSystem>();
    //    collision_solver->time_interval_between_detection() =
    //        10.0 * current_dt();  // anything > 0.0
    // set_dt(1.0e-6);
  }

  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void MovingWallStaticRodStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();
  using RodCoordinates =
      std::tuple<::elastica::real_t, ::elastica::real_t, ::elastica::real_t>;
  using Energies = std::tuple<::elastica::real_t, ::elastica::real_t,
                              ::elastica::real_t, ::elastica::real_t>;
  std::vector<RodCoordinates> rod_coordinates;
  std::vector<Energies> energies;
  using TimesAndForces =
      std::tuple<::elastica::real_t, ::elastica::Vec3, ::elastica::real_t>;
  std::vector<TimesAndForces> times_and_forces;

  //  explicitly get the system
  const auto n_nodes = ::elastica::size(rod_refs[0].get()) + 1UL;
  const auto n_rod = rod_refs.size();
  constexpr auto x_axis = 0UL;
  constexpr auto y_axis = 1UL;
  constexpr auto z_axis = 2UL;

  const ::elastica::real_t fps = 60;
  const std::size_t n_steps = (final_time / current_dt());
  const std::size_t write_interval =
      n_steps / (final_time * fps);  // 1000000UL;
  rod_coordinates.reserve(n_rod * n_nodes);
  energies.reserve(n_rod * 4UL);
  std::ofstream temporal_file;

  std::cout << std::fixed << std::setprecision(1);
  float current_progress = 0.0;
  auto stime = std::chrono::system_clock::now();
  for (std::size_t i = 0UL; i < n_steps; ++i) {
    // Progress Bar
    float progress = float(i) / float(n_steps);
#ifndef ELASTICA_DEBUG
    if (current_progress <= progress && progress > 0.0001) {
      int bar_width = 70;
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
      auto etime = std::chrono::system_clock::now();
      std::chrono::duration<int> elapsed_time =
          std::chrono::duration_cast<std::chrono::seconds>(etime - stime);
      std::time_t etd = std::chrono::system_clock::to_time_t(
          stime +
          std::chrono::seconds(int(float(elapsed_time.count()) / progress)));
      std::cout << "]" << progress * 100.0 << "%"
                << " [elapsed: " << elapsed_time.count() << " sec]"
                << " [etd: " << std::strtok(std::ctime(&etd), "\n") << "]\r";
      std::cout.flush();
      current_progress += 0.001;
    }
#endif

    // Logging
    if (not(current_iteration() % write_interval)) {
      // if (1) {

      // Record energy
      for (auto const& rod_ref : rod_refs) {
        energies.emplace_back(
            ::elastica::compute_translational_energy(rod_ref.get()),
            ::elastica::compute_rotational_energy(rod_ref.get()),
            ::elastica::compute_bend_energy(rod_ref.get()),
            ::elastica::compute_shear_energy(rod_ref.get()));
      }

      char file_name[100];
      sprintf(file_name, "data/rod_energies_%010d.csv", i);
      temporal_file.open(file_name);
      for (auto const& energy : energies) {
        temporal_file << std::setprecision(16) << std::get<0UL>(energy) << ","
                      << std::get<1UL>(energy) << "," << std::get<2UL>(energy)
                      << "," << std::get<3UL>(energy) << "\n";
      }
      temporal_file << std::endl;
      temporal_file.close();
      energies.clear();

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
      sprintf(file_name, "data/rod_coordinates_%010d.csv", i);
      temporal_file.open(file_name);
      for (auto const& rod_coord : rod_coordinates) {
        temporal_file << std::setprecision(16) << std::get<x_axis>(rod_coord)
                      << "," << std::get<y_axis>(rod_coord) << ","
                      << std::get<z_axis>(rod_coord) << "\n";
      }
      temporal_file << std::endl;
      temporal_file.close();
      rod_coordinates.clear();

      // record plane
      auto& imposed_plane = imposed_half_space_bottom_ref.get();
      times_and_forces.emplace_back(
          current_time(), imposed_plane.get_force(),
          imposed_half_space_top_ref.get().get_position()[2UL]);
      sprintf(file_name, "data/imposed_plane_%010d.csv", i);
      std::ofstream nest_diagnostics_file;
      nest_diagnostics_file.open(file_name);
      constexpr auto Time = 0UL;
      constexpr auto ForceVector = 1UL;
      for (auto const& tf : times_and_forces) {
        nest_diagnostics_file << std::setprecision(16) << std::get<Time>(tf)
                              << "," << std::get<ForceVector>(tf)[x_axis] << ","
                              << std::get<ForceVector>(tf)[y_axis] << ","
                              << std::get<ForceVector>(tf)[z_axis] << ","
                              << std::get<2UL>(tf) << "\n";  // Z-Position
      }
      nest_diagnostics_file << std::endl;
      nest_diagnostics_file.close();
      times_and_forces.clear();

      // Record Interpenetration
      auto& collision_system = simulator->get_environment<
          elastica::environments::tags::CollisionSystem>();
      auto& contacts = collision_system->contacts();
      using Penetrations = std::vector<elastica::real_t>;
      Penetrations penetrations;
      penetrations.reserve(contacts.size());
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

    // Actual STEP!!
    step();
  }
}
