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

  /// [block_variable_derived_eg]
  struct ExampleBlockVariableDerived
      : public blocks::Variable<ExampleParameter, ExampleRank, ExampleExtra> {};

  static_assert(
      std::is_same<ExampleBlockVariableDerived::type, std::vector<int>>::value,
      "Block variable inheritance failure!");
  static_assert(ExampleBlockVariableDerived::give_me_5() == 5,
                "Block variable inheritance failure!");
  /// [block_variable_derived_eg]
  static_assert(tt::assert_conforms_to<ExampleBlockVariableDerived,
                                       blocks::protocols::Variable>,
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

  // TODO : does this need to have variable semantics?
  //  static_assert(tt::assert_conforms_to<ExampleBlockVariableSlice,
  //                                       blocks::protocols::Variable>,
  //                "Failure!");
  //  static_assert(tt::assert_conforms_to<ExampleInitializedBlockVariableSlice,
  //                                       blocks::protocols::Variable>,
  //                "Failure!");

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

  // TODO : does this need to have variable semantics?
  //  static_assert(tt::assert_conforms_to<ExampleBlockConstVariableSlice,
  //                                       blocks::protocols::Variable>,
  //                "Failure!");
  //  static_assert(
  //      tt::assert_conforms_to<ExampleInitializedBlockConstVariableSlice,
  //                             blocks::protocols::Variable>,
  //      "Failure!");

  /// [block_variable_same_as_eg]
  using SameAsExampleBlockVariable =
      blocks::same_as<ExampleBlockVariable>::with<struct SomeTag>;

  static_assert(
      std::is_same<SameAsExampleBlockVariable::type, std::vector<int>>::value,
      "Block variable inheritance failure!");
  static_assert(SameAsExampleBlockVariable::give_me_5() == 5,
                "Block variable inheritance failure!");

  /// [block_variable_same_as_eg]

  static_assert(std::is_same<SameAsExampleBlockVariable::This,
                             blocks::Variable<struct SomeTag, ExampleRank,
                                              ExampleExtra>>::value,
                "Block variable inheritance failure!");
  static_assert(tt::assert_conforms_to<SameAsExampleBlockVariable,
                                       blocks::protocols::Variable>,
                "Failure!");

  /// [bv_same_as_derived_eg]
  struct SameAsExampleBlockVariableButDerived
      : public blocks::same_as<ExampleBlockVariable>::with<struct SomeTag> {};

  static_assert(std::is_same<SameAsExampleBlockVariableButDerived::type,
                             std::vector<int>>::value,
                "Block variable inheritance failure!");
  static_assert(SameAsExampleBlockVariableButDerived::give_me_5() == 5,
                "Block variable inheritance failure!");
  /// [bv_same_as_derived_eg]

  static_assert(std::is_same<SameAsExampleBlockVariableButDerived::This,
                             blocks::Variable<struct SomeTag, ExampleRank,
                                              ExampleExtra>>::value,
                "Block variable inheritance failure!");
  static_assert(tt::assert_conforms_to<SameAsExampleBlockVariableButDerived,
                                       blocks::protocols::Variable>,
                "Failure!");

  /// [bv_same_as_from_derived_eg]
  struct SameAsExampleBlockVariableDerived
      : public blocks::same_as<ExampleBlockVariableDerived>::with<
            struct SomeTag> {};

  static_assert(std::is_same<SameAsExampleBlockVariableDerived::type,
                             std::vector<int>>::value,
                "Block variable inheritance failure!");
  static_assert(SameAsExampleBlockVariableDerived::give_me_5() == 5,
                "Block variable inheritance failure!");
  /// [bv_same_as_from_derived_eg]

  static_assert(std::is_same<SameAsExampleBlockVariableDerived::This,
                             blocks::Variable<struct SomeTag, ExampleRank,
                                              ExampleExtra>>::value,
                "Block variable inheritance failure!");
  static_assert(tt::assert_conforms_to<SameAsExampleBlockVariableDerived,
                                       blocks::protocols::Variable>,
                "Failure!");

}  // namespace
