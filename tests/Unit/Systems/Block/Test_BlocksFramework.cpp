//
// Created by Tejaswin Parthasarathy on 2/12/21.
//

#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block.hpp"
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Test beginner block first : tagged tuple semantics
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  /// [beg_tags]
  namespace tags {
    struct ExampleInitializedPosition {};
    struct ExampleInitializedVelocity {};
    struct ExampleAcceleration {};
  }  // namespace tags
  /// [beg_tags]

  /// [beg_ranks]
  template <typename T>
  struct Scalar {
    using type = T;
    using slice_type = T&;
  };
  using ScalarInt = Scalar<int>;
  using ScalarFloat = Scalar<float>;
  /// [beg_ranks]

  /// [beg_plugin]
  struct BeginnerExamplePlugin {
    // Declare the variables
    struct ExamplePosition
        : blocks::InitializedVariable<tags::ExampleInitializedPosition,
                                      ScalarInt> {};
    struct ExampleVelocity
        : blocks::InitializedVariable<tags::ExampleInitializedVelocity,
                                      ScalarFloat> {};
    struct ExampleAcceleration
        : blocks::Variable<tags::ExampleAcceleration, ScalarInt> {};

    // All variables
    using Variables =
        tmpl::list<ExamplePosition, ExampleVelocity, ExampleAcceleration>;
  };
  /// [beg_plugin]
}  // namespace

/// [beg_block]
namespace blocks {

  template <>
  class Block<BeginnerExamplePlugin>
      : public Gettable<Block<BeginnerExamplePlugin>>,
        public BeginnerExamplePlugin {
    using Parent = BeginnerExamplePlugin;
    using Parent::Variables;

    // Make it a value type
    using BlockVariables = as_block_variables<Variables>;
    BlockVariables variables_;

   public:
    //////////////////////////// start conformance /////////////////////////////
    // Conformance data members
    // Conformance for blocks::get (copy & paste this in your own block plugin)
    inline constexpr BlockVariables& data() & noexcept { return variables_; }
    inline constexpr BlockVariables const& data() const& noexcept {
      return variables_;
    }
    inline constexpr BlockVariables&& data() && noexcept {
      return static_cast<BlockVariables&&>(variables_);
    }
    inline constexpr BlockVariables const&& data() const&& noexcept {
      return static_cast<BlockVariables const&&>(variables_);
    }

    using This = Block<BeginnerExamplePlugin>;
    inline constexpr This& parent() & noexcept { return *this; }
    inline constexpr This const& parent() const& noexcept { return *this; }
    inline constexpr This&& parent() && noexcept {
      return static_cast<This&&>(*this);
    }
    inline constexpr This const&& parent() const&& noexcept {
      return static_cast<This const&&>(*this);
    }

    // Conformance type definitions
    // Conformance for blocks::get (copy & paste this in your own block plugin)
    using VariableMap = VariableMapping<Variables>;
    ////////////////////////////// end conformance /////////////////////////////

    Block() : variables_() {};
    ~Block() = default;
  };

}  // namespace blocks
/// [beg_block]

static_assert(sizeof(blocks::Block<BeginnerExamplePlugin>) ==
                  (2 * sizeof(int) + sizeof(float)),
              "Empty base optimization failed!");

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Blocks", "[Systems][Unit]") {
    using MyBlock = blocks::Block<BeginnerExamplePlugin>;
    {
      /// [beg_get]
      MyBlock block;
      blocks::get<tags::ExampleInitializedPosition>(block) = 2;
      blocks::get<tags::ExampleInitializedVelocity>(block) = 2.5F;
      blocks::get<tags::ExampleAcceleration>(block) = 0;
      /// [beg_get]

      CHECK(blocks::get<tags::ExampleInitializedPosition>(block) == 2);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block) ==
            approx(2.5F));
      CHECK(blocks::get<tags::ExampleAcceleration>(block) == 0);

      // check direct block variable access
      CHECK(blocks::get<decltype(block)::ExamplePosition>(block) == 2);
      CHECK(blocks::get<decltype(block)::ExampleVelocity>(block) ==
            approx(2.5F));
      CHECK(blocks::get<decltype(block)::ExampleAcceleration>(block) == 0);

      // const lvalue
      blocks::get<tags::ExampleInitializedPosition>(block) = 20;
      blocks::get<tags::ExampleInitializedVelocity>(block) = 1.25F;
      blocks::get<tags::ExampleAcceleration>(block) = 100;

      auto const& block_alias = static_cast<MyBlock const&>(block);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_alias) == 20);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_alias) ==
            approx(1.25F));
      CHECK(blocks::get<tags::ExampleAcceleration>(block_alias) == 100);

      // xvalue
      CHECK(blocks::get<tags::ExampleInitializedPosition>(std::move(block)) ==
            20);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(std::move(block)) ==
            approx(1.25F));
      CHECK(blocks::get<tags::ExampleAcceleration>(std::move(block)) == 100);
    }
  }
}  // namespace

namespace {

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Test intermediate block next : SoA transformation and slicing
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  /// [int_ranks]
  template <typename T>
  struct Vector {
    using type = std::vector<T>;
    using slice_type = std::vector<std::reference_wrapper<T>>;
    // in case you want to slice a const
    using const_slice_type = std::vector<std::reference_wrapper<T const>>;
  };
  using VectorInt = Vector<int>;
  using VectorFloat = Vector<float>;
  /// [int_ranks]

  /// [int_plugin]
  struct IntermediateExamplePlugin {
    // Declare the variables
    // !! New !! Everything is a vector now!
    struct ExamplePosition
        : blocks::InitializedVariable<tags::ExampleInitializedPosition,
                                      VectorInt> {};
    struct ExampleVelocity
        : blocks::InitializedVariable<tags::ExampleInitializedVelocity,
                                      VectorFloat> {};
    struct ExampleAcceleration
        : blocks::Variable<tags::ExampleAcceleration, VectorInt> {};

    // All variables
    using Variables =
        tmpl::list<ExamplePosition, ExampleVelocity, ExampleAcceleration>;

    // !! New !! Helps take slices of Variables
    using VariableSlices = blocks::as_slices<Variables>;
  };
  /// [int_plugin]

  // TODO to check non-copyable block

}  // namespace

/// [int_block]
namespace blocks {

  template <>
  class Block<IntermediateExamplePlugin>
      : public Gettable<Block<IntermediateExamplePlugin>>,   // add gettable
        public Spannable<Block<IntermediateExamplePlugin>>,  // add rest
        public IntermediateExamplePlugin {
   public:
    using Parent = IntermediateExamplePlugin;
    using Parent::Variables;
    // This type
    using This = Block<Parent>;
    //! Type of gettable
    using GetAffordance = Gettable<This>;
    //! Type of sliceable
    using SpanAfforance = Spannable<This>;
    //! Operator for slicing
    using SpanAfforance::operator[];

   private:
    // Make it a value type
    using BlockVariables = as_block_variables<Variables>;
    BlockVariables variables_;

   public:
    //////////////////////////// start conformance /////////////////////////////
    // Conformance data members
    // Conformance for blocks::get (copy & paste this in your own block plugin)
    inline constexpr BlockVariables& data() & noexcept { return variables_; }
    inline constexpr BlockVariables const& data() const& noexcept {
      return variables_;
    }
    inline constexpr BlockVariables&& data() && noexcept {
      return static_cast<BlockVariables&&>(variables_);
    }
    inline constexpr BlockVariables const&& data() const&& noexcept {
      return static_cast<BlockVariables const&&>(variables_);
    }

    inline constexpr This& parent() & noexcept { return *this; }
    inline constexpr This const& parent() const& noexcept { return *this; }
    inline constexpr This&& parent() && noexcept {
      return static_cast<This&&>(*this);
    }
    inline constexpr This const&& parent() const&& noexcept {
      return static_cast<This const&&>(*this);
    }

    // Conformance type definitions
    // Conformance for blocks::get (copy & paste this in your own block plugin)
    using VariableMap = VariableMapping<Variables>;
    ///////////////////////////// end conformance //////////////////////////////

    //////////////////////////// start conformance /////////////////////////////
    // size for the slices
    std::size_t size() const noexcept {
      // ideally all the sizes are same
      return blocks::get<Parent::ExamplePosition>(*this).size();
    }

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t index) & {
      return {blocks::get<Tag>(*this)[index]};
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return {blocks::get<Tag>(*this)[index]};
    }

