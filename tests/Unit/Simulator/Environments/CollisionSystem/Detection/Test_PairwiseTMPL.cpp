//
// Created by Tejaswin Parthasarathy on 3/16/22.
//

#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/PairwiseTMPL.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  template <int I>
  using Integer = tmpl::integral_constant<int, I>;

  // meta checks
  /// [matrix_of_pairs_eg]
  using types = tmpl::integral_list<int, 1, 2, 3>;
  using matrix = elastica::combinations_with_replacement<types>;
  using expected_matrix = tmpl::list<
      tmpl::pair<Integer<1>, Integer<1>>, tmpl::pair<Integer<1>, Integer<2>>,
      tmpl::pair<Integer<1>, Integer<3>>, tmpl::pair<Integer<2>, Integer<1>>,
      tmpl::pair<Integer<2>, Integer<2>>, tmpl::pair<Integer<2>, Integer<3>>,
      tmpl::pair<Integer<3>, Integer<1>>, tmpl::pair<Integer<3>, Integer<2>>,
      tmpl::pair<Integer<3>, Integer<3>>>;

  static_assert(std::is_same_v<matrix, expected_matrix>, "Meta-failure");
  /// [matrix_of_pairs_eg]

  /// [matrix_of_distinct_pairs_eg]
  using types = tmpl::integral_list<int, 1, 2, 3>;
  using distinct_matrix = elastica::combinations<types>;
  using expected_distinct_matrix = tmpl::list<
      tmpl::pair<Integer<1>, Integer<1>>, tmpl::pair<Integer<1>, Integer<2>>,
      tmpl::pair<Integer<1>, Integer<3>>, tmpl::pair<Integer<2>, Integer<2>>,
      tmpl::pair<Integer<2>, Integer<3>>, tmpl::pair<Integer<3>, Integer<3>>>;

  static_assert(std::is_same_v<distinct_matrix, expected_distinct_matrix>,
                "Meta-failure");
  /// [matrix_of_distinct_pairs_eg]

  ELASTICA_TEST_CASE("Unit.Environments.CollisionSystem.pairwise_for_each",
                     "[Unit][Environments]") {
    /// [pairwise_for_each_eg]
    std::size_t repeated_pairs = 0UL;
    auto count_repeated = [&](auto first, auto second) {
      using F = tmpl::type_from<decltype(first)>;
      using G = tmpl::type_from<decltype(second)>;
      return repeated_pairs += ((F::value) == (G::value));
    };
    elastica::pairwise_for_each<distinct_matrix>(count_repeated);
    CHECK(repeated_pairs == 3UL);
    /// [pairwise_for_each_eg]
  }

}  // namespace
