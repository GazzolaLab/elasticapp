//==============================================================================
/*!
//  \file
//  \brief Geometry component implementing a Square cross section Cosserat rod
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

#include "Systems/CosseratRods/Components/Geometry/Protocols.hpp"
#include "Systems/CosseratRods/Components/Geometry/Types.hpp"
///
#include "Systems/CosseratRods/Components/Geometry/detail/CosseratRodCrossSectionInterface.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/SquareCrossSectionOperations.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/ProtocolHelpers.hpp"
// Implementation details

namespace elastica {
  //  public RodKinematics<RodTraits>
  // Sparse_strategy
  // Dense strategy
  namespace cosserat_rod {

    namespace component {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Geometry component corresponding to a spanwise rod with a
       * square cross section
       * \ingroup cosserat_rod_component
       *
       * \details
       * WithSquareCosseratRod implements the final Geometry component use
       * within Cosserat rods implemented in the Blocks framework. It denotes a
       * Cosserat rod data-structure with a square cross section in the
       * lateral direction and spanning a single spatial dimension along the
       * transverse dimension.
       *
       * \usage
       * Since WithSquareCosseratRod is a valid and complete Geometry component
       * adhering to protocols::Geometry1D, one can use it to declare a
       * CosseratRodPlugin within the @ref blocks framework
       * \code
       * // pre-declare RodTraits, Blocks
       * using CircularCosseratRod = CosseratRodPlugin<RodTraits, Block,
       * components::WithSquareCosseratRod>
       * \endcode
       *
       * \tparam CRT A valid Cosserat Rod Traits class
       * \tparam ComputationalBlock The final block which is derived from the
       * current component
       *
       * \see CosseratRodPlugin
       */
      template <typename CRT, typename ComputationalBlock>
      class WithSquareCosseratRod
          : public detail::CosseratRodCrossSectionInterface<
                CRT, ComputationalBlock,
                detail::SquareCrossSectionOperations<CRT>>,
            public ::tt::ConformsTo<protocols::Geometry1D>,
            public GeometryComponent<
                WithSquareCosseratRod<CRT, ComputationalBlock>> {
       private:
        //**Type definitions****************************************************
        //! Traits type
        using Traits = CRT;
        //! Cross section type
        using CrossSection = detail::SquareCrossSectionOperations<Traits>;
        //! This type
        using This = WithSquareCosseratRod<Traits, ComputationalBlock>;
        //! Parent type
        using Parent =
            detail::CosseratRodCrossSectionInterface<Traits, ComputationalBlock,
                                                     CrossSection>;
        //**********************************************************************

        //! sanity check cross section classes
        static_assert(
            cpp17::is_same_v<typename Parent::CrossSection, CrossSection>,
            "Oops!");

       protected:
        //**Parent methods and aliases******************************************
        //! Initialize method inherited from parent class
        using Parent::initialize;
        //! List of computed variables
        using typename Parent::ComputedVariables;
        //! List of initialized variables
        using typename Parent::InitializedVariables;
        //! List of all variables
        using typename Parent::Variables;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