    // customization point for generating a slice of a slab.
    // Since all are vectors, simple indexing should do
    template <typename Tag>
    inline constexpr auto slice(std::size_t start, std::size_t size) & {
      return typename Tag::slice_type(
          blocks::get<Tag>(*this).begin() + static_cast<long>(start),
          blocks::get<Tag>(*this).begin() + static_cast<long>(start + size));
    }
    template <typename Tag>
    inline constexpr auto slice(std::size_t start, std::size_t size) const& {
      return typename Tag::const_slice_type(
          blocks::get<Tag>(*this).cbegin() + static_cast<long>(start),
          blocks::get<Tag>(*this).cbegin() + static_cast<long>(start + size));
    }

    ///////////////////////////// end conformance //////////////////////////////

    Block() : variables_() {};
    ~Block() = default;
  };

  inline auto n_units(Block<IntermediateExamplePlugin> const& block) noexcept
      -> std::size_t {
    return block.size();
  }

  // check to see if within the range, throw error otherwise usually
  // similar to at() of vector
  inline std::size_t index_check(Block<IntermediateExamplePlugin> const&,
                                 std::size_t index) noexcept {
    return index;
  }
  inline std::size_t index_check(Block<IntermediateExamplePlugin> const& block,
                                 elastica::from_end f) noexcept {
    assert(block.size() >= f.i);
    return block.size() - f.i;
  };

}  // namespace blocks
/// [int_block]

