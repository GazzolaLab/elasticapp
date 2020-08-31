#pragma once
#include <cmath>
#include <utility>

///// Types always first
#include "Systems/CosseratRods/Policies/ElasticityPolicies/Types.hpp"
#include "Utilities/DefineTypes.h"
/////
#include "Systems/CosseratRods/Policies/ElasticityPolicies/HyperElasticityBase.hpp"
#include "Systems/CosseratRods/Policies/ElasticityPolicies/Protocols.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    template <typename CRT,                 // Cosserat Rod Traits
              typename ComputationalBlock>  // Block
    class LinearHyperelasticModel
        : public HyperElasticityBase<
              CRT, LinearHyperelasticModel<CRT, ComputationalBlock>>,
          public ::tt::ConformsTo<protocols::Elastic1D> {
     private:
      using Traits = CRT;
      using This = LinearHyperelasticModel<Traits, ComputationalBlock>;
      using Parent = HyperElasticityBase<Traits, This>;
      // Friend it to gain access to data() and area_impl() classes
      friend Parent;

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

      // Interface to the CrossSection Base class
      inline constexpr decltype(auto) data() noexcept { return self().data(); }

      inline constexpr decltype(auto) data() const noexcept {
        return self().data();
      }

     private:
      // These need to be there to feed in stuff to hyper-elasticity base
      inline auto get_moment_of_inertia_along_principal_directions(
          std::size_t i) const noexcept {
        return self().get_second_moment_of_area(i);
      }

      inline auto get_effective_area_along_principal_directions(
          std::size_t i) const noexcept {
        return self().get_area() * Vec3{self().get_shape_factor(),
                                        self().get_shape_factor(), real_t(1.0)};
      }

      // TODO Implementation functions
      inline auto compute_internal_modeled_loads_impl() const noexcept {
        // Geometry interface function
        self().compute_shear_strains();
      }

     public:
      // For some reason it doesn't work to have a protected scope here,
      // hence promoting to public
      using typename Parent::MemberTags;

     protected:
      using Parent::initialize;

     public:
      constexpr decltype(auto) get_force() noexcept {
        return std::get<typename Self::ElementLengthCollectionTag>(
                   self().data()) *
               std::get<typename Self::ReferenceElementLengthCollectionTag>(
                   self().data());
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica