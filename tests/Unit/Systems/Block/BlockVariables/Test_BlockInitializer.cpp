//
// Created by Tejaswin Parthasarathy on 2/11/21.
//
#include "Systems/Block/BlockVariables/BlockInitializer.hpp"
#include "Systems/Block/BlockVariables/BlockVariables.hpp"

///
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/BlockVariables/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace blocks;
  using namespace blocks::detail;

  namespace {  // initializer variable test

    struct ExampleParameter;
    struct ExampleNamedFunction;

    using ExampleBlockInitializerVariable =
        BlockInitializerVariable<ExampleParameter, ExampleNamedFunction>;

    // TODO : Does this need variable semantics?
    //    static_assert(tt::assert_conforms_to<ExampleBlockInitializerVariable,
    //                                         protocols::Variable>,
    //                  "Failure!");
  }  // namespace

  namespace {

    /// [block_variable_setup]
    struct ExampleRank {
      using type = std::vector<int>;
      using slice_type = int&;
      using const_slice_type = int const&;
    };
    /// [block_variable_setup]

    struct FirstVariableTag;
    struct SecondVariableTag;

    struct MinimalPlugin {
      struct FirstVariable
          : public blocks::InitializedVariable<FirstVariableTag, ExampleRank> {
      };
      struct SecondVariable
          : public blocks::InitializedVariable<SecondVariableTag, ExampleRank> {
      };

      using InitializedVariables = tmpl::list<FirstVariable, SecondVariable>;
      using Variables = InitializedVariables;
    };
  }  // namespace
}  // namespace

namespace blocks {

  template <>
  class Block<MinimalPlugin> : MinimalPlugin {
   public:
    using MinimalPlugin::InitializedVariables;
    using MinimalPlugin::Variables;
  };

}  // namespace blocks

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.BlockInitializer.basic", "[Systems][Unit]") {
    auto initializer(blocks::initialize_block<MinimalPlugin>(
        blocks::initialize<FirstVariableTag>([](std::size_t index) -> int {
          return 1 + static_cast<int>(index);
        }),
        blocks::initialize<SecondVariableTag>([](std::size_t index) -> int {
          return static_cast<int>(index) + 2;
        })));

    auto first = blocks::get<FirstVariableTag>(std::move(initializer));
    CHECK(first(0UL) == 1);
    CHECK(first(2UL) == 3);

    // its fine to move here
    auto second = blocks::get<SecondVariableTag>(std::move(initializer));
    CHECK(second(0UL) == 2);
    CHECK(second(2UL) == 4);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockInitializer.const_correctness",
                     "[Systems][Unit]") {
    auto initializer(blocks::initialize_block<MinimalPlugin>(
        blocks::initialize<FirstVariableTag>([](std::size_t index) -> int {
          return 1 + static_cast<int>(index);
        }),
        blocks::initialize<SecondVariableTag>([](std::size_t index) -> int {
          return static_cast<int>(index) + 2;
        })));
    auto const& const_initializer =
        static_cast<decltype(initializer) const&>(initializer);

    auto first = blocks::get<FirstVariableTag>(std::move(const_initializer));
    CHECK(first(0UL) == 1);
    CHECK(first(2UL) == 3);

    // its fine to move here
    auto second = blocks::get<SecondVariableTag>(std::move(const_initializer));
    CHECK(second(0UL) == 2);
    CHECK(second(2UL) == 4);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockInitializer.captures",
                     "[Systems][Unit]") {
    std::size_t first_called = 0UL;
    std::size_t second_called = 0UL;
    auto initializer(blocks::initialize_block<MinimalPlugin>(
        blocks::initialize<FirstVariableTag>(
            [&first_called](std::size_t index) -> int {
              ++first_called;
              return 1 + static_cast<int>(index);
            }),
        blocks::initialize<SecondVariableTag>(
            [&second_called](std::size_t index) -> int {
              ++second_called;
              return static_cast<int>(index) + 2;
            })));

    auto first = blocks::get<FirstVariableTag>(std::move(initializer));
    CHECK(first(0UL) == 1);
    CHECK(first_called == 1);
    CHECK(first(2UL) == 3);
    CHECK(first_called == 2);

    auto second = blocks::get<SecondVariableTag>(std::move(initializer));
    CHECK(second(0UL) == 2);
    CHECK(second_called == 1);
    CHECK(second(2UL) == 4);
    CHECK(second_called == 2);
  }

  ELASTICA_TEST_CASE("Unit.Systems.BlockInitializer.overloads",
                     "[Systems][Unit]") {
    std::size_t first_called = 0UL;
    std::size_t second_called = 0UL;
    auto initializer(blocks::initialize_block<MinimalPlugin>(
        blocks::initialize<FirstVariableTag>(
            [&first_called](std::size_t index) -> int {
              ++first_called;
              return 1 + static_cast<int>(index);
            },
            [](float centerline) -> int {
              return 1 + static_cast<int>(5.0 * centerline);
            }),
        blocks::initialize<SecondVariableTag>(
            [&second_called](std::size_t index) -> int {
              ++second_called;
              return static_cast<int>(index) + 2;
            },
            [](float centerline) -> int {
              return static_cast<int>(centerline) + 2;
            })));

    auto first = blocks::get<FirstVariableTag>(std::move(initializer));
    CHECK(first(0UL) == 1);
    CHECK(first_called == 1);
    CHECK(first(2.0F) == 11);
    CHECK(first_called == 1);

    auto second = blocks::get<SecondVariableTag>(std::move(initializer));
    CHECK(second(0UL) == 2);
    CHECK(second_called == 1);
    CHECK(second(2.5F) == 4);
    CHECK(second_called == 1);
  }

}  // namespace
