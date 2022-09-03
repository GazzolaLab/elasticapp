//==============================================================================
/*!
//  \file
//  \brief Elasticity component implementing a linear hyper-elastic model with
//  diagonal stress--stain relations
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
#include <cmath>
#include <utility>

///// Types always first
#include "Systems/CosseratRods/Components/Tags.hpp"
/////
#include "Systems/CosseratRods/Components/Elasticity/Protocols.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/ElasticityInterface.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/LinearHyperElasticityFacade.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/ProtocolHelpers.hpp"
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
        /*!\brief Implementation of an elasticity component corresponding to a
         * linear hyper-elastic model with diagonal stress--stain relations
         * \ingroup cosserat_rod_component
         *
         * \details
         * Implementation details of
         * components::WithDiagonalLinearHyperElasticModel
         *
         * \see components::WithDiagonalLinearHyperElasticModel
         */
        template <typename CRT,                 // Cosserat Rod Traits
                  typename ComputationalBlock>  // Block
        class WithDiagonalLinearHyperElasticModel
            : public CRTPHelper<ComputationalBlock,
                                WithDiagonalLinearHyperElasticModel>,
              public LinearHyperElasticityFacade<CRT, ComputationalBlock>,
              public ElasticityComponent<
                  WithDiagonalLinearHyperElasticModel<CRT, ComputationalBlock>>,
              public ::tt::ConformsTo<
                  ::elastica::cosserat_rod::protocols::Elastic1D> {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //! This type
          using This =
              WithDiagonalLinearHyperElasticModel<Traits, ComputationalBlock>;
          //! Parent type
          using Parent =
              detail::LinearHyperElasticityFacade<Traits, ComputationalBlock>;
          //! CRTP Type
          using CRTP = CRTPHelper<ComputationalBlock,
                                  WithDiagonalLinearHyperElasticModel>;
          //! CRTP methods
          using CRTP::self;
          //********************************************************************

         protected:
          //**Parent methods and aliases****************************************
          //! Initialize method inherited from parent class
          using Parent::initialize;
          //! List of computed variables
          using typename Parent::ComputedVariables;
          //! List of initialized variables
          using typename Parent::InitializedVariables;
          //! List of all variables
          using typename Parent::Variables;
          //********************************************************************

         protected:
          //**Computation methods***********************************************
          /*!\name Compute methods*/
          //@{

          //********************************************************************
          /*!\brief Computes the modeled internal loads from the elasticity
           * model
           */
          inline void compute_internal_modeled_loads_impl() noexcept {
            auto& strain(blocks::get<tags::ShearStretchStrain>(self()));
            auto& reference_strain(
                blocks::get<tags::ReferenceShearStretchStrain>(self()));
            auto& shear_matrix(
                blocks::get<tags::ShearStretchRigidityMatrix>(self()));
            auto& internal_stress(blocks::get<tags::InternalStress>(self()));
            auto& internal_load(blocks::get<tags::InternalLoads>(self()));
            auto& element_dilatation(
                blocks::get<tags::ElementDilatation>(self()));
            auto& director(blocks::get<tags::Director>(self()));

            compute_shear_strains(self());

            Traits::Operations::batch_matvec(internal_stress, shear_matrix,
                                             strain - reference_strain);

            // allocating temp container (3, n_elems) for temporary storage
            auto& cosserat_internal_stress(
                blocks::get<tags::_DummyElementVector>(self()));
            auto& temp_vec(blocks::get<tags::_DummyElementVector2>(self()));
            Traits::Operations::batch_mattranspvec(temp_vec, director,
                                                   internal_stress);
            Traits::Operations::batch_division_matvec(
                cosserat_internal_stress, temp_vec, element_dilatation);

            // before doing difference, fill it to 0.0
            self().template fill_ghosts_for<tags::_DummyElementVector>();

            Traits::Operations::two_point_difference_kernel(
                internal_load, cosserat_internal_stress);

            // return internal_load;
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Computes the modeled internal torques from the elasticity
           * model
           */
          inline void compute_internal_modeled_torques_impl() noexcept {
            auto& director(blocks::get<tags::Director>(self()));
            auto& omega(blocks::get<tags::AngularVelocity>(self()));
            auto& curvature(blocks::get<tags::Curvature>(self()));
            auto& tangent(blocks::get<tags::Tangent>(self()));
            auto& dilatation(blocks::get<tags::ElementDilatation>(self()));
            auto& voronoi_dilatation(
                blocks::get<tags::VoronoiDilatation>(self()));
            auto& internal_stress(blocks::get<tags::InternalStress>(self()));
            auto& internal_couple(blocks::get<tags::InternalCouple>(self()));
            auto& internal_torques(blocks::get<tags::InternalTorques>(self()));
            auto& bend_matrix(
                blocks::get<tags::BendingTwistRigidityMatrix>(self()));
            auto& reference_curvature(
                blocks::get<tags::ReferenceCurvature>(self()));
            auto& reference_voronoi_length(
                blocks::get<tags::ReferenceVoronoiLength>(self()));
            auto& reference_element_length(
                blocks::get<tags::ReferenceElementLength>(self()));
            auto& mass_SMOI(
                blocks::get<tags::MassSecondMomentOfInertia>(self()));

            auto& dummy_elem_vector(
                blocks::get<tags::_DummyElementVector>(self()));
            auto& dummy_elem_vector2(
                blocks::get<tags::_DummyElementVector2>(self()));
            auto& dummy_voro_vector(
                blocks::get<tags::_DummyVoronoiVector>(self()));

            /* Compute Curvature */
            compute_curvature(self());
            self().template fill_ghosts_for<tags::Curvature>();

            Traits::Operations::batch_matvec(internal_couple, bend_matrix,
                                             curvature - reference_curvature);

            /* Compute dilatation rate */
            auto dilatation_rate = self().get_dilatation_rate();

            auto voronoi_dilatation_inv_cube =
                Traits::Operations::inverse_cube(voronoi_dilatation);

            /* Reset internal torque */
            // Caution: Make sure the dummy variable scope does not overlap
            // internal couple should have 0.0
            Traits::Operations::two_point_difference_kernel(
                internal_torques,  // bend_twist_couple_2D_batch,
                internal_couple % Traits::Operations::transpose(
                                      Traits::Operations::expand_for_broadcast(
                                          voronoi_dilatation_inv_cube)));

            // allocating temp container (3, n_voronoi) for temporary storage
            auto& curvature_cross_internal_couple_batch(dummy_voro_vector);
            Traits::Operations::batch_cross(
                curvature_cross_internal_couple_batch, curvature,
                internal_couple);

            // allocating temp container (3, n_elems) for temporary storage
            auto& bend_twist_couple_3D_batch(dummy_elem_vector);
            Traits::Operations::quadrature_kernel(
                bend_twist_couple_3D_batch,
                curvature_cross_internal_couple_batch %
                    Traits::Operations::transpose(
                        Traits::Operations::expand_for_broadcast(
                            reference_voronoi_length *
                            voronoi_dilatation_inv_cube)));
            internal_torques += bend_twist_couple_3D_batch;

            // allocating temp container (3, n_elems) for temporary storage
            auto& Q_into_t_batch(dummy_elem_vector);
            Traits::Operations::batch_matvec(Q_into_t_batch, director, tangent);

            // allocating temp container (3, n_elems) for temporary storage
            auto& Q_into_t_cross_internal_stress_batch(dummy_elem_vector2);
            Traits::Operations::batch_cross(
                Q_into_t_cross_internal_stress_batch, Q_into_t_batch,
                internal_stress);
            auto shear_stretch_couple_batch =
                Q_into_t_cross_internal_stress_batch %
                Traits::Operations::transpose(
                    Traits::Operations::expand_for_broadcast(
                        reference_element_length));
            internal_torques += shear_stretch_couple_batch;

            /* common sub expression elimination here, as J w / e is used in
             * both */
            /* the lagrangian transport and dilatation terms */
            // allocating temp container (3, n_elems) for temporary storage
            auto& J_omega_upon_e_batch(dummy_elem_vector);
            auto& temp_vec(dummy_elem_vector2);
            Traits::Operations::batch_matvec(temp_vec, mass_SMOI, omega);
            Traits::Operations::batch_division_matvec(J_omega_upon_e_batch,
                                                      temp_vec, dilatation);

            // allocating temp container (3, n_elems) for temporary storage
            auto& lagrangian_transport_batch(temp_vec);
            Traits::Operations::batch_cross(lagrangian_transport_batch,
                                            J_omega_upon_e_batch, omega);
            internal_torques += lagrangian_transport_batch;

            // unsteady_dilatation_batch
            // auto dilatation_ratio = dilatation_rate /
            // Traits::Operations::transpose(dilatation);
            auto dilatation_ratio = dilatation_rate / dilatation;
            auto dilatation_ratio_expanded =
                Traits::Operations::expand_for_broadcast(
                    Traits::Operations::transpose(dilatation_ratio));
            auto unsteady_dilatation_batch =
                J_omega_upon_e_batch % dilatation_ratio_expanded;
            internal_torques += unsteady_dilatation_batch;
          }
          //********************************************************************

          //@}
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Elasticity component corresponding to a linear hyper-elastic
       * model with diagonal stress--stain relations
       * \ingroup cosserat_rod_component
       *
       * \details
       * WithDiagonalLinearHyperElasticModel implements the final Elasticity
       * component (with the right interface) for use within Cosserat rods
       * implemented in the Blocks framework. It denotes a hyper-elasic model
       * where the stresses depend on the strains linearly, in a pure diagonal
       * manner (i.e. strains along a principal axis only affect the stresses
       * along the same principal axis, and nothing else), based on the
       * following relations:
       *
       * \f[
       * \boldsymbol{\tau}_{\mathcal{L}} =
       * \bv{B}\left(\boldsymbol{\kappa}_{\mathcal{L}}-\boldsymbol{\kappa}^o_{\mathcal{L}}\right)
       * \f]
       * \f[
       * \boldsymbol{n}_{\mathcal{L}} =
       * \bv{S}\left(\boldsymbol{\sigma}_{\mathcal{L}}-\boldsymbol{\sigma}^o_{\mathcal{L}}\right)
       * \f]
       *
       * \note
       * It requires a valid Geometry component declared in the Blocks
       * Hierarchy to ensure it is properly used. Else, a compilation error is
       * thrown.
       *
       * \usage
       * Since WithDiagonalLinearHyperElasticModel is a valid and complete
       * Elasticity component adhering to protocols::Elastic1D, one can use it
       * to declare a CosseratRodPlugin within the @ref blocks framework
       *
       * \code
       * // pre-declare RodTraits, Blocks
       * using CircularCosseratRod = CosseratRodPlugin<RodTraits, Block,
       * // Conforms to protocols::Geometry1D!
       * components::WithCircularCosseratRod,
       * components::WithDiagonalLinearHyperElasticModel>;
       * \endcode
       *
       * \tparam CRT A valid Cosserat Rod Traits class
       * \tparam ComputationalBlock The final block which is derived from the
       * current component
       *
       * \see CosseratRodPlugin
       */
      template <typename CRT, typename ComputationalBlock>
      class WithDiagonalLinearHyperElasticModel
          : public Adapt<detail::WithDiagonalLinearHyperElasticModel<
                CRT, ComputationalBlock>>::
                template with<detail::ElasticityInterface> {};
      //************************************************************************

      // clang-format off
//******************************************************************************
/*!\brief Documentation stub with tags of  WithDiagonalLinearHyperElasticModel
 * \ingroup cosserat_rod_component
 *
| Elasticity Variables           ||
|--------------------------------|-------------------------------------------------------------------------------|
| On Nodes    (`n_elements+1`)   | elastica::tags::InternalLoads                                                 |
| On Elements (`n_elements`)     | elastica::tags::InternalTorques, elastica::tags::InternalStress               |
|^                               | elastica::tags::ShearStretchRigidityMatrix                                    |
| On Voronoi  (`n_elements - 1`) | elastica::tags::InternalCouple, elastica::tags::BendingTwistRigidityMatrix    |
*/
      template <typename CRT, typename ComputationalBlock>
      using WithDiagonalLinearHyperElasticModelTagsDocsStub =
      WithDiagonalLinearHyperElasticModel<CRT, ComputationalBlock>;
//******************************************************************************
      // clang-format on

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
