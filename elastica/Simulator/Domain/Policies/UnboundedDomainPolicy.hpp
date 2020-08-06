#pragma once

#include <array>
#include <limits>

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

    // Not define X, Y and Z
    // easier to debug and detect errors early

    UnboundedDomainPolicy() = default;

    ~UnboundedDomainPolicy() = default;

    UnboundedDomainPolicy(SpanType&&, SpanType&&, SpanType&&){};

    UnboundedDomainPolicy(Real, Real, Real, Real, Real, Real){};

    template <typename VecLike>
    UnboundedDomainPolicy(Span<VecLike> span){};

    UnboundedDomainPolicy(const Vec3& mins, const Vec3& maxs){};

    template <std::size_t Direction, Requires<(Direction < 3UL)> = nullptr>
    inline constexpr Real get_lower_bound() const noexcept {
      return -std::numeric_limits<Real>::infinity();
    }

    template <std::size_t Direction, Requires<(Direction < 3UL)> = nullptr>
    inline constexpr Real get_upper_bound() const noexcept {
      return std::numeric_limits<Real>::infinity();
    }
  };
}  // namespace elastica
