//==============================================================================
/*!
//  \file
//  \brief Facade for convenient implementation of linear hyper-elastic models
//  for use as @ref cosserat_rod_component
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
///
#include "Systems/CosseratRods/Components/Elasticity/detail/Types.hpp"
///
#include "Systems/Block.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/LinearHyperElasticityFacadeTags.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/TMPL.hpp"

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
        /*!\brief Variables corresponding to facade for linear hyper-elastic
         * models
         * \ingroup cosserat_rod_component
         *
         * \details
         * LinearHyperElasticityFacadeVariables contains the definitions of
         * variables used within the Blocks framework for convenient
         * implementation of new linear hyperelastic models.
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see LinearHyperElasticityFacade
         */
        template <typename CRT>
        class LinearHyperElasticityFacadeVariables {
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
          /*!\brief Variable marking the BendingTwistRigidityMatrix
           * \f$ \hat{B} \f$ within the Cosserat rod hierarchy
           */
          struct BendingTwistRigidityMatrix
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::BendingTwistRigidityMatrix,  //
                    typename Traits::DataType::Matrix,             //
                    typename Traits::Place::OnVoronoi> {};
          //********************************************************************

          //********************************************************************
          /*!\brief Variable marking the ShearStretchRigidityMatrix
           * \f$ \hat{S} \f$ within the Cosserat rod hierarchy
           */
          struct ShearStretchRigidityMatrix
              : public Traits::template CosseratRodInitializedVariable<
                    ::elastica::tags::ShearStretchRigidityMatrix,  //
                    typename Traits::DataType::Matrix,             //
                    typename Traits::Place::OnElement> {};
          //********************************************************************

          //@}
          //********************************************************************

          //**Type definitions**************************************************
          //! List of computed variables
          using ComputedVariables = tmpl::list<>;
          //! List of initialized variables
          using InitializedVariables = tmpl::list<BendingTwistRigidityMatrix,
                                                  ShearStretchRigidityMatrix>;
          //! List of all variables
          using Variables =
              tmpl::append<InitializedVariables, ComputedVariables>;
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief  Facade for convenient implementation of linear hyper-elastic
         * models
         * \ingroup cosserat_rod_component
         *
         * \details
         * LinearHyperElasticityFacade is a helper for conveniently implementing
         * new linear hyper-elastic models for use within Cosserat rods
         * implemented in the Blocks framework. It sets up common variables and
         * methods (that interface with the Geometry component) for a linear
         * hyper-elastic model that operates on a 1-D parametrized entity
         * spanning a single spatial dimension (i.e. along centerline
         * coordinates, such as a Cosserat rod data-structure) and gives the
         * loads and torques.
         *
         * \usage
         * Since LinearHyperElasticityFacade is useful for creating new
         * linear hyper-elastic modes with an interface, one should use it using
         * by deriving from this class, as shown below
         *
         * \code
         * template <typename Traits, typename Block>
         * class MyCustomLinearHyperElasticityModel
         * : public LinearHyperElasticityFacade<Traits, Block> {
         *   // ...rest of the implementation
         * };
         * \endcode
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         * \tparam ComputationalBlock The final block which is derived from the
         * current component
         *
         * \see LinearHyperElasticityFacadeVariables
         */
        template <typename CRT, typename ComputationalBlock>
        class LinearHyperElasticityFacade
            : public LinearHyperElasticityFacadeVariables<CRT>,
              public CRTPHelper<ComputationalBlock,
                                LinearHyperElasticityFacade> {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //! This type
          using This = LinearHyperElasticityFacade<Traits, ComputationalBlock>;
          //! Type defining variables
          using VariableDefinitions = LinearHyperElasticityFacadeVariables<CRT>;
          //! CRTP Type
          using CRTP =
              CRTPHelper<ComputationalBlock, LinearHyperElasticityFacade>;
          //! CRTP methods
          using CRTP::self;
          //********************************************************************

          /*
          // These need to be there to feed in stuff to hyper-elasticity base
          template <typename B>
          static inline auto get_moment_of_inertia_along_principal_directions(
              B const& block, std::size_t i) noexcept {
            return Vec3{block.template get_second_moment_of_area<B::D1>(),
                        block.template get_second_moment_of_area<B::D1>(),
                        block.template get_second_moment_of_area<B::D3>()};
            // return block.get_second_moment_of_area(i);
          }
          */

          /*
          inline auto get_moment_of_inertia_along_principal_directions(
              std::size_t idx) const noexcept {
            return self().get_second_moment_of_area(idx);
          }
          */

          //**Get methods*******************************************************
          /*!\name Get methods*/
          //@{

          //********************************************************************
          /*!\name Area along principal direction methods*/
          //@{
          /*!\brief Gets effective area along principal directions of the
           * current rod
           */
          template <typename B>
          static inline auto get_effective_area_along_principal_directions(
              B const& block, std::size_t i) noexcept -> Vec3 {
            // TODO : Return a Matrix type instead
            return Vec3{real_t(block.get_area(i) * block.shape_factor()),
                        real_t(block.get_area(i) * block.shape_factor()),
                        real_t(block.get_area(i))};
          }
          inline auto get_effective_area_along_principal_directions(
              std::size_t i) const noexcept {
            return This::get_effective_area_along_principal_directions(self(),
                                                                       i);
          }
          //@}
          //********************************************************************

          //@}
          //********************************************************************

         protected:
          //**Type definitions**************************************************
          //! List of computed variables
          using typename VariableDefinitions::ComputedVariables;
          //! List of initialized variables
          using typename VariableDefinitions::InitializedVariables;
          //! List of all variables
          using typename VariableDefinitions::Variables;
          //! Variables from definitions
          using typename VariableDefinitions::BendingTwistRigidityMatrix;
          using typename VariableDefinitions::ShearStretchRigidityMatrix;
          //********************************************************************

         protected:
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
            // 1. Initialize generators first

            // In this case, the generator expressions are customized to only
            // provide E or B, the modulus of elasticity. We are responsible
            // for putting in the correct are etc. from the expression.

            // shear and stretch only
            {
              using Variable = BendingTwistRigidityMatrix;
              using Tag = blocks::initializer_t<Variable>;

              auto& variable(blocks::get<Tag>(downstream_block));
              auto&& stiffness_along_principal_directions_initializer =
                  blocks::get<Tag>(std::move(initializers));

              const auto dofs = Variable::get_dofs(initializers.n_elems());
              for (std::size_t idx = 0U; idx < dofs; ++idx) {
                // (E, E, G) * (I_1, I_2, I_3)
                Variable::slice(variable, idx) =
                    stiffness_along_principal_directions_initializer(idx) *
                    downstream_block.get_second_moment_of_area(idx);
                /*
                    This::get_moment_of_inertia_along_principal_directions(
                        downstream_block, i);
                        */
              }
            }

            // bending and twisting only
            {
              using Variable = ShearStretchRigidityMatrix;
              using Tag = blocks::initializer_t<Variable>;

              auto& variable(blocks::get<Tag>(downstream_block));
              auto&& stiffness_along_principal_directions_initializer =
                  blocks::get<Tag>(std::move(initializers));

              const auto dofs = Variable::get_dofs(initializers.n_elems());
              for (std::size_t idx = 0U; idx < dofs; ++idx) {
                // (G, G, E) * (\alpha A, \alpha A, A)
                Variable::slice(variable, idx) =
                    stiffness_along_principal_directions_initializer(idx) *
                    This::get_effective_area_along_principal_directions(
                        downstream_block, idx);
              }
            }
          }
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica

/*
 *
* Elasticity layer
- Prerequisites : GeometryLayer
[ ] rest_B
[ ] rest_S
[ ] internal_model_loads_ (n_l)
[ ] internal_model_couples_ (tau_l)
 */

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
 * However, in this case one has to distinguish between "protecting
 * against Murphy, versus protecting against Machiavelli". However,
 * whereas the intentional misuse cannot be prevented (Machiavelli), the
 * accidental misuse via inheritance is rather improbable (Murphy). aka
 * people need to know about CRTP first. Techniques are possible to
 * prevent deriving
 * https://stackoverflow.com/questions/18174441/crtp-and-multilevel-inheritance
 * but is overkill at this point.
 *
 * NICE: There's a simple fix for this, just by making the
 * implementation private and not giving access to the final classes, we
 * can still make it closed for extension.
 *
 */
// For all purposes, this behaves as an ABC derived from another ABC
// instead of enforcing protocols
