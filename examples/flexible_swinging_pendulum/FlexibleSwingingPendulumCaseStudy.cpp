//==============================================================================
/*!
//  \file
//  \brief Flexible swinging pendulum case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale - All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "FlexibleSwingingPendulumCaseStudy.hpp"

#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Size.hpp>
//
#include <iomanip>  // run()
#include <ostream>  // run()
#include <tuple>
#include <vector>

void FlexibleSwingingPendulumCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      1100,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // Properties below don't matter
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

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const std::size_t n_elem = ::elastica::get<n_elements>(input_parameters);

  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},
      StraightInit::Material{my_material},
      StraightInit::Radius{::elastica::get<radius>(input_parameters)},
      StraightInit::Length{base_length},
      StraightInit::Origin{origin},
      StraightInit::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
      StraightInit::Normal{::elastica::Vec3{1.0, 0.0, 0.0}},
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
          [origin](auto /*time*/, auto& rod_system, auto index) {
            ::elastica::position(rod_system, index) = origin;
          }),
      eco::make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
          }));

  simulator->constrain(rod_ref)
      .at(my_constraint_index)
      .with(constraint_generator());

  namespace efo = ::elastica::forcing;

  const ::elastica::Vec3 gravity_acc_vector{
      ::elastica::get<gravitational_acc>(input_parameters), 0.0, 0.0};
  auto& gravity_environment =
      simulator
          ->get_environment<::elastica::environments::tags::GravityField>();
  gravity_environment->gravity() = gravity_acc_vector;

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void FlexibleSwingingPendulumCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void FlexibleSwingingPendulumCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();
  // TODO : replace with event and triggers
  using TimesAndDisplacements =
      std::tuple<::elastica::real_t, ::elastica::real_t, ::elastica::real_t>;
  std::vector<TimesAndDisplacements> time_and_disps;

  const ::elastica::real_t final_time(5.0);
  // explicitly get the system
  const auto n_elements = ::elastica::size(rod_ref.get());
  constexpr auto x_axis = 0UL;
  constexpr auto z_axis = 2UL;

  const std::size_t n_steps = (final_time / current_dt());

  const std::size_t write_interval = 200UL;
  time_and_disps.reserve(n_steps / write_interval);

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();

    if (not(current_iteration() % write_interval)) {
      time_and_disps.emplace_back(
          current_time(),
          ::elastica::position(rod_ref.get(), n_elements)[x_axis],
          ::elastica::position(rod_ref.get(), n_elements)[z_axis]);
    }
  }

  // TODO : replace with a csv writer
  std::ofstream my_file;
  my_file.open("tip_displacements.csv");
  constexpr auto Time = 0UL;
  constexpr auto PendulumTipXCoordinate = 1UL;
  constexpr auto PendulumTipZCoordinate = 2UL;
  for (auto const& td : time_and_disps) {
    my_file << std::setprecision(16) << std::get<Time>(td) << ","
            << std::get<PendulumTipXCoordinate>(td) << ","
            << std::get<PendulumTipZCoordinate>(td) << "\n";
  }
  my_file << std::endl;
  my_file.close();
}
