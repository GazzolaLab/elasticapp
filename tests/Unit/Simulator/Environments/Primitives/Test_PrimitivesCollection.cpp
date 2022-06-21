//
// Created by Tejaswin Parthasarathy on 4/16/22.
//
#include <algorithm>  // fill
#include <array>
#include <atomic>
#include <tuple>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/CapsuleFromRodPrimitive.hpp"
#include "Simulator/Environments/Primitives/CapsulePrimitive.hpp"
#include "Simulator/Environments/Primitives/PrimitivesCollection.hpp"
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace elastica::collision;

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.PrimitivesCollection",
                     "[Unit][Simulator][Environments]") {
    using Primitives =
        tmpl::list<SpherePrimitive, CapsuleFromRodPrimitive, CapsulePrimitive>;
    using Collection = PrimitivesCollection<Primitives>;

    SECTION("Construction") {
      EMPTY_TEST();

      Collection collection;
      // Collection another(collection);
      Collection yet_another(std::move(collection));
    }
    SECTION("Access") {
      EMPTY_TEST();

      Collection collection;
      elastica::ignore_unused(collection.data());
      elastica::ignore_unused(cpp17::as_const(collection).data());

      elastica::ignore_unused(collection.get_collection<SpherePrimitive>());
      elastica::ignore_unused(
          cpp17::as_const(collection).get_collection<SpherePrimitive>());

      elastica::ignore_unused(get_collection<SpherePrimitive>(collection));
      elastica::ignore_unused(
          get_collection<SpherePrimitive>(cpp17::as_const(collection)));

      static_assert(collection.get_number_of_primitive_types() == 3,
                    "Meta-failure");
    }
  }

  using MockImposedPrimitiveOne = std::tuple<int, std::uint16_t>;
  using MockImposedPrimitiveTwo = std::tuple<char, double>;

  inline decltype(auto) setup_collection_with_state(const std::size_t n_units) {
    using PrimitiveList =
        tmpl::list<MockImposedPrimitiveOne, MockImposedPrimitiveTwo>;
    using Collection = PrimitivesCollection<PrimitiveList>;
    Collection collection;

    auto initializers =
        std::make_tuple(std::make_tuple(-5, 45U), std::make_tuple('c', 450.03));
    // gives n_units / 2 blocks of ECBlock and n_units ECNamedBlock
    constexpr auto n_primitives = tmpl::size<PrimitiveList>::value;
    auto add_primitives = [&](auto Is) {
      using P = tmpl::at_c<PrimitiveList, Is>;
      auto initializer = std::get<Is>(initializers);
      for (auto i = 0UL; i < n_units; ++i)
        get_collection<P>(collection)
            .emplace_back(std::get<0UL>(initializer),
                          std::get<1UL>(initializer));
    };
    index_apply<n_primitives>(
        [&](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(add_primitives(Is)); });
    return collection;
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.PrimitivesCollection.for_each",
      "[Unit][Simulator][Environments]") {
    const std::size_t n_units = 4UL;
    auto collection = setup_collection_with_state(n_units);

    using Counter = std::atomic<std::size_t>;
    std::array<Counter, collection.get_number_of_primitive_types()> counters;
    std::array<Counter, collection.get_number_of_primitive_types()>
        expected_counters;

    std::fill(std::begin(counters), std::end(counters), 0UL);
    std::fill(std::begin(expected_counters), std::end(expected_counters),
              n_units);

    auto func = index_apply<collection.get_number_of_primitive_types()>(
        [&](auto... Is) {
          // using P = tmpl::at_c<typename
          // decltype(sim)::ListOfAdmissibleImposedPrimitives, Is>;
          return make_overloader(
              [&i = counters[Is]](
                  tmpl::at_c<typename decltype(collection)::PrimitiveList,
                             Is> const&) { ++i; }...);
        });

    for_each(collection, func);

    CHECK(counters == expected_counters);

    int sum = 0;
    for_each<tmpl::list<MockImposedPrimitiveOne>>(
        collection, [&sum, &i = counters[0]](auto const& sys) {
          ++i;
          sum += std::get<int>(sys);
        });
    expected_counters[0UL] += n_units;
    CHECK(counters == expected_counters);
    CHECK(sum == (-5 * int(n_units)));  // for my own sanity
  }

}  // namespace
