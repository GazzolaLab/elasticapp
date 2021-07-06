//
// Created by Tejaswin Parthasarathy on 1/14/21.
//

#pragma once

namespace elastica {

  namespace interface {

    namespace constraints {

      // 0. Policy classes:
      // - how do you determine a minimal unit (per block? per system?)
      // - do we even want to do preprocessing using DP? (we can let the TBB runtime figure it out)
        // - if so whats the functional for optimizing the contents of a batch?
        // (total operations? total indices? size of system?)
      struct ConstrainerBatch{
        // parameter:
        // 1. size of independently executable batches (say a power of 2)

        // 1. Get a constrainer ID and put it in an appropriate pool for
        // processing
        // pool determined by : 1) minimal unit and 2) actual type of system
        // use : pretty_type for hashing actual type as a string and then use
        // an integral hash for the minimal unit (blockID or systemID) followed
        // by a hash_combined
        // 2. Perform a minimum sum partition on the values
        // 3. Get a processed Batch list and feed it to Constraints in parallel

        // Data
        // Batch = std::vector<ConstrainerID>
        // Batches = std::vector<Batch> ?
        // Batches batches_;
      };

    }

  }  // namespace interface

}  // namespace elastica