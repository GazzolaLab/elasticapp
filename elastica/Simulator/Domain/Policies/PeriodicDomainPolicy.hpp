#pragma once

#include <array>
#include <limits>
#include <utility>

#include "elastica/Simulator/Domain/Span.hpp"
#include "elastica/Utilities/MakeSignalingNan.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/NonCopyable.hpp"
#include "elastica/Utilities/Requires.hpp"

namespace elastica {

  template <typename Real>
  class PeriodicDomainPolicy : public NonCopyable {
   public:
    using SpanType = Span<Real>;
    using SpanStorage = std::array<SpanType, 3UL>;

    static constexpr std::size_t X = 0UL;
    static constexpr std::size_t Y = 1UL;
    static constexpr std::size_t Z = 2UL;

    PeriodicDomainPolicy() {
#ifdef ELASTICA_DEBUG
      auto inf = make_signaling_NaN<Real>();
#else
      auto inf = std::numeric_limits<Real>::infinity();
#endif
      spans_[X] = SpanType(-inf, inf);
      spans_[Y] = SpanType(-inf, inf);
      spans_[Z] = SpanType(-inf, inf);
    }

    ~PeriodicDomainPolicy() = default;

    PeriodicDomainPolicy(SpanType&& x_span, SpanType&& y_span,
                         SpanType&& z_span) {
      spans_[X] = std::move(x_span);
      spans_[Y] = std::move(y_span);
      spans_[Z] = std::move(z_span);
    }

    PeriodicDomainPolicy(Real min_x, Real max_x, Real min_y, Real max_y,
                         Real min_z, Real max_z) {
      PeriodicDomainPolicy(SpanType(min_x, max_x), SpanType(min_y, max_y),
                           SpanType(min_z, max_z));
    }

    template <typename VecLike>
    PeriodicDomainPolicy(Span<VecLike> span) {
      VecLike& mins = span.span_[span.min_idx];
      VecLike& maxs = span.span_[span.max_idx];
      PeriodicDomainPolicy(SpanType(mins[X], maxs[X]),
                           SpanType(mins[Y], maxs[Y]),
                           SpanType(mins[Z], maxs[Z]));
    }

    PeriodicDomainPolicy(const Vec3& mins, const Vec3& maxs) {
      PeriodicDomainPolicy(Span<Vec3>(mins, maxs));
    }

    template <std::size_t Direction, Requires<(Direction < 3UL)> = nullptr>
    inline constexpr Real get_lower_bound() const noexcept {
      return spans_[Direction][SpanType::min_idx];
    }

    template <std::size_t Direction, Requires<(Direction < 3UL)> = nullptr>
    inline constexpr Real get_upper_bound() const noexcept {
      return spans_[Direction][SpanType::max_idx];
    }

   private:
    SpanStorage spans_;
  };
}  // namespace elastica