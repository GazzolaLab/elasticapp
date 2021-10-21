//==============================================================================
/*!
//  \file
//  \brief Interface class for a Elasticity component of
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

//******************************************************************************
// Includes
//******************************************************************************
#include <type_traits>

///// Types always first
#include "Systems/CosseratRods/Components/Elasticity/detail/Types.hpp"
/////
#include "Systems/Block.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/Tags/ElasticityInterfaceTags.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      namespace detail {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Variables corresponding to the elasticity interface component
         * \ingroup cosserat_rod_component
         *
         * \details
         * ElasticityInterfaceVariables contains the definitions of
         * variables used within the Blocks framework for a 1D elastic model
         * with an interface expected as by other components.
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see ElasticityInterface
         */
        template <typename CRT>
        class ElasticityInterfaceVariables {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //********************************************************************

         protected:
          //**Variable definitions**********************************************
          /*!\name Variable definitions*/
          //@{

          //********************************************************************
          /*!\brief Variable marking InternalLoads within the Cosserat rod
           * hierarchy
           */
          struct InternalLoads : public Traits::template CosseratRodVariable<
              ::elastica::tags::InternalLoads,    //
              typename Traits::DataType::Vector,  //
              typename Traits::Place::OnNode> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking InternalStress within the Cosserat rod
           * hierarchy
           */
          struct InternalStress : public Traits::template CosseratRodVariable<
                                     ::elastica::tags::InternalStress,    //
                                     typename Traits::DataType::Vector,  //
                                     typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking InternalTorques within the Cosserat rod
           * hierarchy
           */
          struct InternalTorques : public Traits::template CosseratRodVariable<
                                       ::elastica::tags::InternalTorques,  //
                                       typename Traits::DataType::Vector,  //
                                       typename Traits::Place::OnElement> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking InternalCouple within the Cosserat rod
           * hierarchy
           */
          struct InternalCouple : public Traits::template CosseratRodVariable<
              ::elastica::tags::InternalCouple,  //
              typename Traits::DataType::Vector,  //
              typename Traits::Place::OnVoronoi> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Temporary vector and matrix for dummy purpose
           * TODO try to refactor : avoid temp memory allocations
           *  - Some exception: branch of operation
           */
          struct _DummyNodeVector: public Traits::template CosseratRodVariable<
              ::elastica::tags::_DummyNodeVector,
              typename Traits::DataType::Vector,
              typename Traits::Place::OnNode> {};
          struct _DummyElementVector: public Traits::template CosseratRodVariable<
              ::elastica::tags::_DummyElementVector,
              typename Traits::DataType::Vector,
              typename Traits::Place::OnElement> {};
          struct _DummyElementVector2: public Traits::template CosseratRodVariable<
              ::elastica::tags::_DummyElementVector2,
              typename Traits::DataType::Vector,
              typename Traits::Place::OnElement> {};
          struct _DummyVoronoiVector: public Traits::template CosseratRodVariable<
              ::elastica::tags::_DummyVoronoiVector,
              typename Traits::DataType::Vector,
              typename Traits::Place::OnVoronoi> {};
          struct _DummyMatrix: public Traits::template CosseratRodVariable<
              ::elastica::tags::_DummyMatrix,
              typename Traits::DataType::Matrix,
              typename Traits::Place::OnElement> {};
          //********************************************************************

          //@}
          //********************************************************************

          //**Type definitions**************************************************
          //! List of initialized variables
          using InitializedVariables = tmpl::list<>;
          //! List of computed variables
          using ComputedVariables = tmpl::list<
              InternalLoads,
              InternalTorques,
              InternalStress,
              InternalCouple,
              _DummyNodeVector,
              _DummyElementVector,
              _DummyElementVector2,
              _DummyVoronoiVector,
              _DummyMatrix>;
          //! List of all variables
          using Variables =
              tmpl::append<InitializedVariables, ComputedVariables>;
          //********************************************************************
        };

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Interface component corresponding to an elasticity model
         * \ingroup cosserat_rod_component
         *
         * \details
         * ElasticityInterface implements the final Elasticity component use
         * within Cosserat rods implemented in the Blocks framework. It denotes
         * an Elasticity model that operates on a 1-D parametrized entity
         * spanning a single spatial dimension (i.e. along center-line
         * coordinates, such as a Cosserat rod data-structure) and gives the
         * loads and torques. By design it only binds to classes with signature
         * of a Component
         *
         * \usage
         * Since ElasticityInterface is useful for wrapping an ElasticityModel
         * with an interface, one should use it using the Adapt class as follows
         * \code
         * template <typename Traits, typename Block>
         * class MyCustomElasticityModel : public
         * Component<MyCustomElasticityModel<Traits, Block>> {
         *   // ...implementation
         * };
         *
         * template <typename Traits, typename Block>
         * class MyCustomElasticityModelWithTheCorrectInterface : public
         * Adapt<MyCustomElasticityModel<CRT, ComputationalBlock>>::
         *      template with<ElasticityInterface> {};
         * \endcode
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         * \tparam ComputationalBlock The final block which is derived from the
         * current component
         * \tparam ElasticityModelParam The user defined elasticity model which
         * is to be wrapped in the interface type
         *
         * \see CosseratRodCrossSectionInterfaceVariables
         */
        template <typename CRT, typename ComputationalBlock,
                  template <typename /*CRT*/, typename /* ComputationalBlock*/,
                            typename... /*ElasticityModelMetaArgs*/>
                  class ElasticityModelParam,
                  typename... ElasticityModelMetaArgs>
        class ElasticityInterface<
            CRT, ComputationalBlock,
            ElasticityModelParam<CRT, ComputationalBlock,
                                 ElasticityModelMetaArgs...>>
            : public ElasticityInterfaceVariables<CRT>,
              public ElasticityModelParam<CRT, ComputationalBlock,
                                          ElasticityModelMetaArgs...>,
              public CRTPHelper<ComputationalBlock, ElasticityInterface> {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //! Parent type
          using Parent = ElasticityModelParam<Traits, ComputationalBlock,
                                              ElasticityModelMetaArgs...>;
          //! This type
          using This = ElasticityInterface<Traits, ComputationalBlock, Parent>;
          //! Type defining variables
          using VariableDefinitions = ElasticityInterfaceVariables<CRT>;
          //! CRTP type
          using CRTP = CRTPHelper<ComputationalBlock, ElasticityInterface>;
          //! CRTP methods
          using CRTP::self;
          //********************************************************************

         public:
          //**Type definitions**************************************************
          //! Model type
          using ElasticityModel = Parent;
          //********************************************************************

         protected:
          //**Type definitions**************************************************
          //! List of initialized variables
          using InitializedVariables =
              tmpl::append<typename Parent::InitializedVariables,
                           typename VariableDefinitions::InitializedVariables>;
          //! List of computed variables
          using ComputedVariables =
              tmpl::append<typename ElasticityModel::ComputedVariables,
                           typename VariableDefinitions::ComputedVariables>;
          //! List of all variables
          using Variables =
              tmpl::append<typename ElasticityModel::Variables,
                           typename VariableDefinitions::Variables>;
          //! Variables from definitions
          using typename VariableDefinitions::InternalLoads;
          using typename VariableDefinitions::InternalTorques;
          using typename VariableDefinitions::InternalCouple;
          using typename VariableDefinitions::InternalStress;

          //********************************************************************
          /*!\brief Initialize method for the current component
           *
           * \details
           * initialize() is responsible for initializing the variables present
           * in the current component in the final block of data. For
           * InitializedVariables, it fetches the corresponding initializer from
           * the set of `initializers` to fill in the data. For
           * ComputedVariables the default value is usually set. In this way,
           * each component is responsible for initializing its own data in a
           * manner that does not intrude with initialization from other
           * components.
           *
           * \tparam DownstreamBlock The final block-like object which is
           * derived from the current component
           * \tparam CosseratInitializer An Initializer corresponding to the
           * Cosserat rod hierarchy
           */
          template <typename DownstreamBlock, typename CosseratInitializer>
          static void initialize(DownstreamBlock& downstream_block,
                                 CosseratInitializer&& initializers) {
            // 1. Initialize parent
            Parent::initialize(downstream_block,
                               std::forward<CosseratInitializer>(initializers));

            // 2. Only computed variables here
            tmpl::for_each<typename VariableDefinitions::ComputedVariables>(
                [&downstream_block](auto v) {
                  using Var = tmpl::type_from<decltype(v)>;
                  using Tag = blocks::parameter_t<Var>;
                  blocks::get<Tag>(downstream_block) = 0.0;
                });
          }
          //********************************************************************

         public:
          //**Dynamic methods***************************************************
          /*!\name Dynamics methods*/
          //@{

          //********************************************************************
          /*!\brief Computers internal force from the model
           *
           * \details
           * Computes internal forces (shear force + damping force) by calling
           * compute_shear_strain() and compute_internal_modeled_loads_impl()
           *
           * Ref: eq (3.8) from RSOS
           *
           *
           * \return void/None
           *
           * \see fill later?
           */
          inline auto compute_internal_forces() {
            ElasticityModel::compute_internal_modeled_loads_impl();
            return blocks::get<InternalLoads>(self());
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Computers internal couple from the model
           *
           * \details
           * Computes internal couple by calling
           * compute_curvature() and compute_internal_modeled_torques_impl()
           *
           * \return void/None
           *
           */
          inline auto compute_internal_torques() {
            ElasticityModel::compute_internal_modeled_torques_impl();
            return blocks::get<InternalTorques>(self());
          }
          //********************************************************************

          //@}
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
