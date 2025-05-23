#pragma once

#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Interface/Systems/BlockRefs.hpp>
//
#include <elastica/Systems/CosseratRods/CosseratRodPlugin.hpp>
#include <elastica/Systems/CosseratRods/Utility/CenterOfMass.hpp>
#include <elastica/Systems/Tags.hpp>
//
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math.hpp>
//
#include <cmath>

namespace fish {
  namespace constraint {
    namespace details {
      template <typename Traits, template <typename> class ComputationalBlock,
                template <typename /*CRT*/,
                          typename /*InitializedBlock*/> class... Components>
      decltype(auto) make_carling_fish_constraint(
          ::elastica::cosserat_rod::CosseratRodPlugin<
              Traits, ComputationalBlock, Components...> const& rod_like,
          ::elastica::real_t const period, ::elastica::real_t const wave_number,
          ::elastica::real_t const phase_shift,
          ::elastica::real_t const ramp_up_time) {
        namespace eco = ::elastica::constraints;
        using ::elastica::real_t;
        using ::elastica::Rot3;
        using ::elastica::Vec3;

        auto const& rod = rod_like.self();

        auto const n_elems{cpp17::size(rod)};
        real_t const length{::blaze::sum(
            ::elastica::get<::elastica::tags::ReferenceElementLength>(rod))};
        real_t const total_mass{
            ::blaze::sum(::elastica::get<::elastica::tags::Mass>(rod))};
        real_t const angular_frequency{2 * M_PI / period};
        Vec3 const start_position{::elastica::position(rod, 0UL)};

        std::size_t const x_idx = 0UL;
        std::size_t const y_idx = 1UL;

        return eco::make_constraint(
            eco::make_value_constraint([=](auto time, auto& rod_system,
                                           std::size_t /*index*/) {
              auto&& position{::elastica::position(rod_system)};
              auto&& director{::elastica::director(rod_system)};
              auto&& mass{::blocks::get<::elastica::tags::Mass>(rod_system)};
              Vec3 com = position * mass / total_mass;

              real_t ramp_factor =
                  time <= ramp_up_time
                      ? 0.5 * (1 + std::sin(M_PI * (time / ramp_up_time - 0.5)))
                      : 1.0;

              // reset position for update
              position *= 0.0;

              real_t const ds0 = 1.0 / static_cast<real_t>(n_elems);
              real_t const ds2 = std::pow(length * ds0, 2);
              real_t s = 0.0;

              for (std::size_t i = 0UL; i < n_elems + 1UL; ++i) {
                // Gazzola et al. JCP, 2011 Eqn 45
                position(y_idx, i) =
                    0.125 * ramp_factor * length * (0.03125 + s) / 1.03125 *
                    std::sin(wave_number * s - angular_frequency * time +
                             phase_shift);
                // (delta x)^2 + (delta y)^2 = (ds)^2
                if (i != 0) {
                  position(x_idx, i) =
                      position(x_idx, i - 1) +
                      std::sqrt(ds2 - std::pow(position(y_idx, i) -
                                                   position(y_idx, i - 1),
                                               2));
                }

                s += ds0;
              }

              // Adjust position
              for (std::size_t i = 0UL; i < n_elems + 1UL; ++i) {
                ::blaze::column(position, i) += start_position;
              }
              real_t y_com = ::blaze::dot(mass, ::blaze::row(position, y_idx)) /
                             total_mass;
              ::blaze::row(position, y_idx) += com[y_idx] - y_com;

              // Update directors
              auto&& position_diff(
                  Traits::Operations::difference_kernel(position));
              for (std::size_t i = 0UL; i < n_elems; ++i) {
                ::blaze::columnslice(director, i) =
                    ::elastica::make_orthogonal_bases_from_normal_and_tangent(
                        Vec3{0, 0, 1},  // shouldn't hardcode this
                        ::blaze::normalize(::blaze::column(position_diff, i)));
              }
            }),
            eco::make_rate_constraint(
                [](auto /*time*/, auto& rod_system, std::size_t /*index*/) {
                  ::elastica::velocity(rod_system) *= 0.0;
                  ::elastica::angular_velocity(rod_system) *= 0.0;
                }))();
      }
    }  // namespace details

    /* Make periodic fish motion via constraint. Modeled after Carling et al.,
     * 1998. */
    template <class ViewOrSliceGenerator, typename... Args>
    decltype(auto) make_carling_fish_constraint(
        ::blocks::BlockGenerator<ViewOrSliceGenerator> const& generator,
        Args&&... args) {
      return details::make_carling_fish_constraint(
          blocks::generate_from(generator), std::forward<Args>(args)...);
    }
  }  // namespace constraint
}  // namespace fish