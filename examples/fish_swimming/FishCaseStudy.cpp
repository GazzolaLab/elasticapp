#include "FishCaseStudy.hpp"

#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>

void FishCaseStudy::setup(InputTypes<This> const& input_parameters) {
  ::elastica::MaterialID rod_material = elastica::create_material(
      ::elastica::get<density>(input_parameters), 0.24,
      ::elastica::get<youngs_modulus>(input_parameters),
      ::elastica::get<youngs_modulus>(input_parameters) /
          ::elastica::real_t(3.0),
      200, 1e-10, 1e-10, 0.1, 0.1);

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const std::size_t n_elems = ::elastica::get<n_elements>(input_parameters);
  const std::size_t n_fish = ::elastica::get<num_fish>(input_parameters);
  std::size_t n_nodes = n_elems + 1UL;

  /* Initialize the fishes (actual + virtual) */
  using StraightInit =
      ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;
  for (std::size_t i = 0; i < n_fish; ++i) {
    StraightInit straight_initializer{
        StraightInit::NElement{n_elems},
        StraightInit::Material{rod_material},
        StraightInit::Radius{base_length * 0.01},  // overwritten internally
        StraightInit::Length{base_length},
        StraightInit::Origin{
            ::blaze::column(::elastica::get<origins>(input_parameters), i)},
        StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
    };

    elastic_fish.emplace_back(simulator->template emplace_back<RodType>(
        straight_initializer.template initialize<RodType>()));
    virtual_fish.emplace_back(simulator->template emplace_back<RodType>(
        straight_initializer.template initialize<RodType>()));
  }

  /* Prescribe the virtual fishes' motion */
  for (auto& vf : virtual_fish) {
    simulator->constrain(vf).at(0UL).with(
        ::fish::constraint::make_carling_fish_constraint(
            vf,
            ::elastica::get<period>(input_parameters),  // period
            2 * M_PI / base_length,                     // wave number
            0.0,                                        // phase shift
            ::elastica::get<period>(input_parameters)   // ramp up time
            ));
  }

  /* Connect the virtual and actual fish */
  using Tag = ::elastica::tags::ShearStretchRigidityMatrix;
  real_t const conn_k = ::elastica::get<Tag>(elastic_fish[0].get())(2, 0);
  real_t const y_idx = 1;
  namespace econ = ::elastica::connections;
  auto connection_generator = econ::make_connection(
      econ::make_force([=](auto /*time*/, auto& suitor,
                           std::size_t /*rod_one_index*/, auto& leader,
                           auto /*rod_two_index*/) {
        using ETag = ::elastica::tags::ExternalLoads;
        ::blaze::row(::elastica::get<ETag>(suitor), y_idx) +=
            conn_k * (::blaze::row(::elastica::position(leader), y_idx) -
                      ::blaze::row(::elastica::position(suitor), y_idx));
      }),
      econ::make_torque([=](auto /*time*/, auto& suitor,
                            std::size_t /*rod_one_index*/, auto& leader,
                            auto /*rod_two_index*/) {
        using TraitOp =
            ::elastica::cosserat_rod::DefaultCosseratRodTraits::Operations;
        using ITag = ::elastica::tags::InternalTorques;
        using ETag = ::elastica::tags::ExternalTorques;

        // tau_1 + Q_1 * Q_2^T * m_2 = 0
        auto&& tmp1 =
            ::elastica::get<::elastica::tags::_DummyElementVector>(leader);
        auto&& tmp2 =
            ::elastica::get<::elastica::tags::_DummyElementVector>(suitor);
        TraitOp::batch_mattranspvec(tmp1, ::elastica::director(leader),
                                    ::elastica::get<ITag>(leader));
        TraitOp::batch_matvec(tmp2, ::elastica::director(suitor), tmp1);
        ::elastica::get<ETag>(suitor) -= tmp2;
      }));

  using ::elastica::interface::at;
  for (std::size_t i = 0; i < n_fish; ++i) {
    simulator->connect(elastic_fish[i], at(0UL))
        .to(virtual_fish[i], at(0UL))
        .with(connection_generator());
  }

  /* Apply flow forces onto the elastic fish */
  namespace efo = ::elastica::forcing;
  flow_forces = ::blaze::zero<real_t>(3UL, n_fish * n_nodes);
  flow_torques = ::blaze::zero<real_t>(3UL, n_fish * n_elems);
  for (std::size_t i = 0; i < n_fish; ++i) {
    auto flow_force =
        blaze::submatrix(flow_forces, 0UL, i * n_nodes, 3UL, n_nodes);
    auto flow_torque =
        blaze::submatrix(flow_torques, 0UL, i * n_elems, 3UL, n_elems);
    simulator->force(elastic_fish[i])
        .at(0UL)
        .with(efo::make_forcing(
            efo::make_force([=](auto /*time*/, auto& rod_system,
                                std::size_t /*index*/) {
              ::elastica::get<::elastica::tags::ExternalLoads>(rod_system) +=
                  flow_force;
            }),
            efo::make_torque([=](auto /*time*/, auto& rod_system,
                                 std::size_t /*index*/) {
              ::elastica::get<::elastica::tags::ExternalTorques>(rod_system) +=
                  flow_torque;
            }))());
  }

  /* Finalize the simulator*/
  simulator->finalize();

  /* Set the dt value */
  real_t dt =
      ::elastica::get<dt_scale>(input_parameters) * base_length / n_elems;
  set_dt(dt);
}

void FishCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void FishCaseStudy::run() {}
