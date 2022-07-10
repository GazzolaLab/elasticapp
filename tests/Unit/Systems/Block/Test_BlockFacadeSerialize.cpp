//
// Created by Tejaswin Parthasarathy on 5/20/21.
//

#include <string>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block.hpp"
#include "Systems/Block/Serialize.hpp"

// catch wants << declared and defined while comparing equality, so we include
// STD helpers
#include "Utilities/AsConst.hpp"
#include "Utilities/StdHelpers.hpp"

namespace tags {
  struct ExamplePosition;
  struct ExampleVelocity;
  struct ExampleAcceleration;
}  // namespace tags

namespace {

  template <typename T>
  struct Vector {
    using type = std::vector<T>;
    using slice_type = T&;
    // in case you want to slice a const
    using const_slice_type = T const&;
  };
  using VectorInt = Vector<int>;
  using VectorString = Vector<std::string>;

}  // namespace

namespace {

  /// [adv_plugin]
  struct AdvancedExamplePlugin {
    // Declare the variables
    struct ExamplePosition
        : blocks::Variable<tags::ExamplePosition, VectorInt> {};
    struct ExampleVelocity
        : blocks::Variable<tags::ExampleVelocity, VectorString> {};
    struct ExampleAcceleration
        : blocks::Variable<tags::ExampleAcceleration, VectorInt> {};

    // !! New !! Variables to be initialized are put in a separate list
    using InitializedVariables = tmpl::list<>;
    // !! New !! Other Variables (that can be computed based on initialized
    // variables) are put in a different list
    using ComputedVariables =
        tmpl::list<ExamplePosition, ExampleVelocity, ExampleAcceleration>;
    // All variables
    using Variables = tmpl::append<InitializedVariables, ComputedVariables>;
  };
  /// [adv_plugin]

}  // namespace

namespace blocks {

  template <>
  class Block<AdvancedExamplePlugin>
      : public BlockFacade<AdvancedExamplePlugin> {
    // no-op
  };

}  // namespace blocks

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Blocks.serialize", "[Systems][Unit]") {
    using namespace elastica::io;

    std::stringstream ss;  // any stream can be used
    using MyBlock = blocks::Block<AdvancedExamplePlugin>;

    auto as_facade = [](MyBlock& b) -> decltype(auto) {
      return static_cast<blocks::BlockFacade<AdvancedExamplePlugin>&>(b);
    };
    auto as_const_facade = [](MyBlock const& b) -> decltype(auto) {
      return static_cast<blocks::BlockFacade<AdvancedExamplePlugin> const&>(b);
    };

    MyBlock b;
    MyBlock loaded_b;
    {
      blocks::get<tags::ExamplePosition>(b) = VectorInt::type{1, 2, 3};
      blocks::get<tags::ExampleVelocity>(b) =
          VectorString::type{"Mom", "Dad", "Son", "Daughter"};
      blocks::get<tags::ExampleAcceleration>(b) = VectorInt::type{4, 5, 6};

      BinaryOutputArchive oarchive(ss);  //  Create an output archive
      oarchive(as_const_facade(b));      // Write the data to the archive
    }

    {
      BinaryInputArchive iarchive(ss);  //  Create an input archive

      iarchive(as_facade(loaded_b));  // Read the data from the archive
    }

    // stringifying this requires operator << for vectors.
    CHECK(cpp17::as_const(b).data() == cpp17::as_const(loaded_b).data());
  }

}  // namespace