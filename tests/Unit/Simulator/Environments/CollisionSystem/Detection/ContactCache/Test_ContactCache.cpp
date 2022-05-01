//
// Created by Tejaswin Parthasarathy on 3/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/ContactCache/ContactCache.hpp"
#include "Utilities/AsConst.hpp"

namespace {

  struct A {
    friend bool operator==(A const& first, A const& second) noexcept {
      return first.i == second.i;
    }
    int i;
  };
  struct B {
    friend bool operator==(B const& first, B const& second) noexcept {
      return first.c == second.c;
    }
    char c;
  };
  struct C {
    friend bool operator==(C const& first, C const& second) noexcept {
      return first.c == second.c;
    }
    char c;
  };

  template <typename ContainerPolicy>
  void test_contact_cache() {
    using Pairs = tmpl::list<tmpl::pair<A, B>, tmpl::pair<A, C>>;
    elastica::ContactCache<Pairs> cache;
    A a1{1}, a2{2};
    B b1{'a'}, b2{'b'}, b3{'c'};
    C c1{'d'}, c2{'e'};

    CHECK(cache.empty());

    cache.emplace_back(a1, b1);
    cache.emplace_back(a1, b2);
    cache.emplace_back(a1, b3);

    cache.emplace_back(a2, b1);
    cache.emplace_back(a2, b2);
    cache.emplace_back(a2, b3);

    cache.emplace_back(a1, c1);
    cache.emplace_back(a2, c2);

    CHECK_FALSE(cache.empty());

    // Putting a catch section in between fails!

    // cannot use this because
    /*
     * The deduced types V1 and V2 are std::decay<T1>::type and
     * std::decay<T2>::type (the usual type transformations applied to
     * arguments of functions passed by value) unless application of
     * std::decay results in std::reference_wrapper<X> for some type X, in
     * which case the deduced type is X&.
     */
    //      CHECK(cpp17::as_const(cache).data<A, B>()[0] ==
    //            std::make_pair(std::ref(a1), std::ref(b1)));
    CHECK(cpp17::as_const(cache).data<A, B>()[0].first == std::ref(a1));
    CHECK(cpp17::as_const(cache).data<A, B>()[0].second == std::ref(b1));

    CHECK(cpp17::as_const(cache).data<A, B>()[1].first == std::ref(a1));
    CHECK(cpp17::as_const(cache).data<A, B>()[1].second == std::ref(b2));

    CHECK(cpp17::as_const(cache).data<A, B>()[2].first == std::ref(a1));
    CHECK(cpp17::as_const(cache).data<A, B>()[2].second == std::ref(b3));

    CHECK(cpp17::as_const(cache).data<A, B>()[3].first == std::ref(a2));
    CHECK(cpp17::as_const(cache).data<A, B>()[3].second == std::ref(b1));

    CHECK(cpp17::as_const(cache).data<A, B>()[4].first == std::ref(a2));
    CHECK(cpp17::as_const(cache).data<A, B>()[4].second == std::ref(b2));

    CHECK(cpp17::as_const(cache).data<A, B>()[5].first == std::ref(a2));
    CHECK(cpp17::as_const(cache).data<A, B>()[5].second == std::ref(b3));

    CHECK(cpp17::as_const(cache).data<A, C>()[0].first == std::ref(a1));
    CHECK(cpp17::as_const(cache).data<A, C>()[0].second == std::ref(c1));

    CHECK(cpp17::as_const(cache).data<A, C>()[1].first == std::ref(a2));
    CHECK(cpp17::as_const(cache).data<A, C>()[1].second == std::ref(c2));

    CHECK_FALSE(cpp17::as_const(cache).data<A, B>().empty());
    CHECK_FALSE(cpp17::as_const(cache).data<A, C>().empty());
    cache.clear();
    CHECK(cpp17::as_const(cache).data<A, B>().empty());
    CHECK(cpp17::as_const(cache).data<A, C>().empty());
  }

  ELASTICA_TEST_CASE("Unit.Environments.CollisionSystem.ContactCache",
                     "[Unit][Environments]") {
    // can be tmpl packed, but whatever
    {
      using ContainerPolicy = elastica::PairwiseCacheContainerPolicy<
          elastica::configuration::sequential_policy>;
      test_contact_cache<ContainerPolicy>();
    }
    {
      using ContainerPolicy = elastica::PairwiseCacheContainerPolicy<
          elastica::configuration::parallel_policy>;
      test_contact_cache<ContainerPolicy>();
    }
    {
      using ContainerPolicy = elastica::PairwiseCacheContainerPolicy<
          elastica::configuration::hybrid_policy>;
      test_contact_cache<ContainerPolicy>();
    }
  }

}  // namespace
