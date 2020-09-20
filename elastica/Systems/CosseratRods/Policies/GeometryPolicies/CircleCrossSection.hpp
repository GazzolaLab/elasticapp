#pragma once

#include <cmath>
#include <utility>

///// Types always first
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
/////
#include "Systems/CosseratRods/Policies/GeometryPolicies/CrossSectionBase.hpp"
#include "Systems/CosseratRods/Policies/GeometryPolicies/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Requires.hpp"

// Implementatino details
#include "Utilities/Math/Sqrt.hpp"
#include "Utilities/Math/Square.hpp"

namespace elastica {
  //  public RodKinematics<RodTraits>
  // Sparse_strategy
  // Dense strategy
  namespace detail_cosserat_rod {

    // TODO : Static test : assure that the size is 0 or 1
    /* DONE : Make shape protocol with
     *  1. get_dimension
     *  2. get_area
     *  3. get_volume
     *  4. template function get_second_moment_of_inertia
     *  5. shape_factor
     */

    template <typename CRT,                 // Cosserat Rod Traits
              typename ComputationalBlock>  // Block
    class CircleCrossSection
        : public CrossSectionBase<CRT,
                                  CircleCrossSection<CRT, ComputationalBlock>>,
          public ::tt::ConformsTo<protocols::Shape> {
     protected:
      using Traits = CRT;
      using This = CircleCrossSection<Traits, ComputationalBlock>;
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
      using Self = ComputationalBlock;
      using Reference = Self&;
      using ConstReference = Self const&;

      // For own use
      inline constexpr Reference self() noexcept {
        return static_cast<Reference>(*this);
      }

      inline constexpr ConstReference self() const noexcept {
        return static_cast<ConstReference>(*this);
      }

      template <typename PropertyTag>
      inline constexpr decltype(auto) get() noexcept {
        return self().template get<PropertyTag>();
      }

      template <typename PropertyTag>
      inline constexpr decltype(auto) get() const noexcept {
        return self().template get<PropertyTag>();
      }

//      // Interface to the CrossSection Base class
//      inline constexpr decltype(auto) data() noexcept { return self().data(); }
//
//      inline constexpr decltype(auto) data() const noexcept {
//        return self().data();
//      }

      template <typename Radius>
      inline static constexpr auto area_impl(Radius const& r) noexcept {
        return M_PI * sq(r);
      }

      template <typename Radius>
      inline static constexpr auto second_moment_of_area_impl(
          Radius const& r) noexcept {
        return area_impl(r) * area_impl(r) * (0.25 * M_1_PI);
      }

      template <typename Volume, typename Length>
      inline static constexpr auto dimension_from_volume_impl(
          Volume const& v, Length const& l) noexcept {
        // r = sqrt(V / l / PI)
        return sqrt(M_1_PI * v / l);
      }

     public:
      inline static constexpr auto shape_factor() noexcept -> float_type {
        // is division constexpr?
        return float_type(4.) / float_type(3.);
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
