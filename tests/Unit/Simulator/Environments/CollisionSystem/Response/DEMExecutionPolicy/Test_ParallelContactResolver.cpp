//
// Created by Tejaswin Parthasarathy on 4/1/22.
//

#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/ContactResolverTests.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/ParallelContactResolver.hpp"  // assume to be tested
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/LinearSpringDashpot.hpp"  // assume to be tested
//

namespace {

  template <typename Model>
  struct TestResolver
      : public elastica::collision::response::ParallelContactResolver<Model> {
   public:
    using P = elastica::collision::response::ParallelContactResolver<Model>;
    using P::P;
    using P::resolve_contacts;
  };

  ELASTICA_TEST_CASE(
      "Unit.Environments.CollisionResponse.ParallelContactResolver",
      "[Unit][Environments]") {
    using Model = elastica::collision::response::LinearSpringDashpot;
    using Resolver = TestResolver<Model>;

    Resolver resolver{Model{}};
    run_resolver_test(resolver);
  }

}  // namespace
