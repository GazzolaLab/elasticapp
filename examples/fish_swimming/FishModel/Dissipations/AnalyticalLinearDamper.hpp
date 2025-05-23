#pragma once

#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Interface/Systems/BlockRefs.hpp>
#include <elastica/Systems/CosseratRods/CosseratRodPlugin.hpp>
#include <elastica/Systems/Tags.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Get.hpp>

namespace fish {

  namespace detail {

    /* Physical version */
    template <typename Traits, template <typename> class ComputationalBlock,
              template <typename /*CRT*/,
                        typename /*InitializedBlock*/> class... Components>
    decltype(auto) make_damping_constraint(
        ::elastica::cosserat_rod::CosseratRodPlugin<
            Traits, ComputationalBlock, Components...> const& rod_like,
        ::elastica::real_t translational_damping_coeff,
        ::elastica::real_t rotational_damping_coeff, ::elastica::real_t dt) {
      namespace eco = ::elastica::constraints;
      using ::elastica::real_t;
      auto const& rod = rod_like.self();

      auto&& nodal_mass{::elastica::get<::elastica::tags::Mass>(rod)};
      auto&& inv_moi{
          ::elastica::get<::elastica::tags::InvMassSecondMomentOfInertia>(rod)};

      constexpr std::size_t dim{3UL};
      const auto n_elem{cpp17::size(rod)};
      const auto n_node{n_elem + 1UL};

      // Compute exponential damping constants
      // Rotational damping constants is still missing dilatation term,
      // which is dynamically computed in the constraint lambdas
      blaze::DynamicVector<real_t> translational_damping_constant(n_node);
      blaze::DynamicMatrix<real_t> rotational_damping_constant(dim, n_elem);

      for (std::size_t j = 0UL; j < n_node; ++j) {
        translational_damping_constant[j] =
            std::exp(-translational_damping_coeff * dt);
      }

      for (std::size_t i = 0UL; i < dim; ++i) {
        for (std::size_t j = 0UL; j < n_elem; ++j) {
          real_t element_mass = 0.5 * (nodal_mass[j] + nodal_mass[j + 1]);
          if (j == 0)
            element_mass += 0.5 * nodal_mass[j];
          if (j == (n_elem - 1))
            element_mass += 0.5 * nodal_mass[j + 1];

          rotational_damping_constant(i, j) = std::exp(
              -rotational_damping_coeff * element_mass * inv_moi(i, j) * dt);
        }
      }

      return eco::make_constraint(
          eco::make_value_constraint([](auto /*time*/, auto& /*rod_system*/,
                                        std::size_t /*index*/) {}),
          eco::make_rate_constraint([=](auto /*time*/, auto& rod_system,
                                        std::size_t) {
            auto&& vel = ::elastica::velocity(rod_system);
            auto&& omega = ::elastica::angular_velocity(rod_system);
            auto&& dil =
                ::blocks::get<elastica::tags::ElementDilatation>(rod_system);

            for (std::size_t i = 0UL; i < dim; ++i) {
              for (std::size_t j = 0UL; j < n_elem + 1UL; ++j) {
                vel(i, j) *= translational_damping_constant[j];
              }

              for (auto j = 0UL; j < n_elem; ++j) {
                omega(i, j) *=
                    std::pow(rotational_damping_constant(i, j), dil[j]);
              }
            }
          }))();
    }

    /* Simplified version */
    template <typename Traits, template <typename> class ComputationalBlock,
              template <typename /*CRT*/,
                        typename /*InitializedBlock*/> class... Components>
    decltype(auto) make_damping_constraint(
        ::elastica::cosserat_rod::CosseratRodPlugin<
            Traits, ComputationalBlock, Components...> const& rod_like,
        ::elastica::real_t damping_coeff, ::elastica::real_t dt) {
      namespace eco = ::elastica::constraints;
      using ::elastica::real_t;
      auto const& rod = rod_like.self();

      const real_t damping_constant{std::exp(-damping_coeff * dt)};

      return eco::make_constraint(
          eco::make_value_constraint([](auto /*time*/, auto& /*rod_system*/,
                                        std::size_t /*index*/) {}),
          eco::make_rate_constraint(
              [damping_constant](auto /*time*/, auto& rod_system, std::size_t) {
                ::elastica::velocity(rod_system) *= damping_constant;
                ::elastica::angular_velocity(rod_system) *= damping_constant;
              }))();
    }

    /* Original version */
    template <typename Traits, template <typename> class ComputationalBlock,
              template <typename /*CRT*/,
                        typename /*InitializedBlock*/> class... Components>
    decltype(auto) make_damping_constraint(
        ::elastica::cosserat_rod::CosseratRodPlugin<
            Traits, ComputationalBlock, Components...> const& rod_like,
        ::elastica::real_t damping_coeff_dt) {
      namespace eco = ::elastica::constraints;
      using ::elastica::real_t;
      auto const& rod = rod_like.self();

      const real_t translational_mode_damping_coeff =
          std::exp(-damping_coeff_dt);
      constexpr std::size_t dim{3UL};
      const auto n_elem(cpp17::size(rod));

      auto&& mass = ::elastica::get<elastica::tags::Mass>(rod);

      blaze::DynamicMatrix<real_t> rotational_mode_damping_coeff(dim, n_elem);

      for (std::size_t column_idx = 0; column_idx < n_elem; column_idx++) {
        real_t elemental_mass = 0.5 * (mass[column_idx] + mass[column_idx + 1]);
        if (column_idx == 0)
          elemental_mass += 0.5 * mass[column_idx];
        if (column_idx == (n_elem - 1))
          elemental_mass += 0.5 * mass[column_idx + 1];
        for (std::size_t row_idx = 0; row_idx < dim; row_idx++) {
          rotational_mode_damping_coeff(row_idx, column_idx) = std::exp(
              -damping_coeff_dt * elemental_mass *
              ::elastica::get<::elastica::tags::InvMassSecondMomentOfInertia>(
                  rod)(row_idx, column_idx));
        }
      }

      return eco::make_constraint(
          eco::make_value_constraint([](auto /*time*/, auto& /*rod_system*/,
                                        std::size_t /*index*/) {}),
          eco::make_rate_constraint([translational_mode_damping_coeff,
                                     rotational_mode_damping_coeff](
                                        auto /*time*/, auto& rod_system,
                                        std::size_t) {
            ::elastica::velocity(rod_system) *=
                translational_mode_damping_coeff;
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
    }

  }  // namespace detail

  /* Usage
  // simplified version
  simulator->constrain(block_slice_or_view)
    .at(0UL) // index is arbitrary
    .with(make_damping_constraint(block_slice_or_view, nu, dt));

  // physical version
  simulator->constrain(block_slice_or_view)
    .at(0UL) // index is arbitrary
    .with(make_damping_constraint(block_slice_or_view, nu_t, nu_r, dt));

  // Original version
  simulator->constrain(block_slice_or_view)
    .at(0UL) // index is arbitrary
    .with(make_damping_constraint(block_slice_or_view, nu*dt));
  */
  template <class ViewOrSliceGenerator, typename... Args>
  decltype(auto) make_damping_constraint(
      ::blocks::BlockGenerator<ViewOrSliceGenerator> const& generator,
      Args&&... args) {
    return detail::make_damping_constraint(blocks::generate_from(generator),
                                           std::forward<Args>(args)...);
  }

}  // namespace fish
