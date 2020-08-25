#pragma once

#include <array>
#include <limits>

#include "elastica/Simulator/Domain/Policies/Directions.hpp"
#include "elastica/Simulator/Domain/Span.hpp"
#include "elastica/Utilities/MakeSignalingNan.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/NonCopyable.hpp"
#include "elastica/Utilities/Requires.hpp"

namespace elastica {

  template <typename Real>
  class UnboundedDomainPolicy : public NonCopyable {
   public:
    using SpanType = Span<Real>;
    using SpanStorage = std::array<SpanType, 3UL>;

    using DirectionType = typename detail::Directions::DirectionType;
    // We do not define X, Y and Z (or derived from Directions)
    // as it is easier to debug and detect errors early

    UnboundedDomainPolicy() = default;

    ~UnboundedDomainPolicy() = default;

    void set(SpanType&&, SpanType&&, SpanType&&) const noexcept {}

    void set(Real, Real, Real, Real, Real, Real) const noexcept {}

    template <typename VecLike>
    void set(Span<VecLike>) const noexcept {}

    void set(const Vec3&, const Vec3&) const noexcept {}

    template <DirectionType Direction, Requires<(Direction < 3UL)> = nullptr>
    inline constexpr Real get_lower_bound() const noexcept {
      return -std::numeric_limits<Real>::infinity();
    }

    template <DirectionType Direction, Requires<(Direction < 3UL)> = nullptr>
    inline constexpr Real get_upper_bound() const noexcept {
      return std::numeric_limits<Real>::infinity();
    }

    template <DirectionType Direction>
    inline Real get_span() const noexcept {
      return std::numeric_limits<Real>::infinity();
    }
  };
}  // namespace elastica
