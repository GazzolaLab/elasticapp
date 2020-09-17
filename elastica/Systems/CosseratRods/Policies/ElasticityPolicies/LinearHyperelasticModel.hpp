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
        : public HyperElasticityBase<CRT, ComputationalBlock>,
          public ::tt::ConformsTo<protocols::Elastic1D> {
     private:
      using Traits = CRT;
      using This = LinearHyperelasticModel<Traits, ComputationalBlock>;
      using Parent = HyperElasticityBase<Traits, ComputationalBlock>;
      // Friend it to gain access to data() and area_impl() classes
      //      friend Parent;

      // Unfortunately here explicit damping adapter is used like a
      // regular inheritance class rather than a CRTP class
      // so we need to friend it. This is not a sustainable design
      friend ExplicitDampingAdapter<Traits, ComputationalBlock,
                                    LinearHyperelasticModel>;

     protected:
      using Base = typename Parent::Base;
      friend Base;

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

     protected:
      // These are protected to give access to derived classes
      // TODO Implementation functions
      inline auto compute_internal_modeled_loads_impl() const noexcept {
        std::cout << "Adding hyperelastic forces";
        // Geometry interface function
        //        self().compute_shear_strains();
      }

      // TODO Implementation functions
      inline auto compute_internal_modeled_torques_impl() const noexcept {
        // Geometry interface function
        std::cout << "Adding hyperelastic torques";
        //        self().compute_shear_strains();
      }

     public:
      // For some reason it doesn't work to have a protected scope here,
      // hence promoting to public
      using typename Parent::MemberTags;

     protected:
      using Parent::initialize;
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica