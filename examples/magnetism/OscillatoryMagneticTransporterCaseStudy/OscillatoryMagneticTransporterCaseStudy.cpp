//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic cilia carpet study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale- All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "OscillatoryMagneticTransporterCaseStudy.hpp"

#include <cmath>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>

template <typename Traits, template <typename> class ComputationalBlock,
          template <typename /*CRT*/, typename /*InitializedBlock*/>
          class... Components>
auto make_analytical_damper(
    elastica::cosserat_rod::CosseratRodPlugin<Traits, ComputationalBlock,
                                              Components...> const& rod_like,
    elastica::real_t const damping_coeff, elastica::real_t const dt) {
  using elastica::real_t;

  // ===========ANALYTICAL DAMPING CODE START============================
  const real_t translational_mode_damping_coeff =
      blaze::exp(-damping_coeff * dt);

  auto const& rod = rod_like.self();
  auto&& mass = ::elastica::get<elastica::tags::Mass>(rod);
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
              row_idx, column_idx));
    }
  }

  // setting analytical damper via constraint
  namespace eco = ::elastica::constraints;
  auto analytical_linear_damper = eco::make_constraint(
      eco::make_value_constraint(
          [](auto /*time*/, auto& /*rod_system*/, std::size_t /*index*/) {}),
      eco::make_rate_constraint(
          [translational_mode_damping_coeff, rotational_mode_damping_coeff](
              auto /*time*/, auto& rod_system, std::size_t /*index*/) {
            ::elastica::velocity(rod_system) =
                ::elastica::velocity(rod_system) *
                translational_mode_damping_coeff;
            ::elastica::angular_velocity(rod_system) =
                ::elastica::angular_velocity(rod_system) %
                rotational_mode_damping_coeff;
          }));
  // ===========ANALYTICAL DAMPING CODE END============================
  return analytical_linear_damper;
}

namespace elastica {

