//
// Created by Tejaswin Parthasarathy on 5/13/21.
//
#include <algorithm>
#include <sstream>
#include <string>
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Systems/Block/Block/Metadata.hpp"
#include "Systems/Block/Block/Protocols.hpp"
#include "Systems/Block/Block/Types.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  struct Checker {
    explicit Checker(std::stringstream const& os) : stream(os), found(true) {}
    void operator()(std::string const& expected) const {
      found &= (stream.str().find(expected) != std::string::npos);
    }
    operator bool() const { return found; }
    std::stringstream const& stream;
    mutable bool found;
  };

}  // namespace

namespace {

  /// [empty_plugin]
  struct EmptyPlugin : public ::tt::ConformsTo<blocks::protocols::Plugin> {
    using Variables = tmpl::list<>;
  };
  /// [empty_plugin]

  ELASTICA_TEST_CASE("Unit.Systems.Block.Metadata.empty", "[Systems][Unit]") {
    using Plugin = EmptyPlugin;

    std::stringstream os;
    os << blocks::metadata<Plugin>();

    std::vector<std::string> expected_strings{"Plugin: EmptyPlugin",
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

  ELASTICA_TEST_CASE("Unit.Systems.Block.Metadata.without_initialization",
                     "[Systems][Unit]") {
    using Plugin = PluginWithoutInitialization;

    std::stringstream os;
    os << blocks::metadata<Plugin>();

    std::vector<std::string> expected_strings{
        "Initialized: \"false\"", "Plugin: PluginWithoutInitialization"};

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

  ELASTICA_TEST_CASE("Unit.Systems.Block.Metadata.with_initialization",
                     "[Systems][Unit]") {
    using Plugin = PluginWithInitialization;

    std::stringstream os;
    os << blocks::metadata<Plugin>();

    std::vector<std::string> expected_strings{
        "Initialized: \"true\"", "Plugin: PluginWithInitialization"};

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
  struct Metadata<PluginWithCustomization> {
    template <typename Var>
    static auto apply() -> std::vector<std::pair<std::string, std::string>> {
      return {{"ConstSliceType",
               pretty_type::get_name<typename Var::ConstSliceType>()}};
    }
  };

}  // namespace blocks
/// [customization_for_plugin]

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.Block.Metadata.with_customization",
                     "[Systems][Unit]") {
    using Plugin = PluginWithCustomization;

    std::stringstream os;
    os << blocks::metadata<Plugin>();

    std::vector<std::string> expected_strings{
        "ConstSliceType: ", "Plugin: PluginWithCustomization"};

    auto checked = std::for_each(expected_strings.begin(),
                                 expected_strings.end(), Checker{os});
    CHECK(checked);
  }

  ELASTICA_TEST_CASE("Unit.Systems.Block.Metadata.form_map",
                     "[Systems][Unit]") {
    using Plugin = PluginWithCustomization;

    std::stringstream os;
    auto m = blocks::detail::form_map(blocks::metadata<Plugin>());
    CHECK_FALSE(m.empty());

    auto check_map = [](auto const& mapped) {
      CHECK_FALSE(mapped.empty());
      CHECK(mapped.count("Type"));
      CHECK(mapped.count("Initialized"));
      CHECK(mapped.count("Parameter"));
      CHECK(mapped.count("Rank"));
      CHECK(mapped.count("ConstSliceType"));
    };

    for (auto const& k :
         {"ExamplePosition", "ExampleVelocity", "ExampleAcceleration"}) {
      INFO(k);
      check_map(m[k]);
    }
  }

}  // namespace
