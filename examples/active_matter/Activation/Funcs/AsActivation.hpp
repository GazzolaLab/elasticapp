#include "Utilities/DefineTypes.h"
//
#include <utility>

namespace elastica {

  namespace forcing {

    namespace activation {

      template <typename F>
      struct SpatialActivationAdapter {
        real_t operator()(real_t s, real_t) const { return data(s); }
        F data;
      };

      template <typename F>
      decltype(auto) mark_spatial_activation(F func) {
        using RT = SpatialActivationAdapter<F>;
        return RT{std::move(func)};
      }

      template <typename F>
      struct TemporalActivationAdapter {
        real_t operator()(real_t, real_t t) const { return data(t); }
        F data;
      };

      template <typename F>
      decltype(auto) mark_temporal_activation(F func) {
        using RT = TemporalActivationAdapter<F>;
        return RT{std::move(func)};
      }

    }  // namespace activation

  }  // namespace forcing

}  // namespace elastica