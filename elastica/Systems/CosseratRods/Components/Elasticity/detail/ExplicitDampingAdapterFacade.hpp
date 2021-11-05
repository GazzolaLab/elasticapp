//==============================================================================
/*!
//  \file
//  \brief Facade for an ExplicitDampingAdapter for use as
//  @ref cosserat_rod_component
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

///// Types always first
#include "Systems/CosseratRods/Components/Elasticity/detail/Types.hpp"
/////
#include "Systems/Block.hpp"
#include "Systems/CosseratRods/Components/Tags.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/Tags/ExplicitDampingAdapterFacadeTags.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      namespace detail {

        template <typename CRT,                 // Cosserat Rod Traits
                  typename ComputationalBlock,  // Block
                  class ElasticityModelWithBlock,
                  template <typename /* CRT */> class VariableInfo>
        class ExplicitDampingAdapterFacade : public ElasticityModelWithBlock,
                                             public VariableInfo<CRT> {
         private:
          //! Traits type
          using Traits = CRT;
          //! Variable definitions
          using VariableDefinitions = VariableInfo<Traits>;
          //! This type
          using This = ExplicitDampingAdapterFacade<Traits, ComputationalBlock,
                                                    ElasticityModelWithBlock,
                                                    VariableInfo>;
          //! Parent type
          using Parent = ElasticityModelWithBlock;

         protected:
          //        // Here instead of doing
          //         using Interface = typename Parent::Interface;
          //        // we do the following
          //        // this is because in the earlier case the CRTP class to the
          //        Interface
          //        // is the Parent, and only its
          //        compute_internal_modeled_loads_impl()
          //        // will be called. Rather now the current class's
          //        // compute_internal_modeled_loads_impl() will be called
          //        // using Interface = ElasticityInterface<Traits, This>;
          //        // correspondingly for unambigious recognition of the
          //        compute_internal_forces
          //        // member we bring it explicitly into this namespace
          ////       public:
          ////        using Interface::compute_internal_forces;
          ////        using Interface::compute_internal_torques;

         protected:
          // Meanwhile we still use the Parent's InitializedVariables and not
          // the interface's initialized variables
          using InitializedVariables =
              tmpl::append<typename Parent::InitializedVariables,
                           typename VariableDefinitions::InitializedVariables>;
          using ComputedVariables =
              tmpl::append<typename Parent::ComputedVariables,
                           typename VariableDefinitions::ComputedVariables>;
          using Variables =
              tmpl::append<typename Parent::Variables,
                           typename VariableDefinitions::Variables>;
          using typename VariableDefinitions::ForceDampingCoefficient;
          using typename VariableDefinitions::TorqueDampingCoefficient;

         private:
          // Form friendships
          // friend Parent; don't need since it public derived
          //        friend Interface;

         private:
          // CRTP section
          // \note : cant use CRTP helper because it does not expect template
          // template parameters
          using Self = ComputationalBlock;
          using Reference = Self&;
          using ConstReference = Self const&;

          // For own use
          inline constexpr auto self() & noexcept -> Reference {
            return static_cast<Reference>(*this);
          }

          inline constexpr auto self() const& noexcept -> ConstReference {
            return static_cast<ConstReference>(*this);
          }

         protected:
          // Initializer always protected
          template <typename DownstreamBlock, typename CosseratInitializer>
          static void initialize(DownstreamBlock& downstream_block,
                                 CosseratInitializer&& initializers) {
            // 1. Initializer parent's first
            Parent::initialize(downstream_block,
                               std::forward<CosseratInitializer>(initializers));

            // 2. Initialize generators next
            struct {
              CosseratInitializer value;
            } cap{std::forward<CosseratInitializer>(initializers)};
            tmpl::for_each<typename VariableDefinitions::InitializedVariables>(
                [&downstream_block, &cap  // = std::move(cap)
            ](auto v) mutable -> void {
                  using Variable = tmpl::type_from<decltype(v)>;
                  using Tag = blocks::initializer_t<Variable>;

                  auto& variable(blocks::get<Tag>(downstream_block));
                  auto&& initializer(blocks::get<Tag>(std::move(cap.value)));

                  auto dofs =
                      Variable::get_dofs(std::move(cap.value).n_elems());
                  for (std::size_t i = 0UL; i < dofs; ++i) {
                    Variable::slice(variable, i) = initializer(i);
                  }
                });
          }

         protected:
          inline void compute_internal_modeled_loads_impl() /*noexcept*/ {
            auto& internal_force(blocks::get<tags::InternalLoads>(self()));
            auto& dissipation_constant(blocks::get<ForceDampingCoefficient>(self()));
            auto& length(blocks::get<tags::ElementLength>(self()));
            auto& velocity(blocks::get<tags::Velocity>(self()));
            auto& dummy_vec(blocks::get<tags::_DummyNodeVector>(self()));
            auto elemental_velocity = Traits::Operations::average_kernel(velocity);

            Parent::compute_internal_modeled_loads_impl();
            Traits::Operations::quadrature_kernel(
                dummy_vec,
                elemental_velocity %
                    Traits::Operations::transpose(Traits::Operations::expand_for_broadcast(length))
            );
            internal_force -= Traits::Operations::transpose(Traits::Operations::expand_for_broadcast(dissipation_constant)) %
                              dummy_vec;
          }

          inline void compute_internal_modeled_torques_impl() /*noexcept*/ {
            auto& internal_torque(blocks::get<tags::InternalTorques>(self()));
            auto& dissipation_constant(blocks::get<TorqueDampingCoefficient>(self()));
            auto& angular_velocity(blocks::get<tags::AngularVelocity>(self()));
            auto& length(blocks::get<tags::ElementLength>(self()));

            Parent::compute_internal_modeled_torques_impl();
            internal_torque -= angular_velocity %
                     Traits::Operations::transpose(Traits::Operations::expand_for_broadcast(length * dissipation_constant));
          }

        };

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