static_assert(sizeof(blocks::Block<IntermediateExamplePlugin>) ==
                  (2 * sizeof(std::vector<int>) + sizeof(std::vector<float>)),
              "Empty base optimization failed!");

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Blocks.slice", "[Systems][Unit]") {
    using MyBlock = blocks::Block<IntermediateExamplePlugin>;
    MyBlock block;
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(2);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(3);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(4);
    auto& positions = blocks::get<tags::ExampleInitializedPosition>(block);

    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(2.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(3.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(4.5F);
    auto& velocities = blocks::get<tags::ExampleInitializedVelocity>(block);

    blocks::get<tags::ExampleAcceleration>(block).push_back(0);
    blocks::get<tags::ExampleAcceleration>(block).push_back(1);
    blocks::get<tags::ExampleAcceleration>(block).push_back(2);
    auto& accelerations = blocks::get<tags::ExampleAcceleration>(block);

    // check direct block variable access
    CHECK(blocks::get<decltype(block)::ExamplePosition>(block) == positions);
    CHECK(blocks::get<decltype(block)::ExampleVelocity>(block) == velocities);
    CHECK(blocks::get<decltype(block)::ExampleAcceleration>(block) ==
          accelerations);

    // const lvalue
    auto const& const_block_alias = static_cast<MyBlock const&>(block);
    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_alias) ==
          std::vector<int>{2, 3, 4});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(const_block_alias),
        (std::vector<float>{2.5F, 3.5F, 4.5F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{0, 1, 2});

    //    // check if the get with an index works as expected
    //    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_alias,
    //    1UL) ==
    //          3);

    auto check_slice_at_1 = [](auto const& slice, std::size_t index = 0) {
      CHECK(blocks::get<tags::ExampleInitializedPosition>(slice).at(index) ==
            3);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(slice).at(index) ==
            approx(3.5F));
      CHECK(blocks::get<tags::ExampleAcceleration>(slice).at(index) == 1);
    };

    // normal slice
    SECTION("BlockSlice") {
      auto block_slice = blocks::slice(block, 1UL);
      static_assert(blocks::IsBlockSlice<decltype(block_slice)>::value,
                    "Not a constant block slice");
      using ExpectedSliceType = blocks::BlockSlice<IntermediateExamplePlugin>;
      static_assert(
          std::is_same<ExpectedSliceType, decltype(block_slice)>::value,
          "Block slice logic failure!");

      check_slice_at_1(block_slice);

      auto block_slice_from_end = blocks::slice(block, ::elastica::end - 2UL);
      static_assert(blocks::IsBlockSlice<decltype(block_slice_from_end)>::value,
                    "Not a constant block slice");
      static_assert(std::is_same<ExpectedSliceType,
                                 decltype(block_slice_from_end)>::value,
                    "Block slice logic failure!");

      check_slice_at_1(block_slice_from_end);

      // assign to a slice
      (blocks::get<tags::ExampleAcceleration>(block_slice).at(0).get() = 3);
      // to be reflected in blocks
      CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
            std::vector<int>{0, 3, 2});
    }

    SECTION("ConstBlockSlice") {
      // const block slice
      auto const_block_slice = blocks::slice(const_block_alias, 1UL);
      static_assert(
          blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
          "Not a constant block slice");
      using ExpectedSliceType =
          blocks::ConstBlockSlice<IntermediateExamplePlugin>;
      static_assert(
          std::is_same<ExpectedSliceType, decltype(const_block_slice)>::value,
          "Block slice logic failure!");

      check_slice_at_1(const_block_slice);

      auto block_slice_from_end =
          blocks::slice(const_block_alias, ::elastica::end - 2UL);
      static_assert(
          blocks::IsConstBlockSlice<decltype(block_slice_from_end)>::value,
          "Not a constant block slice");
      static_assert(std::is_same<ExpectedSliceType,
                                 decltype(block_slice_from_end)>::value,
                    "Block slice logic failure!");

      check_slice_at_1(block_slice_from_end);
    }

    // checks the view block[0, 1]
    auto check_first_half_view = [=](auto& view) {
      CHECK(view.region().start == 0UL);
      CHECK(view.region().size == 2UL);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(view).at(0) == 2);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(view).at(0) ==
            approx(2.5F));
      CHECK(blocks::get<tags::ExampleAcceleration>(view).at(0) == 0);
      check_slice_at_1(view, 1UL);
    };

    // checks the view block[1, 2]
    auto check_second_half_view = [=](auto const& view) {
      CHECK(view.region().start == 1UL);
      CHECK(view.region().size == 2UL);
      check_slice_at_1(view, 0UL);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(view).at(1) == 4);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(view).at(1) ==
            approx(4.5F));
      CHECK(blocks::get<tags::ExampleAcceleration>(view).at(1) == 2);
    };

    auto check_subview_from_view_generator = [=](auto&& block_subview,
                                                 auto evt) {
      CHECK(block_subview.region().start == 1UL);
      CHECK(block_subview.region().size == 1UL);
      using ExpectedViewType = tmpl::type_from<decltype(evt)>;
      static_assert(
          std::is_same<ExpectedViewType,
                       std::remove_cvref_t<decltype(block_subview)>>::value,
          "Block subview logic failure!");
      // checking contents of block[1]
      check_slice_at_1(block_subview);
    };

    auto check_slice_from_view_generator = [=](auto&& block_slice, auto est) {
      using ExpectedSliceType = tmpl::type_from<decltype(est)>;
      static_assert(
          std::is_same<ExpectedSliceType,
                       std::remove_cvref_t<decltype(block_slice)>>::value,
          "Block slice logic failure!");
      check_slice_at_1(block_slice);
    };

    auto check_view_integrity = [=](auto& block_like, auto expected_view_type_,
                                    auto expected_slice_type_) {
      auto check_subview_from_view = [=](auto&& block_subview) {
        check_subview_from_view_generator(
            std::forward<decltype(block_subview)>(block_subview),
            expected_view_type_);
      };
      auto check_slice_from_view = [=](auto&& block_slice) {
        check_slice_from_view_generator(
            std::forward<decltype(block_slice)>(block_slice),
            expected_slice_type_);
      };
      auto check_subview_from_constview = [=](auto&& block_subview) {
        check_subview_from_view_generator(
            std::forward<decltype(block_subview)>(block_subview),
            tmpl::type_<blocks::ConstBlockView<IntermediateExamplePlugin>>{});
      };
      auto check_slice_from_constview = [=](auto&& block_slice) {
        check_slice_from_view_generator(
            std::forward<decltype(block_slice)>(block_slice),
            tmpl::type_<blocks::ConstBlockSlice<IntermediateExamplePlugin>>{});
      };

      auto block_view = blocks::slice(block_like, 1UL, 3UL);  // block[1, 2]
      using ExpectedViewType = tmpl::type_from<decltype(expected_view_type_)>;
      static_assert(std::is_same<ExpectedViewType, decltype(block_view)>::value,
                    "Block slice logic failure!");
      check_second_half_view(block_view);

      // block[0, 1]
      // also checks that start can be 0
      auto block_view_1 = blocks::slice(block_like, 0UL, ::elastica::end - 1UL);
      static_assert(
          std::is_same<ExpectedViewType, decltype(block_view_1)>::value,
          "Block slice logic failure!");
      check_first_half_view(block_view_1);

      // Check the ends
      // block[0, 1]
      auto block_view_2 = blocks::slice(block_like, ::elastica::end - 3UL,
                                        ::elastica::end - 1UL);
      static_assert(
          std::is_same<ExpectedViewType, decltype(block_view_2)>::value,
          "Block slice logic failure!");
      check_first_half_view(block_view_2);

      auto block_view_3 = blocks::slice(block_like, ::elastica::end - 2UL, 3UL);
      static_assert(
          std::is_same<ExpectedViewType, decltype(block_view_3)>::value,
          "Block slice logic failure!");
      check_second_half_view(block_view_3);

      // checks that stop can be end
      auto block_view_4 =
          blocks::slice(block_like, ::elastica::end - 2UL, elastica::end);
      static_assert(
          std::is_same<ExpectedViewType, decltype(block_view_4)>::value,
          "Block slice logic failure!");
      check_second_half_view(block_view_4);

      // Check slabbing a block view,
      auto subview = blocks::slice(block_view, 0UL, 1UL);
      check_subview_from_view(subview);
      check_subview_from_view(std::as_const(subview));

      auto another_subview = blocks::slice(std::as_const(block_view), 0UL, 1UL);
      check_subview_from_constview(another_subview);
      check_subview_from_constview(std::as_const(another_subview));

      check_subview_from_view(blocks::slice(block_view, 0UL, 1UL));
      check_subview_from_view(
          blocks::slice(block_view, 0UL, ::elastica::end - 1UL));
      check_subview_from_view(blocks::slice(block_view, ::elastica::end - 2UL,
                                            ::elastica::end - 1UL));
      check_subview_from_view(
          blocks::slice(block_view, ::elastica::end - 2UL, 1UL));

      // 2 is size, so check that edge case
      check_subview_from_view(blocks::slice(block_view_1, 1UL, 2UL));
      // Check end edge case
      check_subview_from_view(
          blocks::slice(block_view_1, ::elastica::end - 1UL, ::elastica::end));
      check_subview_from_view(
          blocks::slice(block_view_1, 1UL, ::elastica::end));
      check_subview_from_view(
          blocks::slice(block_view_1, ::elastica::end - 1UL, 2UL));

      // Check if full slice can be generated again
      check_second_half_view(blocks::slice(block_view, 0UL, ::elastica::end));
      check_second_half_view(blocks::slice(block_view, 0UL, 2UL));
      check_second_half_view(
          blocks::slice(block_view, ::elastica::end - 2UL, ::elastica::end));
      check_second_half_view(
          blocks::slice(block_view, ::elastica::end - 2UL, 2UL));

      // Check slicing a block view
      auto subslice = blocks::slice(block_view, 0UL);
      check_slice_from_view(subslice);
      check_slice_from_view(std::as_const(subslice));

      auto another_subslice = blocks::slice(std::as_const(block_view), 0UL);
      check_slice_from_constview(another_subslice);
      check_slice_from_constview(std::as_const(another_subslice));

      check_slice_from_view(blocks::slice(block_view, 0UL));
      check_slice_from_view(blocks::slice(block_view, ::elastica::end - 2UL));
      check_slice_from_view(blocks::slice(block_view_1, 1UL));
      check_slice_from_view(blocks::slice(block_view_1, ::elastica::end - 1UL));

      // Check that slice at the last but one index does not throw
      CHECK_NOTHROW(blocks::slice(block_view, 1UL));
      CHECK_NOTHROW(block_view[1UL]);
      CHECK_THROWS(blocks::slice(block_view, 2UL));  // end of index
      CHECK_THROWS(block_view[2UL]);
      CHECK_NOTHROW(blocks::slice(block_view, ::elastica::end - 1UL));
      CHECK_NOTHROW(block_view[::elastica::end - 1UL]);
      CHECK_THROWS(blocks::slice(block_view, ::elastica::end - 3UL));
      CHECK_THROWS(block_view[::elastica::end - 3UL]);

      CHECK_NOTHROW(blocks::slice(block_view_1, 0UL));
      CHECK_NOTHROW(block_view_1[0UL]);
      CHECK_THROWS(blocks::slice(block_view_1, 2UL));  // end of index
      CHECK_THROWS(block_view_1[2UL]);
      CHECK_NOTHROW(blocks::slice(block_view_1, ::elastica::end - 2UL));
      CHECK_NOTHROW(block_view_1[::elastica::end - 2UL]);
      CHECK_THROWS(blocks::slice(block_view_1, ::elastica::end - 3UL));
      CHECK_THROWS(block_view_1[::elastica::end - 3UL]);
    };

    SECTION("BlockView") {
      using ExpectedViewType = blocks::BlockView<IntermediateExamplePlugin>;
      using ExpectedSliceType = blocks::BlockSlice<IntermediateExamplePlugin>;

      check_view_integrity(block, tmpl::type_<ExpectedViewType>{},
                           tmpl::type_<ExpectedSliceType>{});

      auto block_view = blocks::slice(block, 1UL, 3UL);  // block[1, 2]
      // assign to a view
      (blocks::get<tags::ExampleAcceleration>(block_view).at(0).get() = 1);
      // to be reflected in blocks
      CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
            std::vector<int>{0, 1, 2});

      // View using the operator[]
      auto another_block_view = block[std::make_pair(1UL, 3UL)];  // block[1, 2]
      // assign to a view
      (blocks::get<tags::ExampleAcceleration>(another_block_view).at(0).get() =
           3);
      // to be reflected in blocks
      CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
            std::vector<int>{0, 3, 2});

      // While block view
      auto whole_block_view = blocks::slice(block);
      static_assert(
          std::is_same<ExpectedViewType, decltype(whole_block_view)>::value,
          "Block slice logic failure!");
      // assign to a view
      (blocks::get<tags::ExampleAcceleration>(whole_block_view).at(0).get() =
           4);
      (blocks::get<tags::ExampleAcceleration>(whole_block_view).at(1).get() =
           5);
      (blocks::get<tags::ExampleAcceleration>(whole_block_view).at(2).get() =
           6);

      // to be reflected in blocks
      CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
            std::vector<int>{4, 5, 6});
    }

    SECTION("ConstBlockView") {
      using ExpectedViewType =
          blocks::ConstBlockView<IntermediateExamplePlugin>;
      using ExpectedSliceType =
          blocks::ConstBlockSlice<IntermediateExamplePlugin>;

      check_view_integrity(const_block_alias, tmpl::type_<ExpectedViewType>{},
                           tmpl::type_<ExpectedSliceType>{});

      // While block view
      auto whole_block_view = blocks::slice(const_block_alias);
      static_assert(
          std::is_same<ExpectedViewType, decltype(whole_block_view)>::value,
          "Block slice logic failure!");

      // to be reflected in blocks
      CHECK(blocks::get<tags::ExampleInitializedPosition>(whole_block_view)
                .at(0) == 2);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(whole_block_view)
                .at(1) == 3);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(whole_block_view)
                .at(2) == 4);
    }

    SECTION("View iterator") {
      auto const count = [](auto& block_view) {
        auto s = 0UL;
        for (auto slice : block_view) {
          elastica::ignore_unused(slice);
          ++s;
        }
        return s;
      };
      auto const iterate = [&](auto range) {
        auto stop = range.first;
        auto expected_size = range.second;

        auto block_view = blocks::slice(block, stop - expected_size, stop);
        CHECK(count(block_view) == expected_size);

        auto const_block_view =
            blocks::slice(const_block_alias, stop - expected_size, stop);
        CHECK(count(const_block_view) == expected_size);
      };

      iterate(std::make_pair(2UL, 2UL));
      iterate(std::make_pair(2UL, 1UL));
      iterate(std::make_pair(::elastica::end, 2UL));
      iterate(std::make_pair(::elastica::end, 1UL));
    }
  }

}  // namespace

