//
// Created by Tejaswin Parthasarathy on 6/21/21.
//
#include <cstddef>
#include <stdexcept>

#include "Systems/common/Access.hpp"
#include "Systems/common/Protocols.hpp"
#include "Utilities/End.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  /// [systems_protocol_eg]
  struct MySystem
      : public ::tt::ConformsTo<elastica::protocols::PhysicalSystem> {
    // for example, we have a 20 dof system
    static constexpr std::size_t n_dofs = 20UL;
    MySystem() = default;
  };

  //**Check functions*********************************************************
  /*!\name Check functions */
  //@{
  /*!\brief Checks whether indices are within range of accessible values
   */
  inline auto index_check(MySystem const& ms, std::size_t index_to_be_sliced)
      -> std::size_t {
    if (index_to_be_sliced < ms.n_dofs) {
      return index_to_be_sliced;
    } else {
      throw std::out_of_range("Index to be sliced exceeds the number of dofs!");
    }
  }
  inline auto index_check(MySystem const& ms,
                          elastica::from_end index_to_be_sliced)
      -> std::size_t {
    // cannot be 0
    const auto fe = index_to_be_sliced.i;
    if (fe <= ms.n_dofs) {
      return ms.n_dofs - fe;
    } else {
      throw std::out_of_range("Index to be sliced exceeds the number of dofs!");
    }
  };
  //**************************************************************************
  //@}
  //**************************************************************************

  // these are illustrative examples, the actual system returns a meaningful
  // position, director etc.
  inline constexpr std::size_t position(MySystem&) noexcept { return 1UL; }
  inline constexpr std::size_t position(MySystem const&) noexcept {
    return 1UL;
  }
  inline constexpr int director(MySystem&) noexcept { return -1; }
  inline constexpr int director(MySystem const&) noexcept { return -1; }
  inline constexpr std::uint8_t velocity(MySystem&) noexcept { return 2; }
  inline constexpr std::uint8_t velocity(MySystem const&) noexcept { return 2; }
  inline constexpr float angular_velocity(MySystem&) noexcept { return 1.45F; }
  inline constexpr float angular_velocity(MySystem const&) noexcept {
    return 1.45F;
  }
  inline constexpr std::uint16_t external_loads(MySystem&) noexcept {
    return 1U;
  }
  inline constexpr std::uint16_t external_loads(MySystem const&) noexcept {
    return 1U;
  }
  inline constexpr std::size_t external_torques(MySystem&) noexcept {
    return 1UL;
  }
  inline constexpr std::size_t external_torques(MySystem const&) noexcept {
    return 1UL;
  }
  /// [systems_protocol_eg]

  static_assert(::tt::assert_conforms_to<MySystem, protocols::PhysicalSystem>,
                "Not a valid system!");

}  // namespace elastica