//
// Created by Tejaswin Parthasarathy on 2/7/21.
//
#include <type_traits>

#include "Systems/Block/BlockVariables/BlockVariables.hpp"
#include "Systems/Block/BlockVariables/TypeTraits.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  template <unsigned int I>
  struct Tags {
    struct TestParameterTag {};
    struct TestExtraTag {};
    struct TestRankTag {
      using type = NoSuchType;
      using slice_type = NoSuchType;
    };
  };

  namespace {
    /// [is_variable_example]
    // a mock class that provides necessary tags
    using T0 = Tags<0U>;

    static_assert(
        blocks::IsVariable<blocks::Variable<
            T0::TestParameterTag, T0::TestRankTag, T0::TestExtraTag>>::value,
        "Not a block variable!");
    /// [is_variable_example]

    using T76 = Tags<76U>;
    static_assert(
        blocks::IsVariable<blocks::Variable<
            T76::TestParameterTag, T76::TestRankTag, T76::TestExtraTag>>::value,
        "Not a block variable!");

  }  // namespace

  namespace {
    // a mock class that provides necessary tags
    static_assert(
        blocks::IsVariable<blocks::InitializedVariable<
            T0::TestParameterTag, T0::TestRankTag, T0::TestExtraTag>>::value,
        "Not a block variable!");

    static_assert(
        blocks::IsVariable<blocks::InitializedVariable<
            T76::TestParameterTag, T76::TestRankTag, T76::TestExtraTag>>::value,
        "Not a block variable!");
  }  // namespace

  namespace {
    // a mock class that provides necessary tags
    using TestBlockVariable0 =
        blocks::Variable<T0::TestParameterTag, T0::TestRankTag,
                         T0::TestExtraTag>;
    static_assert(
        blocks::IsVariable<blocks::VariableSlice<TestBlockVariable0>>::value,
        "Not a block variable!");

    using TestBlockVariable76 =
        blocks::InitializedVariable<T76::TestParameterTag, T76::TestRankTag,
                                    T76::TestExtraTag>;
    static_assert(
        blocks::IsVariable<blocks::VariableSlice<TestBlockVariable76>>::value,
        "Not a block variable!");
  }  // namespace

  // Maps
  /// [map_setup]
  template <unsigned int I>
  using Int = std::integral_constant<int, I>;

  template <unsigned int I>
  struct TestMapType {
    using peek_type = Int<I>;
  };
  template <typename T>
  using peek_t = typename T::peek_type;
  using InitialList = tmpl::list<TestMapType<0U>, TestMapType<3U>,
                                 TestMapType<1U>, TestMapType<2U>>;
  /// [map_setup]

  /// [map_from_to_eg]
  using association = blocks::FormMap::from<peek_t>::to<InitialList>;
  static_assert(
      std::is_same<association,
                   tmpl::map<tmpl::pair<Int<0U>, TestMapType<0U>>,
                             tmpl::pair<Int<3U>, TestMapType<3U>>,
                             tmpl::pair<Int<1U>, TestMapType<1U>>,
                             tmpl::pair<Int<2U>, TestMapType<2U>>>>::value,
      "Invalid association");
  /// [map_from_to_eg]

  /// [map_to_from_eg]
  using mapping = blocks::FormMap::to<peek_t>::from<InitialList>;
  static_assert(
      std::is_same<mapping,
                   tmpl::map<tmpl::pair<TestMapType<0U>, Int<0U>>,
                             tmpl::pair<TestMapType<3U>, Int<3U>>,
                             tmpl::pair<TestMapType<1U>, Int<1U>>,
                             tmpl::pair<TestMapType<2U>, Int<2U>>>>::value,
      "Invalid association");
  /// [map_to_from_eg]

  /// [variable_mapping_eg]
  template <unsigned int I>
  struct MockVariable {
    using Parameter = std::integral_constant<unsigned int, I>;
  };
  using MockVariablesList = tmpl::list<MockVariable<1U>, MockVariable<3U>,
                                       MockVariable<5U>, MockVariable<7U>>;
  using VariableMapping = blocks::VariableMapping<MockVariablesList>;
  static_assert(std::is_same<VariableMapping::variable_from<
                                 std::integral_constant<unsigned int, 5U>>,
                             MockVariable<5U>>::value,
                "Mapping failure");
  /// [variable_mapping_eg]

}  // namespace