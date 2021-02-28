//
// Created by Tejaswin Parthasarathy on 2/26/21.
//
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block/BlockFacade.hpp"
#include "Systems/Block/Block/Types.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/End.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

/// [block_facade_usage]
struct CustomPlugin {
  using Variables = tmpl::list<>;
};

namespace blocks {

  template <>
  class Block<CustomPlugin> : public BlockFacade<CustomPlugin> {};

}  // namespace blocks
/// [block_facade_usage]

namespace blocks {

  // specialize for testing
  template <typename Plugin>
  class TestBlockFacade : public BlockFacade<Plugin> {
   public:
    using Parent = BlockFacade<Plugin>;
    TestBlockFacade() = default;
    using Parent::initialize;
    using Parent::parent_need_not_be_initialized;
    using Parent::parent_to_be_initialized;
  };

}  // namespace blocks

// variables setup
namespace {

  template <int I>
  using Int = std::integral_constant<int, I>;

  template <int I>
  struct DummyVariable {
    static constexpr int value = I;
    using Parameter = Int<I>;
    using type = int;
    using SliceType = int&;
    using ConstSliceType = const int&;
  };

  // helper to put it inside blocks
  template <template <typename> class B, template <typename> class Plugin>
  struct PluginHelper : public Plugin<B<PluginHelper<B, Plugin>>> {};

}  // namespace

namespace {
  /// [no_init_plugin]
  template <typename B>
  struct FacadePluginWithoutInitialization
      : public ::elastica::CRTPHelper<B, FacadePluginWithoutInitialization> {
    // Declare the variables
    // !! New !! Everything is a vector now!
    struct ExamplePosition : DummyVariable<1> {};
    struct ExampleVelocity : DummyVariable<2> {};
    struct ExampleAcceleration : DummyVariable<3> {};

    using ::elastica::CRTPHelper<B, FacadePluginWithoutInitialization>::self;

    // All variables
    using Variables =
        tmpl::list<ExamplePosition, ExampleVelocity, ExampleAcceleration>;

    int& get_position() { return tuples::get<ExamplePosition>(self().data()); }
    int& get_velocity() { return tuples::get<ExampleVelocity>(self().data()); }
    int& get_acceleration() {
      return tuples::get<ExampleAcceleration>(self().data());
    }

    int const& get_position() const {
      return tuples::get<ExamplePosition>(self().data());
    }
    int const& get_velocity() const {
      return tuples::get<ExampleVelocity>(self().data());
    }
    int const& get_acceleration() const {
      return tuples::get<ExampleAcceleration>(self().data());
    }
  };
  /// [no_init_plugin]

  template <template <typename> class B>
  using BoundFacadePluginWithoutInitialization =
      PluginHelper<B, FacadePluginWithoutInitialization>;

}  // namespace

namespace blocks {

  template <>
  class Block<BoundFacadePluginWithoutInitialization<Block>>
      : public TestBlockFacade<BoundFacadePluginWithoutInitialization<Block>> {
   public:
    using Parent =
        TestBlockFacade<BoundFacadePluginWithoutInitialization<Block>>;

    //////////////////////////// start conformance /////////////////////////////

    // In this case, we do not check the business logic in this class, but only
    // if the interfaces are OK.
    inline std::size_t check(std::size_t /* index */) const noexcept {
      return 0UL;
    }
    inline std::size_t check(elastica::from_end /* index */) const noexcept {
      return 0UL;
    };

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t /* index */) & {
      return tuples::get<Tag>(this->data());
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t /* index */) const& {
      return tuples::get<Tag>(this->data());
    }
    ///////////////////////////// end conformance //////////////////////////////

    Block() = default;
    ~Block() = default;
  };

}  // namespace blocks

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.BlockFacade.no_initialize",
                     "[Systems][Unit]") {
    using Plugin = BoundFacadePluginWithoutInitialization<blocks::Block>;
    using MyBlock = blocks::Block<Plugin>;
    MyBlock block;

    static_assert(MyBlock::parent_need_not_be_initialized(),
                  "meta failure in BlockFacade");

    block.get_position() = 1;
    block.get_velocity() = 2;
    block.get_acceleration() = 5;

    auto const& as_const_block = static_cast<MyBlock const&>(block);
    CHECK(as_const_block.get_acceleration() == 5);
    CHECK(as_const_block.get_velocity() == 2);
    CHECK(as_const_block.get_position() == 1);
  }

}  // namespace

