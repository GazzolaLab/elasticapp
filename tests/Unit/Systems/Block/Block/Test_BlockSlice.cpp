//
// Created by Tejaswin Parthasarathy on 2/26/21.
//
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block/BlockSlice.hpp"
#include "Systems/Block/Block/Types.hpp"
#include "Utilities/MakeArray.hpp"
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

  using SliceInfo = blocks::detail::as_slices<DummyVariables>;

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

}  // namespace

namespace {
  template <typename DV>
  struct AnotherDummyVariableSlice {
    using type = int&;
  };

  template <typename DV>
  struct AnotherDummyVariableConstSlice {
    using type = int const&;
  };

  template <typename T>
  struct AnotherDummyVariable {
    using type = int;
    using SliceType = AnotherDummyVariableSlice<AnotherDummyVariable<T>>;
    using Parameter = T;
    using ConstSliceType =
        const AnotherDummyVariableConstSlice<AnotherDummyVariable<T>>;
  };

  struct FirstParam;
  struct SecondParam;
  struct ThirdParam;

  struct TestPluginWithData {
    using Variables = tmpl::list<AnotherDummyVariable<FirstParam>,
                                 AnotherDummyVariable<SecondParam>,
                                 AnotherDummyVariable<ThirdParam>>;
  };

  template <typename T>
  struct NonCopyableDummyVariable {
    using type = std::unique_ptr<int>;
    using SliceType = AnotherDummyVariableSlice<NonCopyableDummyVariable<T>>;
    using Parameter = T;
    using ConstSliceType =
        const AnotherDummyVariableConstSlice<NonCopyableDummyVariable<T>>;
  };

  struct TestPluginWithNonCopyableData {
    using Variables = tmpl::list<AnotherDummyVariable<FirstParam>,
                                 NonCopyableDummyVariable<SecondParam>,
                                 NonCopyableDummyVariable<ThirdParam>>;
  };

}  // namespace

namespace {

  // declare testing members
  template <typename Plugin>
  class TestBlockSliceFacade : public blocks::detail::BlockSliceFacade<Plugin> {
   public:
    using Parent = blocks::detail::BlockSliceFacade<Plugin>;
    template <typename... Args>
    explicit TestBlockSliceFacade(Args&&... args)
        : Parent(std::forward<Args>(args)...) {}
    TestBlockSliceFacade(TestBlockSliceFacade& other)
        : Parent(static_cast<Parent&>(other)){};
    TestBlockSliceFacade(TestBlockSliceFacade const& other)
        : Parent(static_cast<Parent const&>(other)){};
    TestBlockSliceFacade(TestBlockSliceFacade&& other)
        : Parent(static_cast<Parent&&>(other)){};
  };

