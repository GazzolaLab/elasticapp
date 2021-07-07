//
// Created by Tejaswin Parthasarathy on 5/13/21.
//
#include <algorithm>
#include <sstream>
#include <string>
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block/DisplayVariables.hpp"
#include "Systems/Block/Block/Protocols.hpp"
#include "Systems/Block/Block/Types.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  struct Checker {
    Checker(std::stringstream const& os) : stream(os), found(true) {}
    void operator()(std::string const& expected) const {
      found &= (stream.str().find(expected) != std::string::npos);
    }
    operator bool() const { return found; }
    std::stringstream const& stream;
    mutable bool found;
  };

  /// [not_a_plugin]
  struct NotAPlugin {
    using Variables = tmpl::list<>;
  };
  /// [not_a_plugin]

  ELASTICA_TEST_CASE("Unit.Systems.Block.DisplayVariables.not_a_plugin",
                     "[Systems][Unit]") {
    using Plugin = NotAPlugin;

    std::stringstream os;
    blocks::display_variable_summary<Plugin>(os);

    std::vector<std::string> expected_strings{"Name : NotAPlugin",
                                              "Not a plugin!"};

    auto checked = std::for_each(expected_strings.begin(),
                                 expected_strings.end(), Checker{os});
    CHECK(checked);
  }

}  // namespace

namespace {

  /// [empty_plugin]
  struct EmptyPlugin : public ::tt::ConformsTo<blocks::protocols::Plugin> {
    using Variables = tmpl::list<>;
  };
  /// [empty_plugin]

  ELASTICA_TEST_CASE("Unit.Systems.Block.DisplayVariables.empty",
                     "[Systems][Unit]") {
    using Plugin = EmptyPlugin;

    std::stringstream os;
    blocks::display_variable_summary<Plugin>(os);

    std::vector<std::string> expected_strings{"Plugin name : EmptyPlugin",
                                              "No variables!"};
    auto checked = std::for_each(expected_strings.begin(),
                                 expected_strings.end(), Checker{os});
    CHECK(checked);
  }

}  // namespace

// variables setup
namespace {

  template <int I>
  using Int = std::integral_constant<int, I>;

  template <int I>
  struct DummyVariable {
    static constexpr int value = I;
    using Parameter = Int<I>;
    // dummy rank type
    using Rank = Parameter;
    using type = int;
    using SliceType = int&;
    using ConstSliceType = const int&;
  };

}  // namespace

namespace {

  /// [no_init_plugin]
  struct PluginWithoutInitialization
      : public ::tt::ConformsTo<blocks::protocols::Plugin> {
    // Declare the variables
    // !! New !! Everything is a vector now!
    struct ExamplePosition : DummyVariable<1> {};
    struct ExampleVelocity : DummyVariable<2> {};
    struct ExampleAcceleration : DummyVariable<3> {};
    // All variables
    using Variables =
        tmpl::list<ExamplePosition, ExampleVelocity, ExampleAcceleration>;
  };
  /// [no_init_plugin]

  ELASTICA_TEST_CASE(
      "Unit.Systems.Block.DisplayVariables.without_initialization",
      "[Systems][Unit]") {
    using Plugin = PluginWithoutInitialization;

    std::stringstream os;
    blocks::display_variable_summary<Plugin>(os);

    std::vector<std::string> expected_strings{
        "Initialized? : false", "Plugin name : PluginWithoutInitialization"};

    auto checked = std::for_each(expected_strings.begin(),
                                 expected_strings.end(), Checker{os});
    CHECK(checked);
  }

}  // namespace

namespace {

  template <int I>
  struct DummyInitializedVariable {
    static constexpr int value = I;
    using Parameter = Int<I>;
    // dummy rank type
    using Rank = Parameter;
    using Initializer = Parameter;
    using type = int;
    using SliceType = int&;
    using ConstSliceType = const int&;
  };

  /// [init_plugin]
  struct PluginWithInitialization
      : public ::tt::ConformsTo<blocks::protocols::Plugin> {
    // Declare the variables
    // !! New !! Everything is a vector now!
    struct ExamplePosition : DummyInitializedVariable<1> {};
    struct ExampleVelocity : DummyInitializedVariable<2> {};
    struct ExampleAcceleration : DummyVariable<3> {};

    // !! New !! Variables to be initialized are put in a separate list
    using InitializedVariables = tmpl::list<ExamplePosition, ExampleVelocity>;
    // !! New !! Other Variables (that can be computed based on initialized
    // variables) are put in a different list
    using ComputedVariables = tmpl::list<ExampleAcceleration>;
    // All variables
    using Variables = tmpl::append<InitializedVariables, ComputedVariables>;
  };
  /// [init_plugin]

  ELASTICA_TEST_CASE("Unit.Systems.Block.DisplayVariables.with_initialization",
                     "[Systems][Unit]") {
    using Plugin = PluginWithInitialization;

    std::stringstream os;
    blocks::display_variable_summary<Plugin>(os);

    std::vector<std::string> expected_strings{
        "Initialized? : true", "Plugin name : PluginWithInitialization"};

    auto checked = std::for_each(expected_strings.begin(),
                                 expected_strings.end(), Checker{os});
    CHECK(checked);
  }

}  // namespace

namespace {

  /// [customized_plugin]
  struct PluginWithCustomization
      : public ::tt::ConformsTo<blocks::protocols::Plugin> {
    // Declare the variables
    struct ExamplePosition : DummyInitializedVariable<1> {};
    struct ExampleVelocity : DummyInitializedVariable<2> {};
    struct ExampleAcceleration : DummyVariable<3> {};

    using InitializedVariables = tmpl::list<ExamplePosition, ExampleVelocity>;
    using ComputedVariables = tmpl::list<ExampleAcceleration>;
    using Variables = tmpl::append<InitializedVariables, ComputedVariables>;
  };
  /// [customized_plugin]

}  // namespace

/// [customization_for_plugin]
namespace blocks {

  template <>
  struct DisplayVariables<PluginWithCustomization> {
    template <typename Var>
    static void apply(std::ostream& out) {
      out << "ConstSliceType : "
          << pretty_type::get_name<typename Var::ConstSliceType>() << "\n";
    }
  };

}  // namespace blocks
/// [customization_for_plugin]

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Block.DisplayVariables.with_customization",
                     "[Systems][Unit]") {
    using Plugin = PluginWithCustomization;

    std::stringstream os;
    blocks::display_variable_summary<Plugin>(os);

    std::vector<std::string> expected_strings{
        "ConstSliceType :", "Plugin name : PluginWithCustomization"};

    auto checked = std::for_each(expected_strings.begin(),
                                 expected_strings.end(), Checker{os});
    CHECK(checked);
  }

}  // namespace
