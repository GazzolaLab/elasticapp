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
#include "Systems/CosseratRods/Components/Elasticity/Types.hpp"
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
              public Component<
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
          inline auto compute_internal_modeled_loads_impl() const noexcept {
            // std::cout << "Adding hyperelastic forces";
            // Elasticity interface function
            //        self().compute_shear_strains();

            // return E * A * strain;
            // cosserat_rod::batch_matvec(Stiffness, strain);
            // cosserat_rod::batch_matvec(self().get_stiffness(),
            // self().get_strain());
            // cosserat_rod::batch_matvec(self().get_stiffness(),
            // self().compute_cross_section());
            //          Traits::Operations::add(self().get_stiffness(),
            //                                  self().compute_cross_section());
            return 1.0;
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Computes the modeled internal torques from the elasticity
           * model
           */
          inline auto compute_internal_modeled_torques_impl() const noexcept {
            // geomer interface function
            // std::cout << "Adding hyperelastic torques";
            //        self().compute_shear_strains();
            return 2.0;
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

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica