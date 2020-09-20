#pragma once

///// Types always first
#include <Systems/CosseratRods/BlockSlice.hpp>

#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
/////
#include "Systems/CosseratRods/CosseratRodTraitMacros.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    template <typename CRT>
    class ElasticityBaseTraits {
     private:
      using Traits = CRT;

     protected:
      /*
       * Computed Tags : Can be filled in
       */
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(InternalModeledLoadsTag, ElementTag,
                                            MatrixTag);
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(InternalModeledTorquesTag,
                                            VoronoiTag, MatrixTag);

     protected:
      using MemberTags =
          tmpl::list<InternalModeledLoadsTag, InternalModeledTorquesTag>;
    };

    template <typename CRT,                 // Cosserat Rod Traits
              typename ComputationalBlock>  // Constitutive Model
    class ElasticityBase : public ElasticityBaseTraits<CRT> {
     private:
      using Traits = CRT;
      using This = ElasticityBase<Traits, ComputationalBlock>;
      using Parent = ElasticityBaseTraits<CRT>;

     protected:
      using MemberTags = typename Parent::MemberTags;

     private:
      using Self = ComputationalBlock;
      using Reference = Self&;
      using ConstReference = Self const&;

      inline constexpr Reference self() noexcept {
        return static_cast<Reference>(*this);
      }

      inline constexpr ConstReference self() const noexcept {
        return static_cast<ConstReference>(*this);
      }

      //      template <typename... Initializers>
      //      using CosseratInitializer =
      //          typename Self::template CosseratInitializer<Initializers...>;
      //      using CosseratSlice = typename Self::CosseratSlice;

     protected:
      // Curse templates!
      using typename Parent::InternalModeledLoadsTag;
      using typename Parent ::InternalModeledTorquesTag;

      // Initializer always protected

      template <typename CosseratSlice, typename CosseratInitializer>
      void initialize(CosseratSlice& property_slices,
                      CosseratInitializer&& /*unused*/) {
        {
          using Tag = InternalModeledLoadsTag;
          property_slices.template get<Tag>() = 0.0;
        }

        {
          using Tag = InternalModeledTorquesTag;
          property_slices.template get<Tag>() = 0.0;
          //          get<Tag>(property_slices) = 0.0;
        }
      }

     public:
      /*
      constexpr decltype(auto) get_force() noexcept {
        return std::get<typename Self::ElementLengthCollectionTag>(
            self().data()) *
               std::get<typename Self::ReferenceElementLengthCollectionTag>(
                   self().data());
      }
      */

      // TODO
      inline void compute_internal_forces() {
        self().compute_internal_modeled_loads_impl();
      }

      inline void compute_internal_torques() {
        self().compute_internal_modeled_torques_impl();
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
