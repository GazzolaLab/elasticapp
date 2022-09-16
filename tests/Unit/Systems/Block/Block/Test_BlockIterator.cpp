//
// Created by Tejaswin Parthasarathy on 7/10/21.
//
#include <type_traits>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block.hpp"
#include "Systems/Block/Block/BlockIterator.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/End.hpp"
#include "Utilities/TMPL.hpp"

////////////////////////////////////////////////////////////////////////////////
// Copied from block facade
////////////////////////////////////////////////////////////////////////////////
namespace {

  template <int I>
  using Int = std::integral_constant<int, I>;

  struct Vector {
    using type = std::vector<int>;
    using slice_type = int&;
    // in case you want to slice a const
    using const_slice_type = int const&;
  };

  // helper to put it inside blocks
  template <template <typename> class B, template <typename> class Plugin>
  struct PluginHelper : public Plugin<B<PluginHelper<B, Plugin>>> {};

}  // namespace

namespace {
  template <typename B>
  struct FacadePluginWithoutInitialization
      : public ::elastica::CRTPHelper<B, FacadePluginWithoutInitialization> {
    // Declare the variables
    // !! New !! Everything is a vector now!
    struct ExamplePosition : blocks::InitializedVariable<Int<1>, Vector> {};
    struct ExampleVelocity : blocks::InitializedVariable<Int<2>, Vector> {};
    struct ExampleAcceleration : blocks::InitializedVariable<Int<3>, Vector> {};

    using ::elastica::CRTPHelper<B, FacadePluginWithoutInitialization>::self;

    // All variables
    using Variables =
        tmpl::list<ExamplePosition, ExampleVelocity, ExampleAcceleration>;

    decltype(auto) get_position() { return blocks::get<Int<1>>(self()); }
    decltype(auto) get_velocity() { return blocks::get<Int<2>>(self()); }
    decltype(auto) get_acceleration() { return blocks::get<Int<3>>(self()); }

    decltype(auto) get_position() const { return blocks::get<Int<1>>(self()); }
    decltype(auto) get_velocity() const { return blocks::get<Int<2>>(self()); }
    decltype(auto) get_acceleration() const {
      return blocks::get<Int<3>>(self());
    }
  };

  template <template <typename> class B>
  using BoundFacadePluginWithoutInitialization =
      PluginHelper<B, FacadePluginWithoutInitialization>;

}  // namespace

namespace blocks {

  template <>
  class Block<BoundFacadePluginWithoutInitialization<Block>>
      : public BlockFacade<BoundFacadePluginWithoutInitialization<Block>> {
   public:
    using Parent = BlockFacade<BoundFacadePluginWithoutInitialization<Block>>;

    //////////////////////////// start conformance /////////////////////////////

    // In this case, we do not check the business logic in this class, but only
    // if the interfaces are OK.

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t index) & {
      return tuples::get<Tag>(this->data())[index];
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return tuples::get<Tag>(this->data())[index];
    }
    ///////////////////////////// end conformance //////////////////////////////

    Block(std::size_t n)
        : n_(n){
              //      // reserve
              //      tuples::apply(
              //          [this](auto&... v) {
              //            EXPAND_PACK_LEFT_TO_RIGHT(const_cast<std::vector<int>>(v).reserve(n_));
              //          },
              //          data());
          };
    ~Block() = default;
    Block(Block&& other) : Parent(std::move(other)), n_(other.n_) {}
    std::size_t n_;
  };

  // for iteration adapter
  inline auto n_units(
      Block<BoundFacadePluginWithoutInitialization<Block>> const&
          block) noexcept -> std::size_t {
    return block.n_;
  }

  // conformance to slice
  inline std::size_t index_check(
      Block<BoundFacadePluginWithoutInitialization<Block>> const&,
      std::size_t index) noexcept {
    return index;
  }
  inline std::size_t index_check(
      Block<BoundFacadePluginWithoutInitialization<Block>> const& block,
      elastica::from_end index) noexcept {
    return n_units(block) - index.i;
  };

}  // namespace blocks

