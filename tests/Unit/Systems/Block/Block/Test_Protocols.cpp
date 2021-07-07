//
// Created by Tejaswin Parthasarathy on 3/9/21.
//
#include <vector>

#include "Systems/Block/Block/Protocols.hpp"
#include "Systems/Block/BlockVariables/BlockVariables.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  /// [plugin_protocol_eg]
  template <int I>
  struct ExampleParameter;

  struct ExampleRank {
    using type = std::vector<int>;
    using slice_type = int&;
    using const_slice_type = int const&;
  };
  struct ExampleExtra {
    // customize the behavior of Variable by providing a static member
    static constexpr int give_me_5() noexcept { return 5; }
  };

  // declare some variables each of which will contain a set of vectors with
  // appropriate slices
  using ExampleBlockVariable1 =
      blocks::Variable<ExampleParameter<1>, ExampleRank, ExampleExtra>;
  using ExampleBlockVariable2 =
      blocks::Variable<ExampleParameter<2>, ExampleRank, ExampleExtra>;
  using ExampleBlockVariable3 =
      blocks::Variable<ExampleParameter<3>, ExampleRank, ExampleExtra>;

  struct ExamplePlugin : ::tt::ConformsTo<blocks::protocols::Plugin> {
    using Variables = tmpl::list<ExampleBlockVariable1, ExampleBlockVariable2,
                                 ExampleBlockVariable3>;
  };

  static_assert(
      tt::assert_conforms_to<ExamplePlugin, blocks::protocols::Plugin>,
      "Failure!");
  /// [plugin_protocol_eg]

}  // namespace
