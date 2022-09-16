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
  struct NoThrowSystem {};

  inline std::size_t index_check(NoThrowSystem const&, std::size_t i) noexcept {
    return i + 1;
  };
  inline std::size_t index_check(NoThrowSystem const&,
                                 elastica::from_end f) noexcept {
    return 200 - f.i;
  };
  /// [nothrow_mock_system]

  struct ThrowSystem {
    std::size_t n_indices = {4};
  };

  // Assume we have b indices
  inline std::size_t index_check(ThrowSystem const& ts, std::size_t i) {
    if (i < ts.n_indices) {
      return i;
    } else {
      throw std::out_of_range("index exceeds n_indices for MockSystem!");
    }
  };

  inline std::size_t index_check(ThrowSystem const& ts, elastica::from_end fe) {
    if (fe.i < ts.n_indices) {
      return ts.n_indices - fe.i;
    } else {
      throw std::out_of_range("index is less than zero for MockSystem!");
    }
  };

  // We mock the system's check() class here
  struct MockSystem
      : public ::tt::ConformsTo<::elastica::protocols::PhysicalSystem> {
    // cannot aggregate construct since we derive
    explicit MockSystem(std::size_t n_dofs) : n_indices(n_dofs) {}
    ~MockSystem() = default;

    std::size_t n_indices;
  };

  // Assume we have b indices
  inline std::size_t index_check(MockSystem const& ms, std::size_t i) {
    if (i < ms.n_indices) {
      return i;
    } else {
      throw std::out_of_range("second_index exceeds n_indices for MockSystem!");
    }
  };

  inline std::size_t index_check(MockSystem const& ms, elastica::from_end fe) {
    if (fe.i <= ms.n_indices) {
      return ms.n_indices - fe.i;
    } else {
      throw std::out_of_range("second_index is less than zero for MockSystem!");
    }
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