namespace {

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Test initialization next along with data transformations
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  /// [adv_plugin]
  struct AdvancedExamplePlugin {
    // Declare the variables
    struct ExamplePosition
        : blocks::InitializedVariable<tags::ExampleInitializedPosition,
                                      VectorInt> {};
    struct ExampleVelocity
        : blocks::InitializedVariable<tags::ExampleInitializedVelocity,
                                      VectorFloat> {};
    struct ExampleAcceleration
        : blocks::Variable<tags::ExampleAcceleration, VectorInt> {};

    // !! New !! Variables to be initialized are put in a separate list
    using InitializedVariables = tmpl::list<ExamplePosition, ExampleVelocity>;
    // !! New !! Other Variables (that can be computed based on initialized
    // variables) are put in a different list
    using ComputedVariables = tmpl::list<ExampleAcceleration>;
    // All variables
    using Variables = tmpl::append<InitializedVariables, ComputedVariables>;

    // Helps take slices of Variables
    using VariableSlices = blocks::as_slices<Variables>;

    // can be made generic, but we keep it simple here for the sake of clarity
    static constexpr std::size_t n_dofs = 3UL;

    // !! New !! To help initialize the block to be formed, we have an
    // initialize method
    template <typename DownstreamBlock, typename Initializer>
    void initialize(DownstreamBlock& downstream_block,
                    Initializer&& initializer) {
      // can also use the variable names in get directly
      auto& positions =
          blocks::get<tags::ExampleInitializedPosition>(downstream_block);
      auto& velocities =
          blocks::get<tags::ExampleInitializedVelocity>(downstream_block);
      auto& accelerations =
          blocks::get<tags::ExampleAcceleration>(downstream_block);

      // note! The forward is important, without which the code (rightfully)
      // won't compile. This is because we don't want to be copying initializers
      // all over the place for no reason at all
      decltype(auto) position_initializer =
          blocks::get<tags::ExampleInitializedPosition>(
              std::forward<Initializer>(initializer));
      decltype(auto) velocity_initializer =
          blocks::get<tags::ExampleInitializedVelocity>(
              std::forward<Initializer>(initializer));

      for (std::size_t i = 0UL; i < n_dofs; ++i) {
        // we know how to take the slices, so no need for calling
        // blocks::slice()
        positions[i] = position_initializer(i);
        velocities[i] = velocity_initializer(i);

        // computed variable
        accelerations[i] = positions[i] * static_cast<int>(velocities[i]);
      }
    }
  };
  /// [adv_plugin]

  // TODO to check non-copyable block

}  // namespace

/// [adv_block]
namespace blocks {

  template <>
  class Block<AdvancedExamplePlugin>
      : public Gettable<Block<AdvancedExamplePlugin>>,   // add gettable
        public Spannable<Block<AdvancedExamplePlugin>>,  // add slice support
        public AdvancedExamplePlugin {
   public:
    using Parent = AdvancedExamplePlugin;
    // Conformance for initialization
    using Parent::InitializedVariables;
    using Parent::Variables;
    // This type
    using This = Block<Parent>;
    //! Type of gettable
    using GetAffordance = Gettable<This>;
    //! Type of sliceable
    using SpanAfforance = Spannable<This>;
    //! Operator for slicing
    using SpanAfforance::operator[];

   private:
    // Make it a value type
    using BlockVariables = as_block_variables<Variables>;
    BlockVariables variables_;

   public:
    //////////////////////////// start conformance /////////////////////////////
    // Conformance data members
    // Conformance for blocks::get (copy & paste this in your own block plugin)
    inline constexpr BlockVariables& data() & noexcept { return variables_; }
    inline constexpr BlockVariables const& data() const& noexcept {
      return variables_;
    }
    inline constexpr BlockVariables&& data() && noexcept {
      return static_cast<BlockVariables&&>(variables_);
    }
    inline constexpr BlockVariables const&& data() const&& noexcept {
      return static_cast<BlockVariables const&&>(variables_);
    }

    inline constexpr This& parent() & noexcept { return *this; }
    inline constexpr This const& parent() const& noexcept { return *this; }
    inline constexpr This&& parent() && noexcept {
      return static_cast<This&&>(*this);
    }
    inline constexpr This const&& parent() const&& noexcept {
      return static_cast<This const&&>(*this);
    }

    // Conformance type definitions
    // Conformance for blocks::get (copy & paste this in your own block plugin)
    using VariableMap = VariableMapping<Variables>;
    ///////////////////////////// end conformance //////////////////////////////

    //////////////////////////// start conformance /////////////////////////////
    // size for the slices
    std::size_t size() const noexcept {
      // ideally all the sizes are same
      return blocks::get<Parent::ExamplePosition>(*this).size();
    }

    // check to see if within the range, throw error otherwise usually
    // similar to at() of vector
    inline std::size_t check(std::size_t index) const noexcept { return index; }
    inline std::size_t check(elastica::from_end f) const noexcept {
      assert(size() >= f.i);
      return size() - f.i;
    };

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t index) & {
      return {blocks::get<Tag>(*this)[index]};
    }

    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return {blocks::get<Tag>(*this)[index]};
    }

    ///////////////////////////// end conformance //////////////////////////////

    // !! New !!
    // instead of default initialization, we can fill up the block variables
    // using a set of functions, here taking the index as input and returning a
    // value as output
    template <typename Initializer>
    Block(Initializer&& initializer) : Parent(), variables_() {
      // any pre-conditions before initialization
      tmpl::for_each<Variables>([this, n_dofs = Parent::n_dofs](auto v) {
        using Var = tmpl::type_from<decltype(v)>;
        auto& var = tuples::get<Var>(variables_);
        var.resize(n_dofs);
      });

      // initialization
      Parent::initialize(*this, std::forward<Initializer>(initializer));

      // any post-conditions after initialization
    }

    ~Block() = default;
  };

  inline auto n_units(Block<AdvancedExamplePlugin> const& block) noexcept
      -> std::size_t {
    return block.size();
  }

  // check to see if within the range, throw error otherwise usually
  // similar to at() of vector
  inline std::size_t index_check(Block<AdvancedExamplePlugin> const&,
                                 std::size_t index) noexcept {
    return index;
  }
  inline std::size_t index_check(Block<AdvancedExamplePlugin> const& block,
                                 elastica::from_end f) noexcept {
    assert(block.size() >= f.i);
    return block.size() - f.i;
  };

}  // namespace blocks
/// [adv_block]

