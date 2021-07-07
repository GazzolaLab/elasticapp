//
// Created by Tejaswin Parthasarathy on 3/9/21.
//

///
#include "Systems/Block/BlockVariables/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [variable_protocol_eg]
  struct ParameterTag;

  struct Scalar {
    using type = int;
  };

  struct ExampleBlockVariableSlice {
    using type = int&;
  };

  struct ExampleBlockVariableConstSlice {
    using type = int const&;
  };

  struct ExampleBlockVariable
      : public tt::ConformsTo<blocks::protocols::Variable> {
    using Parameter = ParameterTag;
    using Rank = Scalar;
    using type = typename Rank::type;
    using SliceType = ExampleBlockVariableSlice;
    using ConstSliceType = ExampleBlockVariableConstSlice;
  };

  static_assert(
      tt::assert_conforms_to<ExampleBlockVariable, blocks::protocols::Variable>,
      "Failure!");
  /// [variable_protocol_eg]

}  // namespace
