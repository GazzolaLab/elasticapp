//
// Created by Tejaswin Parthasarathy on 5/11/21.
//

#include "Systems/CosseratRods/Traits/PlacementTraits/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [placement_protocol_eg]
  struct ExamplePlacementTrait
      : public ::tt::ConformsTo<
            elastica::cosserat_rod::protocols::PlacementTrait> {
    inline static auto get_dofs(std::size_t n_elems) -> std::size_t {
      return n_elems;
    }
    inline static auto n_ghosts() -> std::size_t {
      return 100UL; //!100 ghosts
    }
  };

  static_assert(
      tt::assert_conforms_to<ExamplePlacementTrait,
                             elastica::cosserat_rod::protocols::PlacementTrait>,
      "Protocol Failure!");
  /// [placement_protocol_eg]

}  // namespace