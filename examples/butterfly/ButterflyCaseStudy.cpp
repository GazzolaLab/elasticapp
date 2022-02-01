//==============================================================================
/*!
//  \file
//  \brief Butterfly case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "ButterflyCaseStudy.hpp"

#include <algorithm>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Identity.hpp>
#include <elastica/Utilities/Math/Rot3.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>

void ButterflyCaseStudy::setup(InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // PyElastica values
      // material density
      5e3,
      // coefficient of restitution
      0.24,
      // Young's modulus
      1e4,
      // Shear modulus
      1e4 / 1.5,
      // The below parameters doesn't change the result of this example
      // coefficient of static friction
      100.0,  // needs to be > 0.0
      // cofficient of dynamic friction
      20.0,  // needs to be > 0.0
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10);

  const std::size_t n_elem(elastica::get<n_elements>(input_parameters));

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const ::elastica::real_t dl = base_length / n_elem;
  const std::size_t half_n_elem = n_elem / 2UL;

  const ::elastica::Vec3 horizontal_direction({0.0, 0.0, 1.0});
  const ::elastica::Vec3 vertical_direction({1.0, 0.0, 0.0});
  const ::elastica::real_t bend_angle(
      elastica::get<angle_of_inclination>(input_parameters));

  auto position_profile = [=](std::size_t index) -> ::elastica::Vec3 {
    const auto indices = std::minmax({index, half_n_elem});
    return origin +
           (dl * static_cast<::elastica::real_t>(indices.first) *
            (std::cos(bend_angle) * horizontal_direction +
             std::sin(bend_angle) * vertical_direction)) +
           (dl * static_cast<::elastica::real_t>(indices.second - half_n_elem) *
            (std::cos(bend_angle) * horizontal_direction -
             std::sin(bend_angle) * vertical_direction));
  };

  auto director_profile = [=](std::size_t index) -> ::elastica::Rot3 {
    // clang-format off
        if (index < half_n_elem) {
          return ::elastica::Rot3(
              {{ 0.0            , 1.0, 0.0                 },
               {-std::sin(bend_angle), 0.0, std::cos(bend_angle)},
               { std::cos(bend_angle), 0.0, std::sin(bend_angle)}}
          );
        } else {
          return ::elastica::Rot3(
              {{ 0.0            , 1.0, 0.0                 },
               {-std::sin(bend_angle), 0.0,-std::cos(bend_angle)},
               {-std::cos(bend_angle), 0.0, std::sin(bend_angle)}}
          );
        }
    // clang-format on
  };

  using Init =
      ::elastica::cosserat_rod::UserDefinedCosseratRodWithoutDampingInitializer<
          decltype(position_profile), decltype(director_profile)>;

  Init initializer{
      // Initialization Wrapper
      Init::NElement{n_elem},
      Init::Material{my_material},
      Init::Radius{0.25},
      Init::Position(std::move(position_profile)),
      Init::Director(std::move(director_profile)),
  };

  rod_ref = simulator->emplace_back<CosseratRodWithoutDamping>(
      initializer.initialize<CosseratRodWithoutDamping>());

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void ButterflyCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt());
}

void ButterflyCaseStudy::run() {
  // TODO : replace with event and triggers
  using Temporal = ::elastica::real_t;
  using BendEnergy = ::elastica::real_t;
  using ShearEnergy = ::elastica::real_t;
  using TranslationEnergy = ::elastica::real_t;
  using RotationEnergy = ::elastica::real_t;
  using TimesAndEnergies = std::tuple<Temporal, BendEnergy, ShearEnergy,
                                      TranslationEnergy, RotationEnergy>;
  std::vector<TimesAndEnergies> stats;

  const ::elastica::real_t final_time(40.0);

  const std::size_t n_steps = (final_time / current_dt());

  constexpr std::size_t write_interval = 200UL;
  stats.reserve(n_steps / write_interval);

  // you can use get_current_iteration() instead of current_iteration()
  // current_iteration() ensures const-correctness
  for (; current_iteration() < n_steps; ++get_current_iteration()) {
    step();

    if (not(current_iteration() % write_interval)) {
      stats.push_back(std::tuple_cat(std::make_tuple(current_time()),
                                     compute_elastic_energy(),
                                     compute_kinetic_energy()));
    }
  }

  // TODO : replace with a csv writer
  std::ofstream my_file;
  my_file.open("energies.csv");
  constexpr auto temporal = 0UL;
  constexpr auto bend_elastic_energy = 1UL;
  constexpr auto shear_elastic_energy = 2UL;
  constexpr auto translation_energy = 3UL;
  constexpr auto rotation_energy = 4UL;
  for (auto const& st : stats) {
    my_file << std::setprecision(16)  //
            << std::get<temporal>(st) << ","
            << std::get<bend_elastic_energy>(st) << ","
            << std::get<shear_elastic_energy>(st) << ", "
            << std::get<translation_energy>(st) << ","
            << std::get<rotation_energy>(st) << "\n";
  }
  my_file << std::endl;
  my_file.close();
}