//
// Created by Tejaswin Parthasarathy on 1/12/21.
//

#pragma once

#include <cstddef>  // size_t
#include <stdexcept>

#include "Systems/Protocols.hpp"
#include "Utilities/End.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace mock {

  /// [nothrow_mock_system]
  struct NoThrowSystem {
    std::size_t check(std::size_t i) const noexcept { return i + 1; };
    std::size_t check(elastica::from_end f) const noexcept {
      return 200 - f.i;
    };
  };
  /// [nothrow_mock_system]

  struct ThrowSystem {
    // Assume we have b indices
    std::size_t check(std::size_t i) const {
      if (i < n_indices) {
        return i;
      } else {
        throw std::out_of_range("index exceeds n_indices for MockSystem!");
      }
    };

    std::size_t check(elastica::from_end fe) const {
      if (fe.i < n_indices) {
        return n_indices - fe.i;
      } else {
        throw std::out_of_range("index is less than zero for MockSystem!");
      }
    };

    std::size_t n_indices = {4};
  };

  // We mock the system's check() class here
  struct MockSystem
      : public ::tt::ConformsTo<::elastica::protocols::PhysicalSystem> {
    // cannot aggregate construct since we derive
    explicit MockSystem(std::size_t n_dofs) : n_indices(n_dofs) {}
    ~MockSystem() = default;

    // Assume we have b indices
    inline std::size_t check(std::size_t i) const {
      if (i < n_indices) {
        return i;
      } else {
        throw std::out_of_range(
            "second_index exceeds n_indices for MockSystem!");
      }
    };

    inline std::size_t check(elastica::from_end fe) const {
      if (fe.i <= n_indices) {
        return n_indices - fe.i;
      } else {
        throw std::out_of_range(
            "second_index is less than zero for MockSystem!");
      }
    };

    std::size_t n_indices;
  };

  // free functions in case the need arises
  inline constexpr void position(MockSystem&) noexcept {}
  inline constexpr void position(MockSystem const&) noexcept {}
  inline constexpr void director(MockSystem&) noexcept {}
  inline constexpr void director(MockSystem const&) noexcept {}
  inline constexpr void velocity(MockSystem&) noexcept {}
  inline constexpr void velocity(MockSystem const&) noexcept {}
  inline constexpr void angular_velocity(MockSystem&) noexcept {}
  inline constexpr void angular_velocity(MockSystem const&) noexcept {}
  inline constexpr std::uint16_t external_loads(MockSystem&) noexcept {
    return 1U;
  }
  inline constexpr std::uint16_t external_loads(MockSystem const&) noexcept {
    return 1U;
  }
  inline constexpr std::size_t external_torques(MockSystem&) noexcept {
    return 1UL;
  }
  inline constexpr std::size_t external_torques(MockSystem const&) noexcept {
    return 1UL;
  }

  static_assert(::tt::assert_conforms_to<MockSystem,
                                         ::elastica::protocols::PhysicalSystem>,
                "internal failure check");
}  // namespace mock
