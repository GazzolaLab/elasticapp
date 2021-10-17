//
// Created by Tejaswin Parthasarathy on 10/10/21.
//

#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Utilities/StdHelpers.hpp"  // for serializing vector
#include "Utilities/StdVectorHelpers.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.append", "[Unit][Utilities]") {
    SECTION("POD vector types") {
      std::vector<int> source{1, 2, 3, 4, 5};
      std::vector<int> destination{-1, 0};
      append(std::move(source), destination);
      CHECK(destination == std::vector<int>{-1, 0, 1, 2, 3, 4, 5});
    }

    SECTION("non-copyable vector types") {
      std::vector<NonCopyable> source(5UL);
      std::vector<NonCopyable> destination(6UL);

      append(std::move(source), destination);
      CHECK(destination == std::vector<NonCopyable>(11UL));
    }
  }

  struct NCI : NonCopyable {
    using NonCopyable::NonCopyable;
    explicit NCI(int l) : i(l){};
    NCI(NCI&&) = default;
    NCI& operator=(NCI&&) = default;
    int i;
  };

  ELASTICA_TEST_CASE("Unit.Utilities.flatten", "[Unit][Utilities]") {
    SECTION("POD vector types") {
      std::vector<int> expected{1, 2, 3, 4, 5, 9, 99, 999};
      SECTION("One level") {
        std::vector<std::vector<int>> unflattened{{1, 2, 3, 4, 5},
                                                  {9, 99, 999}};
        std::vector<int> flattened = flatten(std::move(unflattened));
        CHECK(flattened == expected);
      }
      SECTION("Two levels") {
        std::vector<std::vector<std::vector<int>>> unflattened{
            {{1, 2, 3}, {4, 5}}, {{9, 99}, {999}}};
        std::vector<std::vector<int>> flattened =
            flatten(std::move(unflattened));
        CHECK(flattened ==
              std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {9, 99}, {999}});
        CHECK(flatten(std::move(flattened)) == expected);
      }
    }
    SECTION("non-copyable vector types") {
      // unflattened.resize(2);
      std::vector<NCI> temp;
      temp.emplace_back(NCI(1));
      temp.emplace_back(NCI(2));
      temp.emplace_back(NCI(3));
      temp.emplace_back(NCI(4));
      temp.emplace_back(NCI(5));
      std::vector<std::vector<NCI>> unflattened;
      unflattened.emplace_back(std::move(temp));

      std::vector<NCI> temp2;
      temp2.emplace_back(NCI(9));
      temp2.emplace_back(NCI(99));
      temp2.emplace_back(NCI(999));
      unflattened.emplace_back(std::move(temp2));

      // unflattened[0].emplace_back(std::move(temp));
      //
      //      , {  }
      std::vector<NCI> flattened = flatten(std::move(unflattened));
      std::vector<NCI> expected;
      expected.emplace_back(1);
      expected.emplace_back(2);
      expected.emplace_back(3);
      expected.emplace_back(4);
      expected.emplace_back(5);
      expected.emplace_back(9);
      expected.emplace_back(99);
      expected.emplace_back(999);

      CHECK(flattened == expected);

    }
  }

}  // namespace