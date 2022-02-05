//==============================================================================
/*!
//  \file
//  \brief Axial stretching case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "AxialStretchingCaseStudy.hpp"

#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Size.hpp>
#include <iomanip>
#include <ostream>
#include <utility>  // pair

void AxialStretchingCaseStudy::setup(InputTypes<This> const& input_parameters) {
  //  void AxialStretchingCaseStudy::setup(
  //      InputTypes<AxialStretchingCaseStudy> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      1e3,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      1e4,  // doesn't matter
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
      StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
      StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::ForceDampingCoefficient{
          ::elastica::get<damping>(input_parameters)},
      StraightInit::TorqueDampingCoefficient{
          ::elastica::get<damping>(input_parameters)},
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

  simulator->constrain(rod_ref).at(0UL).with(constraint_generator());

  namespace efo = ::elastica::forcing;

  const ::elastica::Vec3 F{::elastica::get<applied_force>(input_parameters),
                           0.0, 0.0};
  auto force_generator = efo::make_forcing(
      efo::make_force([F](auto /*time*/, auto& rod_system, std::size_t index) {
        using Tag = ::elastica::tags::ExternalLoads;
        blaze::column(::elastica::get<Tag>(rod_system), index + 1UL) = F;
      }),
      efo::make_torque([](auto /*time*/, auto&, std::size_t) {}));

  simulator->force(rod_ref).at(::elastica::end - 1UL).with(force_generator());

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void AxialStretchingCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void AxialStretchingCaseStudy::run() {
  // TODO : replace with event and triggers
  using TimesAndDisplacements =
      std::pair<::elastica::real_t, ::elastica::real_t>;
  std::vector<TimesAndDisplacements> time_and_disps;

  const ::elastica::real_t final_time(20.0);
  // explicitly get the system
  const auto n_elements = ::elastica::size(rod_ref.get());
  constexpr auto x_axis = 0UL;

  const auto n_iterations = static_cast<std::size_t>(final_time / current_dt());

  const std::size_t write_interval = 200UL;
  time_and_disps.reserve(n_iterations / write_interval);

  // you can use get_current_iteration() instead of current_iteration()
  // current_iteration() ensures const-correctness
  for (std::size_t i = 0UL; i < n_iterations; ++i) {
    step();

    if (not(current_iteration() % write_interval)) {
      time_and_disps.emplace_back(
          current_time(),
          ::elastica::position(rod_ref.get(), n_elements)[x_axis]);
    }
  }

  // TODO : replace with a csv writer
  std::ofstream my_file;
  my_file.open("tip_displacements.csv");
  for (auto const& td : time_and_disps) {
    my_file << std::setprecision(16) << td.first << "," << td.second << "\n";
  }
  my_file << std::endl;
  my_file.close();
}
