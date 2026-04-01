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

#include "FreeNestCaseStudy.hpp"

// define to use TBB backend (for now)
#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/global_control.h>
#endif

#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Size.hpp>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <random>
#include <utility>  // pair

void FreeNestCaseStudy::setup(InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
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
      ::elastica::get<contact_damping>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // coefficient of static friction
      0.4,
      // coefficient of dynamic friction
      0.2);

  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const ::elastica::real_t base_radius =
      ::elastica::get<radius>(input_parameters);
  const std::size_t n_elem = ::elastica::get<n_elements>(input_parameters);
  const std::size_t n_rod = ::elastica::get<n_rods>(input_parameters);

  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  const ::elastica::real_t R =
      1 * base_length;  // controls radial extent of rod origin initialisation
  const ::elastica::real_t H = ::elastica::get<packing_initial_vertical_span>(
      input_parameters);  // controls vertical extent of rod origin
  // initialisation
  // Define rod ----Random rods
  std::random_device rd;
  //  std::mt19937 mt(rd());
  std::mt19937 mt(0);  // fixed seed
  std::uniform_real_distribution<::elastica::real_t> dist_r(0.0, R);
  std::uniform_real_distribution<::elastica::real_t> dist_h(base_radius, H);
  std::uniform_real_distribution<::elastica::real_t> dist_ang(0.0, 2 * M_PI);
  constexpr auto x_axis = 0UL;
  constexpr auto y_axis = 1UL;
  constexpr auto z_axis = 2UL;
  for (std::size_t i = 0; i < n_rod; ++i) {
    const ::elastica::real_t origin_r = dist_r(mt);
    const ::elastica::real_t origin_phi = dist_ang(mt);
    const ::elastica::real_t origin_h = dist_h(mt);
    const ::elastica::Vec3 origin{origin_r * std::cos(origin_phi),
                                  origin_r * std::sin(origin_phi), origin_h};

    ::elastica::real_t alpha = dist_ang(mt);
    ::elastica::real_t beta = dist_ang(mt);
    ::elastica::Vec3 relative_endpoint{
        base_length * std::cos(beta) * std::cos(alpha),
        base_length * std::cos(beta) * std::sin(alpha),
        base_length * std::sin(beta)};
    ::elastica::Vec3 end_point = origin + relative_endpoint;
    while ((end_point[x_axis] * end_point[x_axis] +
            end_point[y_axis] * end_point[y_axis]) > R * R ||
           end_point[z_axis] < base_radius) {
      alpha = dist_ang(mt);
      beta = dist_ang(mt);
      relative_endpoint =
          ::elastica::Vec3{base_length * std::cos(beta) * std::cos(alpha),
                           base_length * std::cos(beta) * std::sin(alpha),
                           base_length * std::sin(beta)};
      end_point = origin + relative_endpoint;
    }
    const ::elastica::Vec3 direction{std::cos(beta) * std::cos(alpha),
                                     std::cos(beta) * std::sin(alpha),
                                     std::sin(beta)};
    const ::elastica::Vec3 normal_vector{
        direction[z_axis], direction[z_axis],
        (-direction[x_axis] - direction[y_axis])};
    const ::elastica::Vec3 normal = ::elastica::normalize(normal_vector);

    StraightInit straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{n_elem},
        StraightInit::Material{my_material},
        StraightInit::Radius{::elastica::get<radius>(input_parameters)},
        StraightInit::Length{base_length},
        StraightInit::Origin{origin},
        StraightInit::Direction{direction},
        StraightInit::Normal{normal},
        StraightInit::ForceDampingRate{
            ::elastica::get<damping>(input_parameters)},
        StraightInit::TorqueDampingRate{
            ::elastica::get<damping>(input_parameters)},
    };
    RodRef rod_ref = simulator->emplace_back<CosseratRod>(
        straight_initializer.initialize<CosseratRod>());
    rod_refs.push_back(rod_ref);
  }
  final_time = ::elastica::get<final_sim_time>(input_parameters);

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
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)));

  // Set the bottom plane at (0, 0, 0) and with normal along Z
  const ::elastica::Vec3 plane_position{0.0, 0.0, 0.0};
  auto plane_normal = {0.0, 0.0, 1.0};
  auto plane_binormal = {0.0, 1.0, 0.0};
  auto plane_tangent = {1.0, 0.0, 0.0};
  // implicit convention
  ::elastica::Rot3 Q = {plane_normal, plane_binormal, plane_tangent};
  using Spans = typename ImposedPlane::Spans;
  imposed_plane_ref = simulator->emplace_back<ImposedPlane>(
      ::elastica::collision::PlanePrimitive(0UL, 0UL, true, plane_position, Q,
                                            Spans{1e4, 1e4},  // plane extent
                                            my_material),
      std::move(translations_buffer));

  // set near-list update time
  auto& collision_solver =
      simulator
          ->get_environment<elastica::environments::tags::CollisionSystem>();
  collision_solver->time_interval_between_detection() =
      1e-3;  // 0.02;  // anything > 0.0

  const ::elastica::real_t gravity_acc(
      ::elastica::get<gravitational_acc>(input_parameters));
  const ::elastica::Vec3 gravity_acc_vector{0.0, 0.0, gravity_acc};
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  simulator->finalize();
  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void FreeNestCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void FreeNestCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();
  using RodCoordinates =
      std::tuple<::elastica::real_t, ::elastica::real_t, ::elastica::real_t>;
  using Energies = std::tuple<::elastica::real_t, ::elastica::real_t>;
  std::vector<RodCoordinates> rod_coordinates;
  std::vector<Energies> energies;
  using TimesAndForces = std::tuple<::elastica::real_t, ::elastica::Vec3>;
  std::vector<TimesAndForces> times_and_forces;

  //  explicitly get the system
  const auto n_nodes = ::elastica::size(rod_refs[0].get()) + 1UL;
  const auto n_rod = rod_refs.size();
  constexpr auto x_axis = 0UL;
  constexpr auto y_axis = 1UL;
  constexpr auto z_axis = 2UL;

  const std::size_t n_steps = (final_time / current_dt());
  const std::size_t write_interval = n_steps / 100;  // 1000000UL;
  rod_coordinates.reserve(n_rod * n_nodes);
  energies.reserve(n_rod * 2);
  std::ofstream temporal_file;

  float current_progress = 0.0;
  for (std::size_t i = 0UL; i < n_steps; ++i) {
    // Progress Bar
    float progress = float(i) / float(n_steps);
    if (current_progress <= progress) {
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
      std::cout << "]" << int(progress * 100.0) << "%\r";
      std::cout.flush();
      current_progress += 1.0;
    }

    step();
    reset_ghost();
    if (not(current_iteration() % write_interval)) {  // Record energy
      for (auto const& rod_ref : rod_refs) {
        energies.emplace_back(
            ::elastica::compute_translational_energy(rod_ref.get()),
            ::elastica::compute_rotational_energy(rod_ref.get()));
      }
      // TODO : replace with a csv writer
      char file_name[100];
      sprintf(file_name, "data/rod_energies_%08d.csv", i);
      temporal_file.open(file_name);
      for (auto const& energy : energies) {
        temporal_file << std::setprecision(16) << std::get<0UL>(energy) << ","
                      << std::get<1UL>(energy) << "\n";
      }
      temporal_file << std::endl;
      temporal_file.close();
      energies.clear();
    }
    if (not(current_iteration() % write_interval)) {
      //       record position
      auto& imposed_plane = imposed_plane_ref.get();
      times_and_forces.emplace_back(current_time(), imposed_plane.get_force());
      for (auto const& rod_ref : rod_refs) {
        for (std::size_t k = 0; k < n_nodes; ++k) {
          rod_coordinates.emplace_back(
              ::elastica::position(rod_ref.get(), k)[x_axis],
              ::elastica::position(rod_ref.get(), k)[y_axis],
              ::elastica::position(rod_ref.get(), k)[z_axis]);
        }
        energies.emplace_back(
            ::elastica::compute_translational_energy(rod_ref.get()),
            ::elastica::compute_rotational_energy(rod_ref.get()));
      }
      // TODO : replace with a csv writer
      char file_name[100];
      sprintf(file_name, "data/rod_coordinates_%08d.csv", i);
      temporal_file.open(file_name);
      for (auto const& rod_coord : rod_coordinates) {
        temporal_file << std::setprecision(16) << std::get<x_axis>(rod_coord)
                      << "," << std::get<y_axis>(rod_coord) << ","
                      << std::get<z_axis>(rod_coord) << "\n";
      }
      temporal_file << std::endl;
      temporal_file.close();
      rod_coordinates.clear();
    }
  }
  // TODO : replace with a csv writer
  std::ofstream nest_diagnostics_file;
  nest_diagnostics_file.open("data/nest_diagnostics.csv");
  constexpr auto Time = 0UL;
  constexpr auto ForceVector = 1UL;
  for (auto const& tf : times_and_forces) {
    nest_diagnostics_file << std::setprecision(16) << std::get<Time>(tf) << ","
                          << std::get<ForceVector>(tf)[x_axis] << ","
                          << std::get<ForceVector>(tf)[y_axis] << ","
                          << std::get<ForceVector>(tf)[z_axis] << "\n";
  }
  nest_diagnostics_file << std::endl;
  nest_diagnostics_file.close();
  times_and_forces.clear();
}

void FreeNestCaseStudy::reset_ghost() {
  simulator->for_each_block([](auto& block) {
    block.template fill_ghosts_for<::elastica::tags::Velocity>();
  });
  simulator->for_each_block([](auto& block) {
    block.template fill_ghosts_for<::elastica::tags::AngularVelocity>();
  });
  simulator->for_each_block([](auto& block) {
    block.template fill_ghosts_for<::elastica::tags::Acceleration>();
  });
  simulator->for_each_block([](auto& block) {
    block.template fill_ghosts_for<::elastica::tags::AngularAcceleration>();
  });
}
