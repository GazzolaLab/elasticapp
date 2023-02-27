//
// Created by Tejaswin Parthasarathy on 5/11/21.
//

#include "Systems/common/Traits/DataType/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Systems/CosseratRods/Mocks/CosseratRodTraits.hpp"

namespace {


  /// [datatrait_protocol_eg]
  static_assert(
      tt::assert_conforms_to<mock::VectorType<double>,
                             elastica::cosserat_rod::protocols::DataTrait>,
      "Protocol Failure!");
  /// [datatrait_protocol_eg]

}  // namespace