static_assert(sizeof(blocks::Block<AdvancedExamplePlugin>) ==
                  (2 * sizeof(std::vector<int>) + sizeof(std::vector<float>)),
              "Empty base optimization failed!");

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Blocks.initialize", "[Systems][Unit]") {
    /// [initialize_block_example]
    using Plugin = AdvancedExamplePlugin;
    using MyBlock = blocks::Block<Plugin>;
    MyBlock block(blocks::initialize_block<Plugin>(
        blocks::initialize<tags::ExampleInitializedPosition>(
            [](std::size_t index) -> int {
              return 1 + static_cast<int>(index);
            }),
        blocks::initialize<tags::ExampleInitializedVelocity>(
            [](std::size_t index) -> float {
              return static_cast<float>(index) + 5.5F;
            })));
    /// [initialize_block_example]

    auto& positions = blocks::get<tags::ExampleInitializedPosition>(block);
    auto& velocities = blocks::get<tags::ExampleInitializedVelocity>(block);
    auto& accelerations = blocks::get<tags::ExampleAcceleration>(block);

    CHECK(positions == std::vector<int>{1, 2, 3});
    CHECK_ITERABLE_APPROX(velocities, (std::vector<float>{5.5F, 6.5F, 7.5F}));
    CHECK(accelerations == std::vector<int>{5, 12, 21});

    // check direct block variable access
    CHECK(blocks::get<decltype(block)::ExamplePosition>(block) == positions);
    CHECK(blocks::get<decltype(block)::ExampleVelocity>(block) == velocities);
    CHECK(blocks::get<decltype(block)::ExampleAcceleration>(block) ==
          accelerations);

    // const lvalue
    auto const& const_block_alias = static_cast<MyBlock const&>(block);
    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_alias) ==
          std::vector<int>{1, 2, 3});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(const_block_alias),
        (std::vector<float>{5.5F, 6.5F, 7.5F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{5, 12, 21});

    // normal slice
    auto block_slice = blocks::slice(block, 1UL);
    static_assert(blocks::IsBlockSlice<decltype(block_slice)>::value,
                  "Not a constant block slice");
    using ExpectedSliceType = blocks::BlockSlice<AdvancedExamplePlugin>;
    static_assert(std::is_same<ExpectedSliceType, decltype(block_slice)>::value,
                  "Block slice type failure!");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(block_slice).at(0) ==
          2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_slice).at(0) ==
          approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(block_slice).at(0) == 12);

    // assign to a slice
    (blocks::get<tags::ExampleAcceleration>(block_slice).at(0).get() = 3);
    // to be reflected in blocks
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{5, 3, 21});

    // const block slice
    auto const_block_slice = blocks::slice(const_block_alias, 1UL);
    static_assert(blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
                  "Not a constant block slice");
    using ExpectedConstSliceType =
        blocks::ConstBlockSlice<AdvancedExamplePlugin>;
    static_assert(std::is_same<ExpectedConstSliceType,
                               decltype(const_block_slice)>::value,
                  "Block slice type failure!");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_slice)
              .at(0) == 2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(const_block_slice)
              .at(0) == approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_slice).at(0) == 3);

    // xvalue
    CHECK(blocks::get<tags::ExampleInitializedPosition>(std::move(block)) ==
          std::vector<int>{1, 2, 3});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(std::move(block)),
        (std::vector<float>{5.5F, 6.5F, 7.5F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(std::move(block)) ==
          std::vector<int>{5, 3, 21});
  }

}  // namespace

namespace {

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Test compositions of data with orthogonal functions acting on it
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  /// [com_plugin]
  // Any common code/variables can be refactord as a separate traits class
  struct CompositionTraits {
    // can be made generic, but we keep it simple here for the sake of clarity
    static constexpr std::size_t n_dofs = 3UL;
  };

  // Components are orthogonal entities declaring their own variables and
  // methods to act on them. These methods can also access variables from other
  // components, through specific getter functions or an API.
  // Components are typically templated on the downstream block which they
  // are going to setup

  // The first component
  template <typename BlockModel>
  struct KinematicsComponent {
    // Declare the variables
    struct ExamplePosition
        : blocks::InitializedVariable<tags::ExampleInitializedPosition,
                                      VectorInt> {};
    struct ExampleVelocity
        : blocks::InitializedVariable<tags::ExampleInitializedVelocity,
                                      VectorFloat> {};

    // Variables to be initialized are put in a separate list
    using InitializedVariables = tmpl::list<ExamplePosition, ExampleVelocity>;
    // Other Variables (that can be computed based on initialized
    // variables) are put in a different list
    using ComputedVariables = tmpl::list<>;

    // helper
    inline constexpr BlockModel& self() noexcept {
      return static_cast<BlockModel&>(*this);
    }

    // member to get the acceleration
    inline constexpr decltype(auto) get_velocity() noexcept {
      return blocks::get<ExampleVelocity>(self());
    }

    inline constexpr void update_velocities() noexcept {
      for (std::size_t i = 0UL; i < CompositionTraits::n_dofs; ++i) {
        get_velocity()[i] += static_cast<float>(self().get_acceleration()[i]);
      }
    }

    template <typename DownstreamBlock, typename Initializer>
    static void initialize(DownstreamBlock& downstream_block,
                           Initializer&& initializer) {
      /*
       * tmpl::for_each needs a copyable lambda and so this approach fails
       *
      // we can write it down manually, or let the compiler generate the code
      // for us
      struct {
        Initializer value;
      } capture{std::forward<Initializer>(initializer)};

      tmpl::for_each<InitializedVariables>(
          [n_dofs = CompositionTraits::n_dofs,
           initializer_details = std::move(capture),
           &downstream_block](auto initialized_var) mutable -> void {
            using InitializedVariable =
                tmpl::type_from<decltype(initialized_var)>;

            auto& variable = blocks::get<InitializedVariable>(downstream_block);
            decltype(auto) initializer =
                blocks::get<blocks::parameter_t<InitializedVariable>>(
                    std::move(initializer_details.value));

            for (std::size_t i = 0UL; i < n_dofs; ++i) {
              // we know how to take the slices, so no need for calling
              // blocks::slice()
              variable[i] = initializer(i);
            }
          });
      */

      // can also use the variable names in get directly
      auto& positions =
          blocks::get<tags::ExampleInitializedPosition>(downstream_block);
      auto& velocities =
          blocks::get<tags::ExampleInitializedVelocity>(downstream_block);

      // note! The forward is important, without which the code (rightfully)
      // won't compile. This is because we don't want to be copying initializers
      // all over the place for no reason at all
      decltype(auto) position_initializer =
          blocks::get<tags::ExampleInitializedPosition>(
              std::forward<Initializer>(initializer));
      decltype(auto) velocity_initializer =
          blocks::get<tags::ExampleInitializedVelocity>(
              std::forward<Initializer>(initializer));

      for (std::size_t i = 0UL; i < CompositionTraits::n_dofs; ++i) {
        // we know how to take the slices, so no need for calling
        // blocks::slice()
        positions[i] = position_initializer(i);
        velocities[i] = velocity_initializer(i);
      }
    }
  };

  // The second component
  template <typename BlockModel>
  struct DynamicsComponent {
    // Declare the variables
    struct ExampleAcceleration
        : blocks::Variable<tags::ExampleAcceleration, VectorInt> {};
    // Variables to be initialized are put in a separate list
    using InitializedVariables = tmpl::list<>;
    // Other Variables (that can be computed based on initialized
    // variables) are put in a different list
    using ComputedVariables = tmpl::list<ExampleAcceleration>;

    // member to get the acceleration
    inline constexpr decltype(auto) get_acceleration() noexcept {
      return blocks::get<ExampleAcceleration>(static_cast<BlockModel&>(*this));
    }