namespace {

  /// [init_plugin]
  template <typename B>
  struct FacadePluginWithInitialization
      : public ::elastica::CRTPHelper<B, FacadePluginWithInitialization> {
    // Declare the variables
    // !! New !! Everything is a vector now!
    struct ExamplePosition : DummyVariable<1> {};
    struct ExampleVelocity : DummyVariable<2> {};
    struct ExampleAcceleration : DummyVariable<3> {};

    // !! New !! Variables to be initialized are put in a separate list
    using InitializedVariables = tmpl::list<ExamplePosition, ExampleVelocity>;
    // !! New !! Other Variables (that can be computed based on initialized
    // variables) are put in a different list
    using ComputedVariables = tmpl::list<ExampleAcceleration>;
    // All variables
    using Variables = tmpl::append<InitializedVariables, ComputedVariables>;

    // !! New !! To help initialize the block to be formed, we have an
    // initialize method
    template <typename DownstreamBlock, typename Initializer>
    static void initialize(DownstreamBlock& downstream_block, Initializer&&) {
      // initialize all ints properly
      // do not use blocks::get here, that is tested elsewhere
      // do not test initialization routines here

      {
        using Tag = ExamplePosition;
        tuples::get<Tag>(downstream_block.data()) = Tag::value;
      }
      {
        using Tag = ExampleVelocity;
        tuples::get<Tag>(downstream_block.data()) = Tag::value;
      }
      // say it is computed from the other two tags
      tuples::get<ExampleAcceleration>(downstream_block.data()) = 5;
    }

    using ::elastica::CRTPHelper<B, FacadePluginWithInitialization>::self;

    int& get_position() { return tuples::get<ExamplePosition>(self().data()); }
    int& get_velocity() { return tuples::get<ExampleVelocity>(self().data()); }
    int& get_acceleration() {
      return tuples::get<ExampleAcceleration>(self().data());
    }

    int const& get_position() const {
      return tuples::get<ExamplePosition>(self().data());
    }
    int const& get_velocity() const {
      return tuples::get<ExampleVelocity>(self().data());
    }
    int const& get_acceleration() const {
      return tuples::get<ExampleAcceleration>(self().data());
    }

    int do_it_or_else() { return 3; }
  };
  /// [init_plugin]

  template <template <typename> class B>
  using BoundFacadePluginWithInitialization =
      PluginHelper<B, FacadePluginWithInitialization>;

}  // namespace

namespace blocks {
  template <>
  class Block<BoundFacadePluginWithInitialization<Block>>
      : public TestBlockFacade<BoundFacadePluginWithInitialization<Block>> {
   public:
    using Parent = TestBlockFacade<BoundFacadePluginWithInitialization<Block>>;

    //////////////////////////// start conformance /////////////////////////////

    // In this case, we do not check the business logic in this class, but only
    // if the interfaces are OK.
    inline std::size_t check(std::size_t /* index */) const noexcept {
      return 0UL;
    }
    inline std::size_t check(elastica::from_end /* index */) const noexcept {
      return 0UL;
    };

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t /* index */) & {
      return tuples::get<Tag>(this->data());
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t /* index */) const& {
      return tuples::get<Tag>(this->data());
    }
    ///////////////////////////// end conformance //////////////////////////////

    Block() : Parent() {
      // we are not using initializer so give it a no value
      Parent::initialize(*this, NoSuchType{});
    };
    ~Block() = default;
  };

}  // namespace blocks

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.BlockFacade.initialize", "[Systems][Unit]") {
    using Plugin = BoundFacadePluginWithInitialization<blocks::Block>;
    using MyBlock = blocks::Block<Plugin>;
    MyBlock block;

    static_assert(MyBlock::parent_to_be_initialized(),
                  "meta failure in BlockFacade");

    CHECK(block.get_acceleration() == 5);
    CHECK(block.get_velocity() == 2);
    CHECK(block.get_position() == 1);

    auto const& as_const_block = static_cast<MyBlock const&>(block);
    CHECK(as_const_block.get_acceleration() == 5);
    CHECK(as_const_block.get_velocity() == 2);
    CHECK(as_const_block.get_position() == 1);
  }

}  // namespace