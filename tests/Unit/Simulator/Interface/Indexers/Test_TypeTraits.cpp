
#include "Framework/TestHelpers.hpp"
#include "Simulator/Interface/Indexers/At.hpp"
#include "Simulator/Interface/Indexers/Range.hpp"
#include "Simulator/Interface/Indexers/TypeTraits.hpp"
#include "Utilities/Defer.hpp"
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

/// [is_at_example]
using AT = elastica::interface::detail::At_t<1, 4>;
static_assert(elastica::interface::tt::IsAt<AT>::value, "Failure!");
static_assert(not elastica::interface::tt::IsAt<int>::value, "Failure!");
/// [is_at_example]

template <typename T>
struct CheckAt : std::true_type {
  static_assert(elastica::interface::tt::IsAt<T>::value,
                "An at was recognized as a non-at");
};
template <typename T>
struct CheckNonAt : std::true_type {
  static_assert(not elastica::interface::tt::IsAt<T>::value,
                "A non-at was recognized as an at");
};
using elastica::interface::detail::At_base_t;
using elastica::interface::detail::At_t;
using expected_ats = tmpl::list<At_base_t, At_t<0, 0>, At_t<4, 3>>;
using expected_nonats =
    tmpl::append<expected_admissibles, expected_nonadmissibles>;

using at_check IGNORE_UNUSED = tmpl::transform<expected_ats, CheckAt<tmpl::_1>>;
using nonat_check IGNORE_UNUSED =
    tmpl::transform<expected_nonats, CheckNonAt<tmpl::_1>>;

/// [is_range_example]
using RT = elastica::interface::detail::Range<int, int>;
static_assert(elastica::interface::tt::IsRange<RT>::value, "Failure!");
static_assert(not elastica::interface::tt::IsRange<int>::value, "Failure!");
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

/// [is_pair_example]
using PT = elastica::interface::detail::Pair_t<int, int>;
static_assert(elastica::interface::tt::IsPair<PT>::value, "Failure!");
static_assert(not elastica::interface::tt::IsPair<int>::value, "Failure!");
/// [is_pair_example]
template <typename T>
struct CheckPair : std::true_type {
  static_assert(elastica::interface::tt::IsPair<T>::value,
                "A pair was recognized as a non-pair");
};
template <typename T>
struct CheckNonPair : std::true_type {
  static_assert(not elastica::interface::tt::IsPair<T>::value,
                "A non-pair was recognized as a pair");
};
using elastica::interface::detail::Pair_t;
using expected_pairs =
    tmpl::list<Pair_t<int, int>, Pair_t<int, decltype(::elastica::defer)>,
               Pair_t<elastica::from_end, int>,
               Pair_t<decltype(::elastica::defer), elastica::from_end>>;
using expected_nonpairs =
    tmpl::append<expected_admissibles, expected_nonadmissibles>;

using pair_check IGNORE_UNUSED =
    tmpl::transform<expected_pairs, CheckPair<tmpl::_1>>;
using nonpair_check IGNORE_UNUSED =
    tmpl::transform<expected_nonpairs, CheckNonPair<tmpl::_1>>;
