//
// Created by Tejaswin Parthasarathy on 4/30/22.
//

#include "Simulator/Environments/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [environment_tag_protocol_eg]
  template <typename T>
  struct MyEnvironment;

  struct MyEnvironmentTag
      : public ::tt::ConformsTo<
            elastica::environments::protocols::EnvironmentTag> {
    using requires_primitives = std::false_type;

    template <typename Traits>
    using type = MyEnvironment<Traits>;
  };

  /// [environment_tag_protocol_eg]

  static_assert(
      ::tt::assert_conforms_to<
          MyEnvironmentTag, elastica::environments::protocols::EnvironmentTag>,
      "Not a valid system!");

}  // namespace