    template <typename DownstreamBlock, typename Initializer>
    static void initialize(DownstreamBlock& downstream_block,
                           Initializer&& /*initializer*/) {
      // can also use the variable names in get directly
      auto& positions =
          blocks::get<tags::ExampleInitializedPosition>(downstream_block);
      auto& velocities =
          blocks::get<tags::ExampleInitializedVelocity>(downstream_block);
      auto& accelerations =
          blocks::get<tags::ExampleAcceleration>(downstream_block);

      // Note : to correctly initializer accelerations we need to ensure
      // positions and velocities (i.e. dependents of accelerations) are filled
      // first. This is achieved by Traits.

      // we can specify rules for computed variables automatically
      for (std::size_t i = 0UL; i < CompositionTraits::n_dofs; ++i) {
        // we know how to take the slices, so no need for calling
        // blocks::slice()
        accelerations[i] = positions[i] * static_cast<int>(velocities[i]);
      }
    }
  };

  // !! NEW !! the plugin is now templated on the downstream block
  template <template <typename> class BlockModel>
  struct CompositionExamplePlugin
      : KinematicsComponent<BlockModel<CompositionExamplePlugin<BlockModel>>>,
        DynamicsComponent<BlockModel<CompositionExamplePlugin<BlockModel>>> {
    using Kinematics =
        KinematicsComponent<BlockModel<CompositionExamplePlugin<BlockModel>>>;
    // KinematicsComponent<BlockModel>;
    using Dynamics =
        DynamicsComponent<BlockModel<CompositionExamplePlugin<BlockModel>>>;
    // DynamicsComponent<BlockModel>;

    // Variables to be initialized are put in a separate list
    using InitializedVariables =
        tmpl::append<typename Kinematics::InitializedVariables,
                     typename Dynamics::InitializedVariables>;
    // Other Variables (that can be computed based on initialized
    // variables) are put in a different list
    using ComputedVariables =
        tmpl::append<typename Kinematics::ComputedVariables,
                     typename Dynamics::ComputedVariables>;

    // All variables
    using Variables = tmpl::append<InitializedVariables, ComputedVariables>;

    // Helps take slices of Variables
    using VariableSlices = blocks::as_slices<Variables>;

    // !! New !! To help initialize the block to be formed, we have an
    // initialize method
    template <typename DownstreamBlock, typename Initializer>
    static void initialize(DownstreamBlock& downstream_block,
                           Initializer&& initializer) {
      Kinematics::initialize(downstream_block,
                             std::forward<Initializer>(initializer));
      Dynamics::initialize(downstream_block,
                           std::forward<Initializer>(initializer));
    }
  };
  /// [com_plugin]

  // TODO to check non-copyable block

}  // namespace

/// [com_block]
namespace blocks {

  template <>
  class Block<CompositionExamplePlugin<Block>>
      : public Gettable<Block<CompositionExamplePlugin<Block>>>,   // get
        public Spannable<Block<CompositionExamplePlugin<Block>>>,  // add slice
        public CompositionExamplePlugin<Block> {
   public:
    using Parent = CompositionExamplePlugin<Block>;
    // Conformance for initialization
    using Parent::InitializedVariables;
    using Parent::Variables;

    // This type
    using This = Block<Parent>;
    //! Type of gettable
    using GetAffordance = Gettable<This>;
    //! Type of sliceable
    using SpanAfforance = Spannable<This>;
    //! Operator for slicing
    using SpanAfforance::operator[];

   private:
    // Make it a value type
    using BlockVariables = as_block_variables<Variables>;
    BlockVariables variables_;

   public:
    //////////////////////////// start conformance /////////////////////////////
    // Conformance data members
    // Conformance for blocks::get (copy & paste this in your own block
    // plugin)
    inline constexpr BlockVariables& data() & noexcept { return variables_; }
    inline constexpr BlockVariables const& data() const& noexcept {
      return variables_;
    }
    inline constexpr BlockVariables&& data() && noexcept {
      return static_cast<BlockVariables&&>(variables_);
    }
    inline constexpr BlockVariables const&& data() const&& noexcept {
      return static_cast<BlockVariables const&&>(variables_);
    }

    inline constexpr This& parent() & noexcept { return *this; }
    inline constexpr This const& parent() const& noexcept { return *this; }
    inline constexpr This&& parent() && noexcept {
      return static_cast<This&&>(*this);
    }
    inline constexpr This const&& parent() const&& noexcept {
      return static_cast<This const&&>(*this);
    }

    // Conformance type definitions
    // Conformance for blocks::get (copy & paste this in your own block
    // plugin)
    using VariableMap = VariableMapping<Variables>;
    ///////////////////////////// end conformance //////////////////////////////

    //////////////////////////// start conformance /////////////////////////////
    // size for the slices
    std::size_t size() const noexcept {
      // ideally all the sizes are same
      return blocks::get<Parent::ExamplePosition>(*this).size();
    }

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t index) & {
      return {blocks::get<Tag>(*this)[index]};
    }

    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return {blocks::get<Tag>(*this)[index]};
    }

    ///////////////////////////// end conformance //////////////////////////////

    // instead of default initialization, we can fill up the block variables
    // using a set of functions, here taking the index as input and returning
    // a value as output
    template <typename Initializer>
    explicit Block(Initializer&& initializer) : Parent(), variables_() {
      // !! New !!
      // any pre-conditions before initialization
      tmpl::for_each<Variables>(
          [this, n_dofs = CompositionTraits::n_dofs](auto v) {
            using Var = tmpl::type_from<decltype(v)>;
            tuples::get<Var>(variables_).resize(n_dofs);
          });

      // The plugin knows better what sort of data and sizes are needed, so
      // defer any initialization details to plugin
      Parent::initialize(*this, std::forward<Initializer>(initializer));

      // any post-conditions after initialization
    }

    ~Block() = default;
  };

  // Compliance with slicing
  inline auto n_units(
      Block<CompositionExamplePlugin<Block>> const& block) noexcept
      -> std::size_t {
    return block.size();
  }

}  // namespace blocks
/// [com_block]

