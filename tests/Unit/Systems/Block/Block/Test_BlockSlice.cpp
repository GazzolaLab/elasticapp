//
// Created by Tejaswin Parthasarathy on 2/26/21.
//
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block/BlockSlice.hpp"
#include "Systems/Block/Block/Types.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace {

  template <int I>
  using Int = std::integral_constant<int, I>;

  template <int I>
  struct DummyVariable {
    using Parameter = Int<I>;
    using SliceType = Int<I>;
    using ConstSliceType = const Int<I>;
  };

  using IntegerList = tmpl::integral_list<int, 1, 4, 9, 16>;
  template <typename T>
  struct ToDummyVariable {
    using type = DummyVariable<T::value>;
  };

  using DummyVariables =
      tmpl::transform<IntegerList, ToDummyVariable<tmpl::_1>>;

  template <typename T>
  struct ToInt {
    using type = Int<T::value>;
  };
  using CorrectSlices = tmpl::transform<IntegerList, ToInt<tmpl::_1>>;

  template <typename T>
  struct ToConstInt {
    using type = const Int<T::value>;
  };
  using CorrectConstSlices = tmpl::transform<IntegerList, ToConstInt<tmpl::_1>>;

  using SliceInfo = blocks::as_slices<DummyVariables>;

  static_assert(std::is_same<typename SliceInfo::slices, CorrectSlices>::value,
                "as_slices Failure!");
  static_assert(
      std::is_same<typename SliceInfo::const_slices, CorrectConstSlices>::value,
      "as_slices Failure!");

}  // namespace

namespace {

  struct TestPlugin {
    using Variables = DummyVariables;
    int check_member() & noexcept { return 2; }
    int check_member() const& noexcept { return 3; }
  };

  ELASTICA_TEST_CASE("Unit.Systems.BlockSlice", "[Systems][Unit]") {
    using MyBlockSlice = blocks::BlockSlice<TestPlugin>;
    static_assert(
        std::is_same<
            MyBlockSlice::BlockVariableSlices,
            tuples::TaggedTuple<Int<1>, Int<4>, Int<9>, Int<16>>>::value,
        "Block slice failure");

    // initializes Int<1>, Int<4>, Int<9>, Int<16> all to be integers with the
    // same values as the types
    MyBlockSlice b{};
    CHECK(tuples::get<Int<1>>(b.data()).value == 1);
    CHECK(tuples::get<Int<4>>(b.data()).value == 4);
    CHECK(tuples::get<Int<9>>(b.data()).value == 9);
    CHECK(tuples::get<Int<16>>(b.data()).value == 16);

    CHECK(b.check_member() == 2);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockSlice", "[Systems][Unit]") {
    using MyBlockSlice = blocks::ConstBlockSlice<TestPlugin>;
    static_assert(
        std::is_same<MyBlockSlice::BlockVariableSlices,
                     tuples::TaggedTuple<const Int<1>, const Int<4>,
                                         const Int<9>, const Int<16>>>::value,
        "Block slice failure");

    // initializes Int<1>, Int<4>, Int<9>, Int<16> all to be integers with the
    // same values as the types
    MyBlockSlice b{};
    CHECK(tuples::get<const Int<1>>(b.data()).value == 1);
    CHECK(tuples::get<const Int<4>>(b.data()).value == 4);
    CHECK(tuples::get<const Int<9>>(b.data()).value == 9);
    CHECK(tuples::get<const Int<16>>(b.data()).value == 16);

    CHECK(b.check_member() == 2);
  }

}  // namespace