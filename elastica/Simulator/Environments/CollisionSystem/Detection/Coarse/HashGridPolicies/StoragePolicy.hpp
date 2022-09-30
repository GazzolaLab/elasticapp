#pragma once

#include <vector>

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        // TODO : Remove parallelization component for now add later
        // template <typename ParallelStrategy>
        struct StoragePolicy {
          // Input :
          // AdmissiblePrimitives = tmpl::list<Sphere, Cylinder>
          // Output:
          // CellPrimities = tmpl::list<vector<Sphere*>*, vector<Cylinder*>*>
          template <typename T>
          struct AsIDToPrimitiveIDCollection {
            using type = std::vector<T*>*;
          };

          template <typename Primitive>
          struct AsTagsToCellStorage {
            using type = AsIDToPrimitiveIDCollection<Primitive>;
          };
        };

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
