//==============================================================================
/*!
//  \file
//  \brief Timoshenko case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "TimoshenkoCaseStudy.hpp"

#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Rot3.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Rampup.hpp>
#include <elastica/Utilities/Size.hpp>
//
#include <algorithm>  // equal
#include <cctype>     // tolower()
#include <functional>
#include <utility>

void TimoshenkoCaseStudy::setup(InputTypes<This> const& input_parameters) {
  if (simulator->ready()) {
    simulator->reset();
  }

  elastica::MaterialID my_material = elastica::create_material(
      // PyElastica values
      // material density
      5e3,
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // The below parameters doesn't change the result of this example
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // cofficient of dynamic friction
      0.1);

  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const std::size_t n_elem(::elastica::get<n_elements>(input_parameters));

  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},
      StraightInit::Material{my_material},
      StraightInit::Radius{::elastica::get<radius>(input_parameters)},
      StraightInit::Length{base_length},
      StraightInit::Origin{origin},
      StraightInit::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
      StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::ForceDampingRate{
          ::elastica::get<damping_rate>(input_parameters)},
      StraightInit::TorqueDampingRate{
          ::elastica::get<damping_rate>(input_parameters)},
  };

  rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());

  namespace eco = ::elastica::constraints;
  const std::size_t my_constraint_index = 0UL;
  auto constraint_generator = eco::make_constraint(
      eco::make_value_constraint(
          [origin, origin_frame = ::elastica::Rot3{::elastica::director(
                       rod_ref.get(), my_constraint_index)}](
              auto /*time*/, auto& rod_system, auto index) {
            ::elastica::position(rod_system, index) = origin;
            ::elastica::director(rod_system, index) = origin_frame;
          }),
      eco::make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
            ::elastica::angular_velocity(rod_system, index) = 0.0;
          }));

// #define TEST_VIEWS  // Uncomment if connection to a block view is needed
#ifdef TEST_VIEWS
  auto view_ref = blocks::slice(rod_ref.parent(), 0UL, 1UL);
  simulator->constrain(view_ref).at(0UL).with(constraint_generator());
#else
  simulator->constrain(rod_ref).at(0UL).with(constraint_generator());
#endif  // TEST_VIEWS

  namespace efo = ::elastica::forcing;

  // setup ramping
  auto ramping_fn = [&](::elastica::real_t start_time,
                        ::elastica::real_t rampup_time) {
    using RuntimeProfile =
        std::function<::elastica::real_t(::elastica::real_t)>;
    RuntimeProfile ramp;

    auto iequals = [](std::string const& a, std::string const& b) -> bool {
      return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                        [](unsigned char a, unsigned char b) {
                          return std::tolower(a) == std::tolower(b);
                        });
    };

    if (iequals(::elastica::get<rampup_profile>(input_parameters),
                rampup_profile::default_value()))
      ramp = elastica::make_linear_rampup(start_time, rampup_time);
    else
      ramp = elastica::make_sin_rampup(start_time, rampup_time);

    return ramp;
  };

  const ::elastica::Vec3 F{::elastica::get<applied_force>(input_parameters),
                           0.0, 0.0};
  ::elastica::real_t start_time{0.0};
  const auto rampup_time{::elastica::get<rampup_interval>(input_parameters)};

  auto force_generator = efo::make_forcing(
      efo::make_force([F, ramp = ramping_fn(start_time, rampup_time)](
                          auto time_v, auto& rod_system, std::size_t index) {
        using Tag = ::elastica::tags::ExternalLoads;
        blaze::column(::elastica::get<Tag>(rod_system), index + 1) =
            F * ramp(static_cast<::elastica::real_t>(time_v));
      }),
      efo::make_torque([](auto /*time*/, auto&, std::size_t) {}));
  simulator->force(rod_ref).at(::elastica::end - 1UL).with(force_generator());

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
  n_steps_to_run = ::elastica::get<n_steps>(input_parameters);
}

void TimoshenkoCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void TimoshenkoCaseStudy::run() {
  // TODO : replace with event and triggers
  //  using TimesAndDisplacements =
  //      std::pair<::elastica::real_t, ::elastica::real_t>;
  //  std::vector<TimesAndDisplacements> time_and_disps;

  const ::elastica::real_t final_time(500.0);
  const auto n_elements = ::elastica::size(rod_ref.get());
  constexpr auto x_axis = 0UL;

  const auto n_iterations =
      n_steps_to_run ? n_steps_to_run
                     : static_cast<std::size_t>(final_time / current_dt());

  const std::size_t write_interval = 200UL;
  //  time_and_disps.reserve(n_steps / write_interval);

  run_for_n_iterations(n_iterations);

  // TODO : replace with a csv writer
  //  std::ofstream my_file;
  //  my_file.open("tip_displacements.csv");
  //  for (auto const& td : time_and_disps) {
  //    my_file << std::setprecision(16) << td.first << "," << td.second <<
  //    "\n";
  //  }
  //  my_file << std::endl;
  //  my_file.close();
}