namespace {

  using TestPlugin = BoundFacadePluginWithoutInitialization<blocks::Block>;
  using TestBlock = blocks::Block<TestPlugin>;
  TestBlock fetch_test_block() {
    const std::size_t n_elems = 5UL;
    TestBlock block(n_elems);
    block.get_position().reserve(n_elems);
    for (int i : {1, 2, 3, 4, 5}) {
      block.get_position().push_back(i);
      block.get_velocity().push_back(2 * i);
      block.get_acceleration().push_back(3 * i);
    }
    return block;
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.default_constructor",
                     "[Systems][Unit]") {
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it{};

    CHECK(it == TestIterator());
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.constructor",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    {
      TestIterator it(&test_block, 0UL);
      CHECK(it.data() == &test_block);
      CHECK(it.index() == 0UL);
    }

    {
      TestIterator it(&test_block, 0UL);

      CHECK((*it).get_position() == 1);
      CHECK((*it).get_velocity() == 2);
      CHECK((*it).get_acceleration() == 3);
    }

    {
      TestIterator it(&test_block, 2UL);

      CHECK((*it).get_position() == 3);
      CHECK((*it).get_velocity() == 6);
      CHECK((*it).get_acceleration() == 9);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.copy_constructor",
                     "[Systems][Unit]") {
    using TestIterator = blocks::BlockIterator<TestPlugin>;
    auto test_block = fetch_test_block();
    auto it([&test_block]() {
      TestIterator it(&test_block, 0UL);
      return it;
    }());

    CHECK((*it).get_position() == 1);
    CHECK((*it).get_velocity() == 2);
    CHECK((*it).get_acceleration() == 3);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.modify_access",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    {
      TestIterator it(&test_block, 1UL);

      CHECK((*it).get_position() == 2);
      CHECK((*it).get_velocity() == 4);
      CHECK((*it).get_acceleration() == 6);

      (*it).get_position() = 3;
      (*it).get_velocity() = 2;
      (*it).get_acceleration() = 1;

      CHECK((*it).get_position() == 3);
      CHECK((*it).get_velocity() == 2);
      CHECK((*it).get_acceleration() == 1);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.add_assign",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 0UL);
    it += 2UL;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.sub_assign",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    it -= 1UL;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.copy_assign",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    auto another_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 2UL);

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);

    TestIterator another_it(&another_block, 3UL);
    CHECK((*another_it).get_position() == 4);
    CHECK((*another_it).get_velocity() == 8);
    CHECK((*another_it).get_acceleration() == 12);

    another_it = it;
    CHECK((*another_it).get_position() == 3);
    CHECK((*another_it).get_velocity() == 6);
    CHECK((*another_it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.pre_increment",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 1UL);
    ++it;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.post_increment",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 1UL);
    it++;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.pre_decrement",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    --it;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.post_decrement",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    it--;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.logical_comparisons",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    TestIterator another(&test_block, 3UL);
    TestIterator yet_another(&test_block, 4UL);

    CHECK(it == another);
    CHECK(it != yet_another);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockIterator.iteration",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::BlockIterator<TestPlugin>;
    {
      auto it(begin(test_block));

      CHECK((*it).get_position() == 1);
      CHECK((*it).get_velocity() == 2);
      CHECK((*it).get_acceleration() == 3);
    }

    {
      auto b(begin(test_block));
      auto e(end(test_block));
      CHECK(b != e);
    }

    {
      auto b(begin(test_block));
      TestIterator it(&test_block, 3UL);
      auto e(end(test_block));

      CHECK(it != b);
      CHECK(it != e);
    }

    // this is the prize we are after
    {
      int position_counter = 0UL;
      for (auto slice : test_block) {
        position_counter += slice.get_position();
        // check we can modify
        slice.get_position() = 1;
      }
      CHECK(position_counter == 15);
      CHECK(test_block.get_position() == std::vector<int>(5UL, 1));
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.default_constructor",
                     "[Systems][Unit]") {
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it{};

    CHECK(it == TestIterator());
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.constructor",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    {
      TestIterator it(&test_block, 0UL);
      CHECK(it.data() == &test_block);
      CHECK(it.index() == 0UL);
    }

    {
      TestIterator it(&test_block, 0UL);

      CHECK((*it).get_position() == 1);
      CHECK((*it).get_velocity() == 2);
      CHECK((*it).get_acceleration() == 3);
    }

    {
      TestIterator it(&test_block, 2UL);

      CHECK((*it).get_position() == 3);
      CHECK((*it).get_velocity() == 6);
      CHECK((*it).get_acceleration() == 9);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.copy_constructor",
                     "[Systems][Unit]") {
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;
    auto const test_block = fetch_test_block();
    auto it([&test_block]() {
      TestIterator it(&test_block, 0UL);
      return it;
    }());

    CHECK((*it).get_position() == 1);
    CHECK((*it).get_velocity() == 2);
    CHECK((*it).get_acceleration() == 3);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.read_only_access",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    {
      TestIterator it(&test_block, 1UL);

      CHECK((*it).get_position() == 2);
      CHECK((*it).get_velocity() == 4);
      CHECK((*it).get_acceleration() == 6);

      // confirmed to not work
      // (*it).get_position() = 3;
      // (*it).get_velocity() = 2;
      // (*it).get_acceleration() = 1;
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.add_assign",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 0UL);
    it += 2UL;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.sub_assign",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    it -= 1UL;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.copy_assign",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    auto another_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 2UL);

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);

    TestIterator another_it(&another_block, 3UL);
    CHECK((*another_it).get_position() == 4);
    CHECK((*another_it).get_velocity() == 8);
    CHECK((*another_it).get_acceleration() == 12);

    another_it = it;
    CHECK((*another_it).get_position() == 3);
    CHECK((*another_it).get_velocity() == 6);
    CHECK((*another_it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.pre_increment",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 1UL);
    ++it;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.post_increment",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 1UL);
    it++;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.pre_decrement",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    --it;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.post_decrement",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    it--;

    CHECK((*it).get_position() == 3);
    CHECK((*it).get_velocity() == 6);
    CHECK((*it).get_acceleration() == 9);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.logical_comparisons",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(&test_block, 3UL);
    TestIterator another(&test_block, 3UL);
    TestIterator yet_another(&test_block, 4UL);

    CHECK(it == another);
    CHECK(it != yet_another);
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.iteration",
                     "[Systems][Unit]") {
    auto const test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;
    {
      auto it(begin(test_block));

      CHECK((*it).get_position() == 1);
      CHECK((*it).get_velocity() == 2);
      CHECK((*it).get_acceleration() == 3);
    }

    {
      auto b(begin(test_block));
      auto e(end(test_block));
      CHECK(b != e);
    }

    {
      TestIterator it(&test_block, 3UL);
      auto b(begin(test_block));
      auto e(end(test_block));

      CHECK(it != b);
      CHECK(it != e);
    }

    // this is the prize we are after
    {
      int position_counter = 0UL;
      for (auto const_slice : test_block) {
        position_counter += const_slice.get_position();
      }
      CHECK(position_counter == 15);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.ConstBlockIterator.conversion",
                     "[Systems][Unit]") {
    auto test_block = fetch_test_block();
    using TestIterator = blocks::ConstBlockIterator<TestPlugin>;

    TestIterator it(begin(test_block));

    CHECK((*it).get_position() == 1);
    CHECK((*it).get_velocity() == 2);
    CHECK((*it).get_acceleration() == 3);

    (*begin(test_block)).get_position() = 0;
    (*begin(test_block)).get_velocity() = 1;
    (*begin(test_block)).get_acceleration() = 2;

    CHECK((*it).get_position() == 0);
    CHECK((*it).get_velocity() == 1);
    CHECK((*it).get_acceleration() == 2);
  }

}  // namespace
