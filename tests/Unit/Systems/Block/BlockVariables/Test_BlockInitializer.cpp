//
// Created by Tejaswin Parthasarathy on 2/11/21.
//
#include "Systems/Block/BlockVariables/BlockInitializer.hpp"

///
#include "Systems/Block/BlockVariables/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  using namespace blocks;
  using namespace blocks::detail;

  namespace {  // initializer variable test

    struct ExampleParameter;
    struct ExampleNamedFunction;

    using ExampleBlockInitializerVariable =
        BlockInitializerVariable<ExampleParameter, ExampleNamedFunction>;

    static_assert(tt::assert_conforms_to<ExampleBlockInitializerVariable,
                                         protocols::Variable>,
                  "Failure!");
  }  // namespace


}  // namespace