  namespace environments {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename Traits>
    void resolve(elastica::environments::GravityField<Traits> const&,
                 elastica::cosserat_rod::detail::
                     MagnetizedCosseratRodWithoutDampingBlock&) {
      // FIXME : does CRTP work here?
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica

void OscillatoryMagneticTransporterCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  const ::elastica::real_t friction_coefficient(
      ::elastica::get<friction_coeff>(input_parameters));

  elastica::MaterialID my_material = elastica::create_material(
      // material density
      2.39e3,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping_normal>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping_tangential>(input_parameters),
      // coefficient of static friction
      friction_coefficient,
      // coefficient of dynamic friction
      friction_coefficient);

  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const std::size_t n_elem = ::elastica::get<n_elements>(input_parameters);
  const std::size_t num_rods_along_x =
      ::elastica::get<n_rods_along_x>(input_parameters);
  const std::size_t num_rods_along_y =
      ::elastica::get<n_rods_along_y>(input_parameters);
  const std::size_t num_rods = num_rods_along_x * num_rods_along_y;
  const ::elastica::real_t cilia_carpet_spacing =
      ::elastica::get<carpet_spacing>(input_parameters);
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavelength_along_x =
          ::elastica::get<carpet_magnetization_spatial_wavelength_along_x>(
              input_parameters);
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavelength_along_y =
          ::elastica::get<carpet_magnetization_spatial_wavelength_along_y>(
              input_parameters);
  const ::elastica::real_t cilia_carpet_magnetization_spatial_phase_difference =
      ::elastica::get<carpet_magnetization_spatial_phase_difference>(
          input_parameters);

  ::elastica::Vec3 updir{0.0, 0.0, 1.0};
  ::elastica::Vec3 normaldir{1.0, 0.0, 0.0};

  constexpr auto x_axis = 0UL;
  constexpr auto y_axis = 1UL;
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavenumber_along_x =
          2 * M_PI / cilia_carpet_magnetization_spatial_wavelength_along_x;
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavenumber_along_y =
          2 * M_PI / cilia_carpet_magnetization_spatial_wavelength_along_y;

  elastica::real_t sphere_radius =
      elastica::get<sphere_radius_to_rod_radius>(input_parameters) *
      elastica::get<radius>(input_parameters);
  auto const top_layer_nd_width(
      ::elastica::get<top_layer_nondimensional_width>(input_parameters));
  bool top_layer = top_layer_nd_width > 0.0;
  bool staggered(true);

  for (std::size_t i = 0; i < num_rods; ++i) {
    ::elastica::Vec3 rod_origin{0.0, 0.0, 0.0};
    rod_origin[x_axis] += (i % num_rods_along_x) * cilia_carpet_spacing;
    rod_origin[y_axis] += (i / num_rods_along_x) * cilia_carpet_spacing;
    const ::elastica::real_t rod_magnetization_angle_along_x =
        cilia_carpet_magnetization_spatial_phase_difference +
        rod_origin[x_axis] *
            cilia_carpet_magnetization_spatial_wavenumber_along_x;
    const ::elastica::real_t rod_magnetization_angle_along_y =
        cilia_carpet_magnetization_spatial_phase_difference +
        rod_origin[y_axis] *
            cilia_carpet_magnetization_spatial_wavenumber_along_y;
    const ::elastica::Vec3 magnetization_vector{
        std::sin(rod_magnetization_angle_along_x),
        std::sin(rod_magnetization_angle_along_y),
        std::cos(rod_magnetization_angle_along_x) +
            std::cos(rod_magnetization_angle_along_y)};
    const ::elastica::Vec3 magnetization_direction =
        blaze::normalize(magnetization_vector);
    using RodInitializer = ::elastica::cosserat_rod::
        StraightUniformMagnetizedCosseratRodWithoutDampingInitializer;
    RodInitializer rod_initializer{
        // Rod Initializer Wrapper
        RodInitializer::NElement{n_elem},
        RodInitializer::Material{my_material},
        RodInitializer::Radius{::elastica::get<radius>(input_parameters)},
        RodInitializer::Length{base_length},
        RodInitializer::Origin{rod_origin},
        RodInitializer::Direction{updir},
        RodInitializer::Normal{normaldir},
        RodInitializer::MagnetizationDensity{
            ::elastica::get<magnetization_density>(input_parameters)},
        RodInitializer::UniformMagnetizationDirectionInWorldFrame{
            magnetization_direction}};

    // This is to store
    RodRef rod_ref = simulator->emplace_back<MagnetizedCosseratRod>(
        rod_initializer.initialize<MagnetizedCosseratRod>());
    rod_refs.push_back(rod_ref);

    namespace eco = ::elastica::constraints;
    const std::size_t my_constraint_index = 0UL;
    auto constraint_generator = eco::make_constraint(
        eco::make_value_constraint(
            [origin = rod_origin,
             initial_director = ::elastica::Rot3{::elastica::director(
                 rod_ref.get(), my_constraint_index)}](
                auto /*time*/, auto& rod_system, auto index) {
              ::elastica::position(rod_system, index) = origin;
              ::elastica::director(rod_system, index) = initial_director;
            }),
        eco::make_rate_constraint(
            [](auto /*time*/, auto& rod_system, std::size_t index) {
              ::elastica::velocity(rod_system, index) = 0.0;
              ::elastica::angular_velocity(rod_system, index) = 0.0;
            }));

    simulator->constrain(rod_ref)
        .at(my_constraint_index)
        .with(constraint_generator());

    if (top_layer) {
      ::elastica::Vec3 another_origin{rod_origin +
                                      base_length * top_layer_nd_width * updir};
      if (staggered)
        another_origin +=
            0.5 * cilia_carpet_spacing * ::elastica::Vec3{1.0, 1.0, 0.0};
      RodRef another_ref = simulator->emplace_back<MagnetizedCosseratRod>(
          rod_initializer
              .but_with(
                  RodInitializer::Origin{another_origin},
                  RodInitializer::Direction{-updir},
                  RodInitializer::UniformMagnetizationDirectionInWorldFrame{
                      -magnetization_direction})
              .initialize<MagnetizedCosseratRod>());
      rod_refs.push_back(another_ref);

      auto another_constraint_generator = eco::make_constraint(
          eco::make_value_constraint(
              [origin = another_origin,
               initial_director = ::elastica::Rot3{::elastica::director(
                   another_ref.get(), my_constraint_index)}](
                  auto /*time*/, auto& rod_system, auto index) {
                ::elastica::position(rod_system, index) = origin;
                ::elastica::director(rod_system, index) = initial_director;
              }),
          eco::make_rate_constraint(
              [](auto /*time*/, auto& rod_system, std::size_t index) {
                ::elastica::velocity(rod_system, index) = 0.0;
                ::elastica::angular_velocity(rod_system, index) = 0.0;
              }));

      simulator->constrain(another_ref)
          .at(my_constraint_index)
          .with(another_constraint_generator());
    }  // top layer
  }

  // Setup spheres
  {
    elastica::MaterialID sphere_material = elastica::create_material(
        // material density
        ::elastica::get<sphere_density_to_rod_density>(input_parameters) *
            elastica::Material::get_density(my_material),
        // coefficient of restitution
        0.24,
        // Young's modulus
        elastica::Material::get_youngs_modulus(my_material),
        // Shear modulus
        elastica::Material::get_shear_modulus(my_material),
        // Contact stiffness
        elastica::Material::get_stiffness(my_material),
        // normal contact damping
        elastica::Material::get_normal_damping(my_material),
        // tangent contact damping
        elastica::Material::get_tangential_damping(my_material),
        // coefficient of static friction
        friction_coefficient,
        // coefficient of dynamic friction
        friction_coefficient);

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

    const auto num_rods_along_y_to_num_spheres_along_y = 2UL;
    const auto num_spheres_along_y =
        num_rods_along_y / num_rods_along_y_to_num_spheres_along_y;

    for (auto idx = 1UL; idx < num_spheres_along_y; ++idx) {
      for (auto x_idx = 1UL; x_idx < num_rods_along_x; ++x_idx) {
        elastica::Vec3 sphere_origin =
            elastica::position(
                rod_refs[num_rods_along_y_to_num_spheres_along_y *
                             (top_layer ? 2UL : 1UL) * idx * num_rods_along_x +
                         // Offset by 1 to place inside grid
                         (top_layer ? 2UL : 1UL) * x_idx]
                    .get(),
                n_elem) +
            1.1 * sphere_radius * updir + 0.1 * sphere_radius * normaldir;
        sphere_refs.push_back(simulator->template emplace_back<Sphere>(
            initialize_sphere(sphere_origin, sphere_radius)));
      }
    }
  }

  for (auto rod_ref : rod_refs) {
    simulator->constrain(rod_ref).at(0UL).with(make_analytical_damper(
        rod_ref.get(), elastica::get<damping_rate>(input_parameters),
        ::elastica::get<dt>(input_parameters))());
  }

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

  // **************************************************//
  /* Gravity */
  const ::elastica::Vec3 gravity_acc_vector{
      0.0, 0.0, ::elastica::get<acc_gravity>(input_parameters)};
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  // **************************************************//

  final_time = ::elastica::get<final_sim_time>(input_parameters);
  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void OscillatoryMagneticTransporterCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void OscillatoryMagneticTransporterCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();

  const std::size_t n_steps = (final_time / current_dt());

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();
  }
}
