
#include "Framework/TestHelpers.hpp"
#include "Simulator/Interface/Indexers/TypeTraits.hpp"
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"

/// [is_admissible_index_example]
static_assert(elastica::interface::tt::IsAdmissibleIndex<int>::value,
              "Failure!");
static_assert(
    elastica::interface::tt::IsAdmissibleIndex<elastica::from_end>::value,
    "Failure!");
static_assert(not elastica::interface::tt::IsAdmissibleIndex<double>::value,
              "Failure!");
/// [is_admissible_index_example]
template <typename T>
struct CheckAdmissible : std::true_type {
  static_assert(elastica::interface::tt::IsAdmissibleIndex<T>::value,
                "An admissible index was recognized as a non-admissible index");
};
template <typename T>
struct CheckNonAdmissible : std::true_type {
  static_assert(not elastica::interface::tt::IsAdmissibleIndex<T>::value,
                "A non-admissible index was recognized as an admissible index");
};

using expected_admissibles =
    tmpl::push_back<lists::integers, elastica::from_end>;
using expected_nonadmissibles =
    tmpl::push_back<lists::floating_points, std::string>;

using admissibles_check IGNORE_UNUSED =
    tmpl::transform<expected_admissibles, CheckAdmissible<tmpl::_1>>;
using nonadmissibles_check IGNORE_UNUSED =
    tmpl::transform<expected_nonadmissibles, CheckNonAdmissible<tmpl::_1>>;

/// [is_range_example]
using RT = elastica::interface::detail::Range<int, int>;
static_assert(elastica::interface::tt::IsRange<RT>::value, "Failure!");
static_assert(!elastica::interface::tt::IsRange<int>::value, "Failure!");
/// [is_range_example]
template <typename T>
struct CheckRange : std::true_type {
  static_assert(elastica::interface::tt::IsRange<T>::value,
                "A range was recognized as a non-range");
};
template <typename T>
struct CheckNonRange : std::true_type {
  static_assert(not elastica::interface::tt::IsRange<T>::value,
                "A non-range was recognized as a range");
};
using elastica::interface::detail::Range;
using expected_ranges = tmpl::list<Range<int, int>, Range<float, int>,
                                   Range<int, elastica::from_end>>;
using expected_nonranges =
    tmpl::append<expected_admissibles, expected_nonadmissibles>;

using range_check IGNORE_UNUSED =
    tmpl::transform<expected_ranges, CheckRange<tmpl::_1>>;
using nonrange_check IGNORE_UNUSED =
    tmpl::transform<expected_nonranges, CheckNonRange<tmpl::_1>>;
