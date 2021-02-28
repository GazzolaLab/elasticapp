//
// Created by Tejaswin Parthasarathy on 2/9/21.
//
#include <vector>

#include "Systems/Block/BlockVariables/BlockVariables.hpp"
///
#include "Systems/Block/BlockVariables/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [block_variable_setup]
  struct ExampleParameter;
  struct ExampleRank {
    using type = std::vector<int>;
    using slice_type = int&;
    using const_slice_type = int const&;
  };
  struct ExampleExtra {
    // customize the behavior of Variable by providing a static member
    static constexpr int give_me_5() noexcept { return 5; }
  };
  /// [block_variable_setup]

  /// [block_variable_eg]
  using ExampleBlockVariable =
      blocks::Variable<ExampleParameter, ExampleRank, ExampleExtra>;

  static_assert(
      std::is_same<ExampleBlockVariable::type, std::vector<int>>::value,
      "Block variable inheritance failure!");
  static_assert(ExampleBlockVariable::give_me_5() == 5,
                "Block variable inheritance failure!");
  /// [block_variable_eg]
  static_assert(
      tt::assert_conforms_to<ExampleBlockVariable, blocks::protocols::Variable>,
      "Failure!");

  /// [init_block_variable_eg]
  using ExampleInitializedBlockVariable =
      blocks::InitializedVariable<ExampleParameter, ExampleRank, ExampleExtra>;

  static_assert(std::is_same<ExampleInitializedBlockVariable::type,
                             std::vector<int>>::value,
                "Block variable inheritance failure!");
  static_assert(ExampleInitializedBlockVariable::give_me_5() == 5,
                "Block variable inheritance failure!");
  /// [init_block_variable_eg]
  static_assert(tt::assert_conforms_to<ExampleInitializedBlockVariable,
                                       blocks::protocols::Variable>,
                "Failure!");

  /// [variable_slice_eg]
  using ExampleBlockVariable =
      blocks::Variable<ExampleParameter, ExampleRank, ExampleExtra>;
  using ExampleInitializedBlockVariable =
      blocks::InitializedVariable<ExampleParameter, ExampleRank, ExampleExtra>;

  using ExampleBlockVariableSlice = blocks::VariableSlice<ExampleBlockVariable>;
  using ExampleInitializedBlockVariableSlice =
      blocks::VariableSlice<ExampleInitializedBlockVariable>;

  static_assert(std::is_same<ExampleBlockVariableSlice::type, int&>::value,
                "Block variable failure!");
  static_assert(
      std::is_same<ExampleInitializedBlockVariableSlice::type, int&>::value,
      "Block variable failure!");
  /// [variable_slice_eg]

  static_assert(tt::assert_conforms_to<ExampleBlockVariableSlice,
                                       blocks::protocols::Variable>,
                "Failure!");
  static_assert(tt::assert_conforms_to<ExampleInitializedBlockVariableSlice,
                                       blocks::protocols::Variable>,
                "Failure!");

  /// [const_variable_slice_eg]
  using ExampleBlockVariable =
      blocks::Variable<ExampleParameter, ExampleRank, ExampleExtra>;
  using ExampleInitializedBlockVariable =
      blocks::InitializedVariable<ExampleParameter, ExampleRank, ExampleExtra>;

  using ExampleBlockConstVariableSlice =
      blocks::ConstVariableSlice<ExampleBlockVariable>;
  using ExampleInitializedBlockConstVariableSlice =
      blocks::ConstVariableSlice<ExampleInitializedBlockVariable>;

  static_assert(
      std::is_same<ExampleBlockConstVariableSlice::type, int const&>::value,
      "Block variable failure!");
  static_assert(std::is_same<ExampleInitializedBlockConstVariableSlice::type,
                             int const&>::value,
                "Block variable failure!");
  /// [const_variable_slice_eg]

  static_assert(tt::assert_conforms_to<ExampleBlockConstVariableSlice,
                                       blocks::protocols::Variable>,
                "Failure!");
  static_assert(
      tt::assert_conforms_to<ExampleInitializedBlockConstVariableSlice,
                             blocks::protocols::Variable>,
      "Failure!");

}  // namespace
