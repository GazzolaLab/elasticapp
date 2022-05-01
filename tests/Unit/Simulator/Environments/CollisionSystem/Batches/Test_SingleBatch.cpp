//
// Created by Tejaswin Parthasarathy on 4/2/22.
//

#include <functional>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/BatchAlgorithmTests.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/SingleBatch.hpp"

namespace {

  struct Config {
    using BatchGenerator = elastica::collision::batches::SingleBatch<Config>;
  };

  ELASTICA_TEST_CASE("Unit.Environments.CollisionBatches.SingleBatch",
                     "[Unit][Environments]") {
    using Alg = elastica::collision::batches::SingleBatch<Config>;
    using Contact = elastica::collision::Contact;

    Alg batch_algorithm{};
    using ContactRef = std::reference_wrapper<Contact>;
    using Batch = std::vector<ContactRef>;
    using Batches = std::vector<Batch>;
    Batches batches;

    test_batch_algorithm(batch_algorithm, batches);
  }

}  // namespace