// ensure EBCO
static_assert(sizeof(blocks::Block<CompositionExamplePlugin<blocks::Block>>) ==
                  (2 * sizeof(std::vector<int>) + sizeof(std::vector<float>)),
              "Empty base optimization failed!");

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Blocks.composition", "[Systems][Unit]") {
    using Plugin = CompositionExamplePlugin<blocks::Block>;
    using MyBlock = blocks::Block<Plugin>;
    MyBlock block(blocks::initialize_block<Plugin>(
        blocks::initialize<tags::ExampleInitializedPosition>(
            [](std::size_t index) -> int {
              return 1 + static_cast<int>(index);
            }),
        blocks::initialize<tags::ExampleInitializedVelocity>(
            [](std::size_t index) -> float {
              return static_cast<float>(index) + 5.5F;
            })));

    auto& positions = blocks::get<tags::ExampleInitializedPosition>(block);
    auto& velocities = blocks::get<tags::ExampleInitializedVelocity>(block);
    auto& accelerations = blocks::get<tags::ExampleAcceleration>(block);

    CHECK(positions == std::vector<int>{1, 2, 3});
    CHECK_ITERABLE_APPROX(velocities, (std::vector<float>{5.5F, 6.5F, 7.5F}));
    CHECK(accelerations == std::vector<int>{5, 12, 21});

    // check direct block variable access
    CHECK(blocks::get<decltype(block)::ExamplePosition>(block) == positions);
    CHECK(blocks::get<decltype(block)::ExampleVelocity>(block) == velocities);
    CHECK(blocks::get<decltype(block)::ExampleAcceleration>(block) ==
          accelerations);

    // const lvalue
    auto const& const_block_alias = static_cast<MyBlock const&>(block);
    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_alias) ==
          std::vector<int>{1, 2, 3});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(const_block_alias),
        (std::vector<float>{5.5F, 6.5F, 7.5F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{5, 12, 21});

    // normal slice
    auto block_slice = blocks::slice(block, 1UL);
    using ExpectedSliceType =
        blocks::BlockSlice<CompositionExamplePlugin<blocks::BlockSlice>>;
    static_assert(std::is_same<ExpectedSliceType, decltype(block_slice)>::value,
                  "Block slice type failure!");

    static_assert(blocks::IsBlockSlice<decltype(block_slice)>::value,
                  "Not a constant block slice");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(block_slice).at(0) ==
          2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_slice).at(0) ==
          approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(block_slice).at(0) == 12);

    // assign to a slice
    (blocks::get<tags::ExampleAcceleration>(block_slice).at(0).get() = 3);
    // to be reflected in blocks
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{5, 3, 21});

    // const block slice
    auto const_block_slice = blocks::slice(const_block_alias, 1UL);
    static_assert(blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
                  "Not a constant block slice");
    using ExpectedConstSliceType = blocks::ConstBlockSlice<
        CompositionExamplePlugin<blocks::ConstBlockSlice>>;
    static_assert(std::is_same<ExpectedConstSliceType,
                               decltype(const_block_slice)>::value,
                  "Block slice type failure!");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_slice)
              .at(0) == 2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(const_block_slice)
              .at(0) == approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_slice).at(0) == 3);

    // check the member functions
    CHECK(velocities == block.get_velocity());
    CHECK(accelerations == block.get_acceleration());
    // update velocities with accelerations
    block.update_velocities();
    // check new velocities
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(const_block_alias),
        (std::vector<float>{5.5F + 5.F, 6.5F + 3.F, 7.5F + 21.F}));

    // xvalue
    CHECK(blocks::get<tags::ExampleInitializedPosition>(std::move(block)) ==
          std::vector<int>{1, 2, 3});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(std::move(block)),
        (std::vector<float>{5.5F + 5.F, 6.5F + 3.F, 7.5F + 21.F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(std::move(block)) ==
          std::vector<int>{5, 3, 21});
  }

}  // namespace

/*
namespace blocks {

  template <typename Block, typename Plugin>
  struct BlockFacade : public Plugin, elastica::CRTPHelper<Block, BlockFacade> {
   public:
    using Parent = Plugin;
    // Conformance for initialization
    // using typename Parent::InitializedVariables;
    using typename Parent::Variables;

    using CRTP = elastica::CRTPHelper<Block, BlockFacade>;
    using CRTP::self;

   private:
    // Make it a value type
    using BlockVariables = as_block_variables<Variables>;
    BlockVariables variables_;

   public:
    //////////////////////////// start conformance /////////////////////////////
    // Conformance data members
    // Conformance for blocks::get (copy & paste this in your own block
    // plugin)
    inline constexpr BlockVariables& data() & noexcept { return variables_; }
    inline constexpr BlockVariables const& data() const& noexcept {
      return variables_;
    }
    inline constexpr BlockVariables&& data() && noexcept {
      return static_cast<BlockVariables&&>(variables_);
    }
    inline constexpr BlockVariables const&& data() const&& noexcept {
      return static_cast<BlockVariables const&&>(variables_);
    }

    // Conformance type definitions
    // Conformance for blocks::get (copy & paste this in your own block
    // plugin)
    using VariableMap = blocks::VariableMapping<Variables>;
    ///////////////////////////// end conformance //////////////////////////////

    BlockFacade() = default;
    ~BlockFacade() = default;

    // !! New !! To help initialize the block to be formed, we have an
    // initialize method
    // using Parent::initialize;

    template <typename IndexLike>
    inline decltype(auto) operator[](IndexLike index) {
      return slice(self(), index);
    }
  };
}  // namespace blocks
*/

struct FacadedIntermediateExamplePlugin
    : public ::tt::ConformsTo<blocks::protocols::Plugin> {
  // Declare the variables
  // !! New !! Everything is a vector now!
  struct ExamplePosition
      : blocks::InitializedVariable<tags::ExampleInitializedPosition,
                                    VectorInt> {};
  struct ExampleVelocity
      : blocks::InitializedVariable<tags::ExampleInitializedVelocity,
                                    VectorFloat> {};
  struct ExampleAcceleration
      : blocks::Variable<tags::ExampleAcceleration, VectorInt> {};

  // All variables
  using Variables =
      tmpl::list<ExamplePosition, ExampleVelocity, ExampleAcceleration>;

  // !! New !! Helps take slices of Variables
  using VariableSlices = blocks::as_slices<Variables>;

  int do_it_or_else() { return 1; }
};

/// [fac_int_block]
namespace blocks {

  template <>
  class Block<FacadedIntermediateExamplePlugin>
      : public BlockFacade<FacadedIntermediateExamplePlugin> {
   public:
    using Parent = BlockFacade<FacadedIntermediateExamplePlugin>;

    //////////////////////////// start conformance /////////////////////////////
    // size for the slices
    std::size_t size() const noexcept {
      // ideally all the sizes are same
      return blocks::get<Parent::ExamplePosition>(*this).size();
    }

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t index) & {
      return {blocks::get<Tag>(*this)[index]};
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return {blocks::get<Tag>(*this)[index]};
    }

    // customization point for generating a slice of a slab.
    // Since all are vectors, simple indexing should do
    template <typename Tag>
    inline constexpr auto slice(std::size_t start, std::size_t size) & {
      return typename Tag::slice_type(
          blocks::get<Tag>(*this).begin() + static_cast<long>(start),
          blocks::get<Tag>(*this).begin() + static_cast<long>(start + size));
    }
    template <typename Tag>
    inline constexpr auto slice(std::size_t start, std::size_t size) const& {
      return typename Tag::const_slice_type(
          blocks::get<Tag>(*this).cbegin() + static_cast<long>(start),
          blocks::get<Tag>(*this).cbegin() + static_cast<long>(start + size));
    }

    ///////////////////////////// end conformance //////////////////////////////

