#pragma once

#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  namespace collision {

    namespace response {

      // Serial verion, to mock TBB's reduce API
      template <typename FM>  // The force model to resolve contact forces
      struct ContactResolver {
        using ForceModel = FM;

        explicit ContactResolver(real_t initial_overlap = 0.0)
            : force_model_(), max_overlap_(initial_overlap){};
        ~ContactResolver() = default;

        inline void set_max_overlap(real_t overlap) { max_overlap_ = overlap; }
        inline real_t get_max_overlap() { return max_overlap_; }

        template <class BlockedContactRange>
        void operator()(const BlockedContactRange& r) const noexcept {
          real_t overlap(max_overlap_);

          auto end = r.end();
          for (auto c_id = r.begin(); c_id != end; ++c_id) {
            overlap = std::max(overlap, force_model_(*c_id));
          }

          max_overlap_ = overlap;
        }

        ForceModel force_model_;  // Stateful for stiction etc.
        mutable real_t max_overlap_;
      };

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