  template <typename Plugin>
  class TestConstBlockSliceFacade
      : public blocks::detail::ConstBlockSliceFacade<Plugin> {
   public:
    using Parent = blocks::detail::ConstBlockSliceFacade<Plugin>;
    template <typename... Args>
    explicit TestConstBlockSliceFacade(Args&&... args)
        : Parent(std::forward<Args>(args)...) {}
    TestConstBlockSliceFacade(TestConstBlockSliceFacade& other)
        : Parent(static_cast<Parent&>(other)){};
    TestConstBlockSliceFacade(TestConstBlockSliceFacade const& other)
        : Parent(static_cast<Parent const&>(other)){};
    TestConstBlockSliceFacade(TestConstBlockSliceFacade&& other)
        : Parent(static_cast<Parent&&>(other)){};
  };

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.BlockSliceFacade.member_construction",
                     "[Systems][Unit]") {
    using MyBlockSlice = TestBlockSliceFacade<TestPlugin>;
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

  ELASTICA_TEST_CASE("Unit.Systems.BlockSliceFacade.equality",
                     "[Systems][Unit]") {
    {
      using MyBlockSlice = TestBlockSliceFacade<TestPlugin>;
      MyBlockSlice b{};
      MyBlockSlice c{};
      static_assert(b == c,
                    "Static equality check for block slices incorrect!");
    }

    {
      using MyBlockSlice = TestBlockSliceFacade<TestPluginWithData>;
      auto first_array(make_array(1, 2, 3));
      auto second_array(make_array(4, 5, 6));

      MyBlockSlice b{first_array[0U], first_array[1U], first_array[2U]};
      MyBlockSlice c{second_array[0U], second_array[1U], second_array[2U]};
      CHECK(not(b == c));

      MyBlockSlice d{first_array[0U], first_array[1U], first_array[2U]};
      CHECK((b == d));
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockSliceFacade.copy_construction",
                     "[Systems][Unit]") {
    {
      using MyBlockSlice = TestBlockSliceFacade<TestPlugin>;
      MyBlockSlice b{};  // does work from before
      MyBlockSlice c(b);
      CHECK(b == c);
    }

    {
      using MyBlockSlice = TestBlockSliceFacade<TestPluginWithData>;
      auto first_array(make_array(1, 2, 3));
      auto second_array(make_array(4, 5, 6));

      MyBlockSlice b{first_array[0U], first_array[1U], first_array[2U]};
      MyBlockSlice c{second_array[0U], second_array[1U], second_array[2U]};
      CHECK(not(b == c));

      MyBlockSlice d(b);
      CHECK((b == d));
      CHECK(not(d == c));
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockSliceFacade.move_construction",
                     "[Systems][Unit]") {
    {
      using MyBlockSlice = TestBlockSliceFacade<TestPlugin>;
      MyBlockSlice b{};  // does work from before
      MyBlockSlice c(b);
      MyBlockSlice d(std::move(b));
      CHECK(d == c);
    }

    {
      using MyBlockSlice = TestBlockSliceFacade<TestPluginWithData>;
      auto first_array(make_array(1, 2, 3));

      MyBlockSlice b{first_array[0U], first_array[1U], first_array[2U]};
      MyBlockSlice c{first_array[0U], first_array[1U], first_array[2U]};
      MyBlockSlice d(std::move(b));
      CHECK(d == c);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockSliceFacade.construction",
                     "[Systems][Unit]") {
    using MyBlockSlice = TestConstBlockSliceFacade<TestPlugin>;
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

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockSliceFacade.equality",
                     "[Systems][Unit]") {
    {  // static equality
      using MyBlockSlice = TestConstBlockSliceFacade<TestPlugin>;
      MyBlockSlice b{};
      MyBlockSlice c{};
      static_assert(b == c,
                    "Static equality check for block slices incorrect!");
    }

    {
      using MyBlockSlice = TestConstBlockSliceFacade<TestPluginWithData>;
      auto first_array(make_array(1, 2, 3));
      auto second_array(make_array(4, 5, 6));

      MyBlockSlice b{first_array[0U], first_array[1U], first_array[2U]};
      MyBlockSlice c{second_array[0U], second_array[1U], second_array[2U]};
      CHECK(not(b == c));

      MyBlockSlice d{first_array[0U], first_array[1U], first_array[2U]};
      CHECK((b == d));
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockSliceFacade.copy_construction",
                     "[Systems][Unit]") {
    {
      using MyConstBlockSlice = TestConstBlockSliceFacade<TestPlugin>;
      MyConstBlockSlice b{};  // does work from before
      MyConstBlockSlice c(b);
      CHECK(b == c);
    }

    {
      using MyConstBlockSlice = TestConstBlockSliceFacade<TestPluginWithData>;
      auto first_array(make_array(1, 2, 3));
      auto second_array(make_array(4, 5, 6));

      MyConstBlockSlice b{first_array[0U], first_array[1U], first_array[2U]};
      MyConstBlockSlice c{second_array[0U], second_array[1U], second_array[2U]};
      CHECK(not(b == c));

      MyConstBlockSlice d(b);
      CHECK((b == d));
      CHECK(not(d == c));
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockSliceFacade.move_construction",
                     "[Systems][Unit]") {
    {
      using MyConstBlockSlice = TestConstBlockSliceFacade<TestPlugin>;
      MyConstBlockSlice b{};  // does work from before
      MyConstBlockSlice c(b);
      MyConstBlockSlice d(std::move(b));
      CHECK(d == c);
    }

    {
      using MyConstBlockSlice = TestConstBlockSliceFacade<TestPluginWithData>;
      auto first_array(make_array(1, 2, 3));

      MyConstBlockSlice b{first_array[0U], first_array[1U], first_array[2U]};
      MyConstBlockSlice c{first_array[0U], first_array[1U], first_array[2U]};
      MyConstBlockSlice d(std::move(b));
      CHECK(d == c);
    }
  }

}  // namespace