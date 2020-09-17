#pragma once

///// Types always first
#include "Systems/CosseratRods/Policies/ElasticityPolicies/Types.hpp"
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
/////
#include "Systems/CosseratRods/CosseratRodTraitMacros.hpp"
#include "Systems/CosseratRods/TypeTraits/detail.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    template <typename CRT>
    struct ExplicitDampingAdapterTraits {
     private:
      using Traits = CRT;

     protected:
      /*
       * Generative Tags : To be filled in by generators
       */
      // TODO : Fill initializer
      ELASTICA_BLOCK_REGISTER_INITIALIZED_TYPE(ForceDampingCollectionTag,
                                               ElementTag, VectorTag,
                                               tags::ForceDampingProfileTag);
      ELASTICA_BLOCK_REGISTER_INITIALIZED_TYPE(TorqueDampingCollectionTag,
                                               ElementTag, VectorTag,
                                               tags::TorqueDampingProfileTag);

     protected:
      using MemberTags =
          tmpl::list<ForceDampingCollectionTag, TorqueDampingCollectionTag>;
    };

    template <typename CRT,                 // Cosserat Rod Traits
              typename ComputationalBlock,  // Block
              template <typename, typename> class ElasticityModel>
    class ExplicitDampingAdapter :
        public ElasticityModel<CRT, ComputationalBlock>,
        public ExplicitDampingAdapterTraits<CRT> {
     private:
      using Traits = CRT;
      using This =
          ExplicitDampingAdapter<Traits, ComputationalBlock, ElasticityModel>;
      using Parent = ElasticityModel<CRT, ComputationalBlock>;
      // Friend it to gain access to data() and area_impl() classes
      friend Parent;
      using EDAT = ExplicitDampingAdapterTraits<CRT>;

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

     private:
      //
      using ParentMemberTags = typename Parent::MemberTags;
      using ThisMemberTags = typename EDAT::MemberTags;
      using GeneratedTags =
          tmpl::filter<ThisMemberTags,
                       tt::CheckForInitializerTagType<tmpl::_1>>;
      using ComputedTags = tmpl::list_difference<ThisMemberTags, GeneratedTags>;

      // TODO Implementation functions
      inline auto compute_internal_modeled_loads_impl() const noexcept {
        Parent::compute_internal_modeled_loads_impl();
        // Geometry interface function
        std::cout << "Adding damping to forces";
        //        self().compute_shear_strains();
      }

      // TODO Implementation functions
      inline auto compute_internal_modeled_torques_impl() const noexcept {
        Parent::compute_internal_modeled_torques_impl();
        // Geometry interface function
        std::cout << "Adding damping to torques";
        //        self().compute_shear_strains();
      }

     public:
      // For some reason it doesn't work to have a protected scope here,
      // hence promoting to public
      using MemberTags = tmpl::append<ParentMemberTags, ThisMemberTags>;

     protected:
      // Initializer always protected
      template <class InitializerTuple, typename... Members>
      void initialize(std::size_t n_elems, std::tuple<Members...>& members_,
                      InitializerTuple&& initializers) {
        // 1. Initializer parent's first (almost no-op) in this case
        Parent::initialize(n_elems, members_,
                           std::forward<InitializerTuple>(initializers));

        // 2. Initialize generators first
        struct {
          InitializerTuple value;
        } cap{std::forward<InitializerTuple>(initializers)};
        tmpl::for_each<This::GeneratedTags>(
            [n_elems, &members_,
             cap = std::move(cap)](auto generated_member_tag) mutable -> void {
              using GeneratedMemberTag =
                  tmpl::type_from<decltype(generated_member_tag)>;

              auto& member =
                  Traits::template get_slice<GeneratedMemberTag>(members_);
              auto&& initializer =
                  Traits::template get_initializer<GeneratedMemberTag>(
                      std::forward<InitializerTuple>(cap.value));

              auto dofs = GeneratedMemberTag::get_dofs(n_elems);
              for (std::size_t i = 0; i < dofs; ++i) {
                // TODO : This doesn't work on slices, consider having the
                // slice member in the initializer itself
                GeneratedMemberTag::slice(member, i) = initializer(i);
              }
            });
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
