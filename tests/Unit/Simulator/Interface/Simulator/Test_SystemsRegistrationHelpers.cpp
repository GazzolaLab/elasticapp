//
// Created by Tejaswin Parthasarathy on 5/15/22.
//

#include <type_traits>

#include "Simulator/Interface/Simulator/SystemsRegistrationHelpers.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  /// [has_emplace_back_eg]
  using ::elastica::interface::detail::tt::has_emplace_back_t;
  struct HasEmplaceBack {
    template <typename R>
    void emplace_back(int) & {}
  };
  struct DoesNotHaveEmplaceBack {};

  static_assert(typename has_emplace_back_t<HasEmplaceBack, float, int>::type{},
                "Meta-failure");
  struct Custom;
  // Custom cannot be converted to int
  static_assert(
      not typename has_emplace_back_t<HasEmplaceBack, double, Custom>::type{},
      "Meta-failure");
  static_assert(not typename has_emplace_back_t<DoesNotHaveEmplaceBack, double,
                                                int>::type{},
                "Meta-failure");
  /// [has_emplace_back_eg]

  /// [has_get_invalid_ref_eg]
  using ::elastica::interface::detail::tt::has_get_invalid_ref;

  struct HasGetInvalidRef {
    template <typename R>
    void get_invalid_ref() const {}
  };
  struct DoesNotHaveGetInvalidRef {};

  static_assert(typename has_get_invalid_ref<HasGetInvalidRef, float>::type{},
                "Meta-failure");
  static_assert(
      not typename has_get_invalid_ref<DoesNotHaveGetInvalidRef, float>::type{},
      "Meta-failure");
  /// [has_get_invalid_ref_eg]

  /// [choose_overload_for_eg]
  using ::elastica::interface::detail::ChooseOverloadFor;

  struct HasIntEmplaceBack {
    template <typename R, Requires<std::is_integral<R>::value> = nullptr>
    void emplace_back(char) & {}
  };
  struct HasFloatEmplaceBack {
    template <typename R, Requires<std::is_floating_point<R>::value> = nullptr>
    void emplace_back(char) & {}
  };
  using PossibleEmplaceBackOverloads =
      tmpl::list<HasIntEmplaceBack, HasFloatEmplaceBack>;

  using FirstProbe = int;
  using FirstProbeResult =
      ChooseOverloadFor<has_emplace_back_t, PossibleEmplaceBackOverloads,
                        FirstProbe, char>;
  static_assert(typename FirstProbeResult::value_type{}, "Meta-failure");
  // The first type is a feedback to user
  static_assert(cpp17::is_same_v<typename FirstProbeResult::type::type,
                                 HasIntEmplaceBack>,
                "Meta-failure");

  using SecondProbe = float;
  using SecondProbeResult =
      ChooseOverloadFor<has_emplace_back_t, PossibleEmplaceBackOverloads,
                        SecondProbe, char>;
  // The first type is a feedback to user
  static_assert(cpp17::is_same_v<typename SecondProbeResult::type::type,
                                 HasFloatEmplaceBack>,
                "Meta-failure");

  using ThirdProbe = Custom;
  using ThirdProbeResult =
      ChooseOverloadFor<has_emplace_back_t, PossibleEmplaceBackOverloads,
                        ThirdProbe, char>;
  // The first type is a feedback to user
  static_assert(cpp17::is_same_v<typename ThirdProbeResult::type::type, Custom>,
                "Meta-failure");
  /// [choose_overload_for_eg]

}  // namespace