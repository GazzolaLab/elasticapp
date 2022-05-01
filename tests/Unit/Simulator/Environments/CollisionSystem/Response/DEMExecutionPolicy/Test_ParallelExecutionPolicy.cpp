//
// Created by Tejaswin Parthasarathy on 4/1/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/ResponsePolicyTests.hpp"
///
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/ParallelExecutionPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/LinearSpringDashpot.hpp"
//// assume to be tested

namespace {

  struct Config {
    using ForceModel = elastica::collision::response::LinearSpringDashpot;
    // need to have a better name
    struct KernelExecutionPolicy {
      using ExecutionPolicy = elastica::configuration::parallel_policy;
      // will also contain other static functions
    };
  };

  ELASTICA_TEST_CASE(
      "Unit.Environments.CollisionResponse.ParallelExecutionPolicy",
      "[Unit][Environments]") {
    using ResponseOp = elastica::collision::response::DEMSolverOp<Config>;
    ResponseOp response{Config::ForceModel{}};
    run_response_policy_test(response);
  }

}  // namespace
