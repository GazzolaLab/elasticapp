#pragma once

#include <cstddef>

#include "ModuleSettings/HashGrids.hpp"
#include "Simulator/Domain/Policies/Directions.hpp"
#include "Simulator/Domain/Types.hpp"
#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {

          // Specialization for periodic domain
          class DomainPolicyImplBase {
           protected:
            using DirectionType =
                typename ::elastica::detail::Directions::DirectionType;

            template <DirectionType Dir>
            static inline constexpr auto initial_cell_count() noexcept
                -> std::size_t;

            template <>
            inline constexpr auto
            initial_cell_count<::elastica::detail::Directions::X>() noexcept
                -> std::size_t {
              using namespace elastica::collision::detection::coarse::hhg;
              return initial_x_cell_count();
            };

            template <>
            inline constexpr auto
            initial_cell_count<::elastica::detail::Directions::Y>() noexcept
                -> std::size_t {
              using namespace elastica::collision::detection::coarse::hhg;
              return initial_y_cell_count();
            };

            template <>
            inline constexpr auto
            initial_cell_count<::elastica::detail::Directions::Z>() noexcept
                -> std::size_t {
              using namespace elastica::collision::detection::coarse::hhg;
              return initial_z_cell_count();
            };
          }

        }  // namespace detail

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
