//
// Created by Tejaswin Parthasarathy on 2/12/21.
//

#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block.hpp"
#include "Utilities/End.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Test beginner block first : tagged tuple semantics
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  /// [beg_tags]
  namespace tags {
    struct ExampleInitializedPosition;
    struct ExampleInitializedVelocity;
    struct ExampleAcceleration;
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
  class Block<BeginnerExamplePlugin> : public BeginnerExamplePlugin {
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

    // Conformance type definitions
    // Conformance for blocks::get (copy & paste this in your own block plugin)
    using VariableMap = VariableMapping<Variables>;
    ////////////////////////////// end conformance /////////////////////////////

    Block() : variables_(){};
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
    using slice_type = T&;
    // in case you want to slice a const
    using const_slice_type = T const&;
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
  class Block<IntermediateExamplePlugin> : public IntermediateExamplePlugin {
   public:
    using Parent = IntermediateExamplePlugin;
    using Parent::Variables;

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
      return blocks::get<Tag>(*this)[index];
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return blocks::get<Tag>(*this)[index];
    }

    ///////////////////////////// end conformance //////////////////////////////

    Block() : variables_(){};
    ~Block() = default;
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

    // normal slice
    auto block_slice = blocks::slice(block, 1UL);
    static_assert(blocks::IsBlockSlice<decltype(block_slice)>::value,
                  "Not a constant block slice");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(block_slice) == 3);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_slice) ==
          approx(3.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(block_slice) == 1);

    // assign to a slice
    (blocks::get<tags::ExampleAcceleration>(block_slice) = 3);
    // to be reflected in blocks
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{0, 3, 2});

    // const block slice
    auto const_block_slice = blocks::slice(const_block_alias, 1UL);
    static_assert(blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
                  "Not a constant block slice");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_slice) ==
          3);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(const_block_slice) ==
          approx(3.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_slice) == 3);

    // xvalue
    CHECK(blocks::get<tags::ExampleInitializedPosition>(std::move(block)) ==
          std::vector<int>{2, 3, 4});
    CHECK_ITERABLE_APPROX(
        blocks::get<tags::ExampleInitializedVelocity>(std::move(block)),
        (std::vector<float>{2.5F, 3.5F, 4.5F}));
    CHECK(blocks::get<tags::ExampleAcceleration>(std::move(block)) ==
          std::vector<int>{0, 3, 2});
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
  class Block<AdvancedExamplePlugin> : public AdvancedExamplePlugin {
   public:
    using Parent = AdvancedExamplePlugin;
    // Conformance for initialization
    using Parent::InitializedVariables;
    using Parent::Variables;

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
      return blocks::get<Tag>(*this)[index];
    }

    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return blocks::get<Tag>(*this)[index];
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
    };

    ~Block() = default;
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

    CHECK(blocks::get<tags::ExampleInitializedPosition>(block_slice) == 2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_slice) ==
          approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(block_slice) == 12);

    // assign to a slice
    (blocks::get<tags::ExampleAcceleration>(block_slice) = 3);
    // to be reflected in blocks
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{5, 3, 21});

    // const block slice
    auto const_block_slice = blocks::slice(const_block_alias, 1UL);
    static_assert(blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
                  "Not a constant block slice");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_slice) ==
          2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(const_block_slice) ==
          approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_slice) == 3);

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
      : public CompositionExamplePlugin<Block> {
   public:
    using Parent = CompositionExamplePlugin<Block>;
    // Conformance for initialization
    using Parent::InitializedVariables;
    using Parent::Variables;

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
      return blocks::get<Tag>(*this)[index];
    }

    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return blocks::get<Tag>(*this)[index];
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
    };

    ~Block() = default;
  };

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
    static_assert(blocks::IsBlockSlice<decltype(block_slice)>::value,
                  "Not a constant block slice");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(block_slice) == 2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_slice) ==
          approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(block_slice) == 12);

    // assign to a slice
    (blocks::get<tags::ExampleAcceleration>(block_slice) = 3);
    // to be reflected in blocks
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{5, 3, 21});

    // const block slice
    auto const_block_slice = blocks::slice(const_block_alias, 1UL);
    static_assert(blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
                  "Not a constant block slice");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_slice) ==
          2);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(const_block_slice) ==
          approx(6.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_slice) == 3);

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

struct FacadedIntermediateExamplePlugin {
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
      return blocks::get<Tag>(*this)[index];
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t index) const& {
      return blocks::get<Tag>(*this)[index];
    }

    ///////////////////////////// end conformance //////////////////////////////

    Block() = default;
    ~Block() = default;
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

    CHECK(blocks::get<tags::ExampleInitializedPosition>(block_slice) == 3);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(block_slice) ==
          approx(3.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(block_slice) == 1);

    // assign to a slice
    (blocks::get<tags::ExampleAcceleration>(block_slice) = 3);
    // to be reflected in blocks
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_alias) ==
          std::vector<int>{0, 3, 2});

    // const block slice
    auto const_block_slice = blocks::slice(const_block_alias, 1UL);
    static_assert(blocks::IsConstBlockSlice<decltype(const_block_slice)>::value,
                  "Not a constant block slice");

    CHECK(blocks::get<tags::ExampleInitializedPosition>(const_block_slice) ==
          3);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(const_block_slice) ==
          approx(3.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(const_block_slice) == 3);

    // subscript the main block
    auto new_block_slice = block[elastica::end - 1UL];

    CHECK(blocks::get<tags::ExampleInitializedPosition>(new_block_slice) == 4);
    CHECK(blocks::get<tags::ExampleInitializedVelocity>(new_block_slice) ==
          approx(4.5F));
    CHECK(blocks::get<tags::ExampleAcceleration>(new_block_slice) == 2);

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
}  // namespace