    Block() = default;
    ~Block() = default;
  };

  // for apply_over_slice adapter
  inline auto n_units(
      Block<FacadedIntermediateExamplePlugin> const& block) noexcept
      -> std::size_t {
    return block.size();
  }
  // check to see if within the range, throw error otherwise usually
  // similar to at() of vector
  inline std::size_t index_check(Block<FacadedIntermediateExamplePlugin> const&,
                                 std::size_t index) noexcept {
    return index;
  }
  inline std::size_t index_check(
      Block<FacadedIntermediateExamplePlugin> const& block,
      elastica::from_end f) noexcept {
    assert(block.size() >= f.i);
    return block.size() - f.i;
  };

}  // namespace blocks
/// [fac_int_block]

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Blocks.facade", "[Systems][Unit]") {
    using MyBlock = blocks::Block<FacadedIntermediateExamplePlugin>;
    MyBlock block;
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(2);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(3);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(4);
    auto& positions = blocks::get<tags::ExampleInitializedPosition>(block);

    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(2.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(3.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(4.5F);
    auto& velocities = blocks::get<tags::ExampleInitializedVelocity>(block);

    blocks::get<tags::ExampleAcceleration>(block).push_back(0);
    blocks::get<tags::ExampleAcceleration>(block).push_back(1);
    blocks::get<tags::ExampleAcceleration>(block).push_back(2);
    auto& accelerations = blocks::get<tags::ExampleAcceleration>(block);

    // check direct block variable access
    CHECK(blocks::get<decltype(block)::ExamplePosition>(block) == positions);
    CHECK(blocks::get<decltype(block)::ExampleVelocity>(block) == velocities);
    CHECK(blocks::get<decltype(block)::ExampleAcceleration>(block) ==
          accelerations);

    // const lvalue
    auto const& const_block_alias = static_cast<MyBlock const&>(block);
    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_alias) ==
          std::vector<int>{2, 3, 4});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(const_block_alias),
        (std::vector<float>{2.5F, 3.5F, 4.5F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{0, 1, 2});

    // normal slice
    auto block_slice = blocks::slice(block, 1UL);
    static_assert(blocks::IsBlockSlice<decltype(block_slice)>::value,
                  "Not a constant block slice");
    using ExpectedSliceType =
        blocks::BlockSlice<FacadedIntermediateExamplePlugin>;
    static_assert(std::is_same<ExpectedSliceType, decltype(block_slice)>::value,
                  "Block slice type failure!");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(block_slice).at(0) ==
          3);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_slice).at(0) ==
          approx(3.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(block_slice).at(0) == 1);

    // assign to a slice
    (blocks::get<tags::ExampleAcceleration>(block_slice).at(0).get() = 3);
    // to be reflected in blocks
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{0, 3, 2});

    // const block slice
    auto const_block_slice = blocks::slice(const_block_alias, 1UL);
    static_assert(blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
                  "Not a constant block slice");
    using ExpectedConstSliceType =
        blocks::ConstBlockSlice<FacadedIntermediateExamplePlugin>;
    static_assert(std::is_same<ExpectedConstSliceType,
                               decltype(const_block_slice)>::value,
                  "Block slice type failure!");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_slice)
              .at(0) == 3);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(const_block_slice)
              .at(0) == approx(3.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_slice).at(0) == 3);

    // subscript the main block
    auto new_block_slice = block[elastica::end - 1UL];

    CHECK(
        blocks::get<tags::ExampleInitializedPosition>(new_block_slice).at(0) ==
        4);
    CHECK(
        blocks::get<tags::ExampleInitializedVelocity>(new_block_slice).at(0) ==
        approx(4.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(new_block_slice).at(0) == 2);

    // check if member functions are imported
    CHECK(new_block_slice.do_it_or_else() == 1);

    // xvalue
    CHECK(blocks::get<tags::ExampleInitializedPosition>(std::move(block)) ==
          std::vector<int>{2, 3, 4});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(std::move(block)),
        (std::vector<float>{2.5F, 3.5F, 4.5F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(std::move(block)) ==
          std::vector<int>{0, 3, 2});
  }

  ELASTICA_TEST_CASE("Unit.Systems.Blocks.VariableCache", "[Systems][Unit]") {
    using MyBlock = blocks::Block<FacadedIntermediateExamplePlugin>;
    MyBlock block;
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(2);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(3);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(4);

    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(2.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(3.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(4.5F);

    blocks::get<tags::ExampleAcceleration>(block).push_back(0);
    blocks::get<tags::ExampleAcceleration>(block).push_back(1);
    blocks::get<tags::ExampleAcceleration>(block).push_back(2);

    {
      auto block_slice = blocks::slice(block, 1UL);

      auto block_cache = blocks::cache(block_slice);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_cache).at(0) ==
            3);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_cache).at(0) ==
            approx(3.5F));
      CHECK(blocks::get<tags::ExampleAcceleration>(block_cache).at(0) == 1);

      using SubsetList = tmpl::list<tags::ExampleInitializedPosition,
                                    tags::ExampleInitializedVelocity>;
      auto block_subset_cache = blocks::cache<SubsetList>(block_slice);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
                .at(0) == 3);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
                .at(0) == approx(3.5F));

      // Check assignment etc.
      blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
          .at(0)
          .get() = 2;
      blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
          .at(0)
          .get() = 2.5F;

      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
                .at(0) == 2);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
                .at(0) == approx(2.5F));

      blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
          .at(0)
          .get() = 3;
      blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
          .at(0)
          .get() = 3.5F;
    }

    {
      auto block_slice = blocks::slice(block, 1UL, 3UL);

      auto block_cache = blocks::cache(block_slice);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_cache).at(0) ==
            3);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_cache).at(0) ==
            approx(3.5F));
      CHECK(blocks::get<tags::ExampleAcceleration>(block_cache).at(0) == 1);

      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_cache).at(1) ==
            4);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_cache).at(1) ==
            approx(4.5F));
      CHECK(blocks::get<tags::ExampleAcceleration>(block_cache).at(1) == 2);

      using SubsetList = tmpl::list<tags::ExampleInitializedPosition,
                                    tags::ExampleInitializedVelocity>;
      auto block_subset_cache = blocks::cache<SubsetList>(block_slice);
      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
                .at(0) == 3);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
                .at(0) == approx(3.5F));

      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
                .at(1) == 4);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
                .at(1) == approx(4.5F));

      // Check assignment etc.
      blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
          .at(0)
          .get() = 2;
      blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
          .at(0)
          .get() = 2.5F;
      blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
          .at(1)
          .get() = 12;
      blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
          .at(1)
          .get() = 12.5F;

      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
                .at(0) == 2);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
                .at(0) == approx(2.5F));
      CHECK(blocks::get<tags::ExampleInitializedPosition>(block_subset_cache)
                .at(1) == 12);
      CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_subset_cache)
                .at(1) == approx(12.5F));
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.Blocks.metadata", "[Systems][Unit]") {
    using MyBlock = blocks::Block<FacadedIntermediateExamplePlugin>;
    MyBlock block;

    blocks::get<tags::ExampleInitializedPosition>(block).push_back(2);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(3);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(4);

    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(2.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(3.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(4.5F);

    blocks::get<tags::ExampleAcceleration>(block).push_back(0);
    blocks::get<tags::ExampleAcceleration>(block).push_back(1);
    blocks::get<tags::ExampleAcceleration>(block).push_back(2);

    auto const check_integrity = [](std::string const& str) {
      using namespace std::string_literals;
      for (auto const& substr :
           {"ExampleInitializedPosition"s, "ExampleInitializedVelocity"s,
            "ExampleAcceleration"s}) {
        INFO(substr);
        CHECK(str.find(substr) != std::string::npos);
      }
    };

    auto const run_test = [=](auto const& b) {
      std::stringstream ss;
      ss << blocks::metadata(b);
      check_integrity(ss.str());
    };

    run_test(block);
    run_test(blocks::slice(block, 0UL));
    run_test(blocks::slice(block, 0UL, 2UL));
    run_test(blocks::slice(std::as_const(block), 0UL));
    run_test(blocks::slice(std::as_const(block), 0UL, 2UL));
  }

}  // namespace

namespace {
  ELASTICA_TEST_CASE("Unit.Systems.Blocks.apply_over_slices_adapter",
                     "[Systems][Unit]") {
    using MyBlock = blocks::Block<FacadedIntermediateExamplePlugin>;
    MyBlock block;
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(2);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(3);
    blocks::get<tags::ExampleInitializedPosition>(block).push_back(4);

    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(2.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(3.5F);
    blocks::get<tags::ExampleInitializedVelocity>(block).push_back(4.5F);

    blocks::get<tags::ExampleAcceleration>(block).push_back(0);
    blocks::get<tags::ExampleAcceleration>(block).push_back(1);
    blocks::get<tags::ExampleAcceleration>(block).push_back(2);

    std::size_t n_slices = 0UL;
    auto count_slices = blocks::for_each_slice_adapter(
        [&n_slices](auto const&) { ++n_slices; });
    (void)count_slices(block);
    CHECK(n_slices == 3UL);

    auto old_n_slices = n_slices;
    auto view = blocks::slice(block, 0UL, 2UL);
    (void)count_slices(view);  // cannot be used with a rvalue ref.
    CHECK(n_slices == old_n_slices + 2UL);

    int accum_position = 0;
    auto accumulate_position_over =
        blocks::for_each_slice_adapter([&accum_position](auto slice) {
          accum_position +=
              blocks::get<tags::ExampleInitializedPosition>(slice).at(0);
        });
    (void)accumulate_position_over(block);
    CHECK(accum_position == (2 + 3 + 4));

    auto old_accum_position = accum_position;
    (void)accumulate_position_over(view);
    CHECK(accum_position == old_accum_position + (2 + 3));
  }
}  // namespace