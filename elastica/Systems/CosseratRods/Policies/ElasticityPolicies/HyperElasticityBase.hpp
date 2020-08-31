#pragma once

///// Types always first
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
/////

#include "Systems/CosseratRods/TypeTraits/detail.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    /*
     *
  * Elasticity layer
   - Prerequisites : GeometryLayer
  [ ] rest_B
  [ ] rest_S
  [ ] internal_model_loads_ (n_l)
  [ ] internal_model_couples_ (tau_l)
     */
    template <typename CRT>
    class HyperElasticityBaseTraits {
     private:
      using Traits = CRT;

     protected:
      /*
       * Generative Tags : To be filled in by generators
       */
      struct BendingTwistRigidityMatrixCollectionTag : Traits::VoronoiMatrix {
        // should ideally be a voronoi tensor, but we are always aligned with
        // the principal shear/stretch/twist directions
        using This = BendingTwistRigidityMatrixCollectionTag;
        using Parent = typename Traits::VoronoiMatrix;
        using SliceType = typename Parent::template Slice<This>;
        using InitializerType = int;
      };

      struct ShearStretchRigidityMatrixCollectionTag : Traits::ElementMatrix {
        // should ideally be a voronoi tensor, but we are always aligned with
        // the principal shear/stretch/twist directions
        using This = ShearStretchRigidityMatrixCollectionTag;
        using Parent = typename Traits::ElementMatrix;
        using SliceType = typename Parent::template Slice<This>;
        using InitializerType = int;
      };

      /*
       * Computed Tags : Can be filled in
       */
      struct InternalModeledLoadsTag : Traits::ElementMatrix {
        using This = InternalModeledLoadsTag;
        using Parent = typename Traits::ElementMatrix;
        using SliceType = typename Parent::template Slice<This>;
      };

      struct InternalModeledTorquesTag : Traits::VoronoiMatrix {
        using This = InternalModeledTorquesTag;
        using Parent = typename Traits::VoronoiMatrix;
        using SliceType = typename Parent::template Slice<This>;
      };

     protected:
      using MemberTags =
          tmpl::list<BendingTwistRigidityMatrixCollectionTag,
                     ShearStretchRigidityMatrixCollectionTag,
                     InternalModeledLoadsTag, InternalModeledTorquesTag>;
    };


    /*
     * The following CRTP relies on a trick. Normally one uses
     * CRTP like the following:
     *
     * template <typename D>
     * class Base{
     * //...impl details
     * };
     *
     * class Derived : Base<Derived>{
     *  // impl details
     * };
     *
     * In case of multiple inheritance then two patterns exist:
     * 1. Cascade
     * template <typename D>
     * class Base{
     * //...impl details
     * };
     *
     * template <typename T>
     * class MiddleDerived : Base<MiddleDerived<T>>{
     *  // impl details
     * };
     *
     * template <typename Block>
     * class Derived : MiddleDerived<Derived<Block>>{
     *  // impl details
     * }
     *
     * class Block : Derived<Block>{
     *
     * };
     *
     * This way base has access to middle derived's functionality which
     * in turn has access to Derived's functionality.
     *
     * Advantages:
     * - this pattern marks Derived like a "final" class : no additional
     * derivation is possible and hence the hierarchy is "complete". There
     * is only one way to use it correctly.
     *
     *
     * Disadvantages:
     * - One downside is that middle derived cannot be used separately
     * if its coded for taking in Derived. (this is the patten used in
     * CrossSectionBase, for example, since CrossSectionBase is
     * like an ABC and shouldnt be used in the final block). To overcome
     * this one can use T as a dummy parameter (which
     * can be defaulted).
     * - To resolve the final
     * functions too additional code is needed (to forward to the correct
     * class).
     *
     *
     * 2. Concrete
     * template <typename D>
     * class Base{
     * //...impl details
     * };
     *
     * template <typename B>
     * class MiddleDerived : Base<B>{
     *  // impl details
     * };
     *
     * template <typename Block> // This is the block
     * class Derived : MiddleDerived<Block>{
     *  // impl details
     * }
     *
     * class Block : Derived<Block>{
     * };
     *
     * This way base has access to middle derived' and derived's overriden
     * functionality (if any), throgh the final impl class Block,
     * thus making it better in terms of code reduce.
     *
     * Advantages:
     * 1. Final implementation avaialbe in the top level class : so it
     * looks like a conventional CRTP hierarchy.
     *
     * Disadvantages:
     * 1. More tightly coupled code to the Block : i.e. we need a Block
     * class to do anything. This is not that disadvantageous as Block
     * is needed for a concrete implementation of CRTP anyway.
     * 2. Easy to extend and misuse.
     * i.e. one can easily make a
     *
     * template <tpyename B>
     * NewDerived : Derived<B>{
     *  // crazy functionality
     * };
     *
     * While this seems beneficial from OCP, this gives too much freedom.
     * Quoting Sutter,
     * However, in this case one has to distinguish between "protecting against Murphy,
     * versus protecting against Machiavelli". However, whereas the intentional misuse
     * cannot be prevented (Machiavelli), the accidental misuse via inheritance is
     * rather improbable (Murphy). aka people need to know about CRTP first.
     * Techniques are possible to prevent deriving
     * https://stackoverflow.com/questions/18174441/crtp-and-multilevel-inheritance
     * but is overkill at this point.
     */
    // For all purposes, this behaves as an ABC instead of enforcing protocols
    template <typename CRT,                            // Cosserat Rod Traits
              typename HyperElasticConstitutiveModel>  // Constitutive Model
    class HyperElasticityBase : public HyperElasticityBaseTraits<CRT> {
     private:
      using Traits = CRT;
      using HECM = HyperElasticConstitutiveModel;
      using This = HyperElasticityBase<Traits, HECM>;
      using HEBT = HyperElasticityBaseTraits<CRT>;

     protected:
      using MemberTags = typename HEBT::MemberTags;

     private:
      using GeneratedTags =
          tmpl::filter<MemberTags, tt::CheckForInitializerTagType<tmpl::_1>>;
      using ComputedTags = tmpl::list_difference<MemberTags, GeneratedTags>;

      using Self = HECM;
      using Reference = Self&;
      using ConstReference = Self const&;

      inline constexpr Reference self() noexcept {
        return static_cast<Reference>(*this);
      }

      inline constexpr ConstReference self() const noexcept {
        return static_cast<ConstReference>(*this);
      }

     protected:
      // Curse templates!
      using typename HEBT::BendingTwistRigidityMatrixCollectionTag;
      using typename HEBT::InternalModeledLoadsTag;
      using typename HEBT::InternalModeledTorquesTag;
      using typename HEBT::ShearStretchRigidityMatrixCollectionTag;

      // Initializer always protected
      template <class InitializerTuple, typename... Members>
      void initialize(std::size_t n_elems, std::tuple<Members...>& members_,
                      InitializerTuple&& initializers) {
        // 1. Initialize generators first

        // In this case, the generator expressions are customized to only
        // provide E or B, the modulus of elasticity. We are responsible
        // for putting in the correct are etc. from the expression.

        // shear and stretch only
        {
          using Tag = ShearStretchRigidityMatrixCollectionTag;
          auto& member = Traits::template get_slice<Tag>(members_);

          auto&& stiffness_along_principal_directions_initializer =
              Traits::template get_initializer<Tag>(
                  std::forward<InitializerTuple>(initializers));

          auto dofs = Tag::get_dofs(n_elems);
          for (std::size_t i = 0; i < dofs; ++i) {
            // (E, E, G) * (I_1, I_2, I_3)
            Tag::slice(member, i) =
                stiffness_along_principal_directions_initializer(i) *
                self().get_moment_of_inertia_along_principal_directions(i);
          }
        }

        // Same as before
        // bending and twisting only
        {
          using Tag = BendingTwistRigidityMatrixCollectionTag;
          auto& member = Traits::template get_slice<Tag>(members_);

          auto&& stiffness_along_principal_directions_initializer =
              Traits::template get_initializer<Tag>(
                  std::forward<InitializerTuple>(initializers));

          auto dofs = Tag::get_dofs(n_elems);
          for (std::size_t i = 0; i < dofs; ++i) {
            // (E, E, G) * (I_1, I_2, I_3)
            Tag::slice(member, i) =
                stiffness_along_principal_directions_initializer(i) *
                self().get_effective_area_along_principal_directions(i);
          }
        }

        // Computed tags

        {
          using Tag = InternalModeledLoadsTag;
          Traits::template get_slice<Tag>(members_) = 0.0;
        }

        {
          using Tag = InternalModeledTorquesTag;
          Traits::template get_slice<Tag>(members_) = 0.0;
        }
      }

     public:
      // TODO
      inline void compute_internal_forces(){
        self().compute_internal_modeled_loads_impl();
      }

      inline void compute_internal_torques(){
        self().compute_internal_modeled_torques_impl();
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica