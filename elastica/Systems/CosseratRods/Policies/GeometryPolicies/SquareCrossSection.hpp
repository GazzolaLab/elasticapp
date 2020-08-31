#pragma once

#include <cmath>
#include <utility>

///// Types always first
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
/////
#include "Systems/CosseratRods/Policies/GeometryPolicies/CrossSectionBase.hpp"
#include "Systems/CosseratRods/Policies/GeometryPolicies/Protocols.hpp"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Requires.hpp"

namespace elastica {
  //  public RodKinematics<RodTraits>
  // Sparse_strategy
  // Dense strategy
  namespace detail_cosserat_rod {

    // TODO : Static test : assure that the size is 0 or 1

    template <typename CRT,                 // Cosserat Rod Traits
              typename ComputationalBlock>  // Block
    class SquareCrossSection
        : public CrossSectionBase<CRT,
                                  SquareCrossSection<CRT, ComputationalBlock>>,
          public ::tt::ConformsTo<protocols::Shape> {
     protected:
      using Traits = CRT;
      using This = SquareCrossSection<Traits, ComputationalBlock>;
      using Parent = CrossSectionBase<Traits, This>;
      // Friend it to gain access to data() and area_impl() classes
      friend Parent;

     protected:
      using Parent::initialize;

     public:
      // For some reason it doesn't work to have a protected scope here,
      // hence promoting to public
      using typename Parent::MemberTags;
      // alias for real
      using typename Parent::float_type;

     private:
      using Block = ComputationalBlock&;
      using ConstBlock = ComputationalBlock const&;

      // For own use
      inline constexpr Block self() noexcept {
        return static_cast<Block>(*this);
      }

      inline constexpr ConstBlock self() const noexcept {
        return static_cast<ConstBlock>(*this);
      }

      // Interface to the CrossSection Base class
      inline constexpr decltype(auto) data() noexcept { return self().data(); }

      inline constexpr decltype(auto) data() const noexcept {
        return self().data();
      }

      template <typename InCircleRadius>
      inline static constexpr auto area_impl(InCircleRadius& r) noexcept {
        return real_t(4.0) * r * r;
      }

      template <typename InCircleRadius>
      inline static constexpr auto second_moment_of_area_impl(
          InCircleRadius& r) noexcept {
        // https://en.wikipedia.org/wiki/List_of_second_moments_of_area
        const auto area(area_impl(r));
        return area * area / real_t(12.0);
      }

      template <typename Volume, typename Length>
      inline static constexpr auto dimension_from_volume_impl(
          Volume const& v, Length const& l) noexcept {
        // r = sqrt(V / l / 4.0)
        return 0.5 * sqrt(v / l);
      }

     public:
      inline static constexpr auto shape_factor() noexcept -> float_type {
        // is division constexpr?
        // TODO : Complete
        return inf;
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
