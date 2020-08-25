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
          struct AddCellPrimitivesStorage {
            using type = std::vector<T*>*;
          };
        };

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
