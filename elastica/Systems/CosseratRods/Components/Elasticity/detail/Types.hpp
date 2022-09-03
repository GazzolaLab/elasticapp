//==============================================================================
/*!
//  \file
//  \brief Helper types belonging to the Elasticity component of Cosserat Rods
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
#include "Systems/CosseratRods/Components/Elasticity/detail/Tags.hpp"
//
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      namespace detail {

        ////////////////////////////////////////////////////////////////////////
        //
        // Forward declarations of helper elasticity components
        //
        ////////////////////////////////////////////////////////////////////////

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        template <typename /* Rod Traits */, typename /*Block*/,
                  typename /* ElasticityModel */>
        class ElasticityInterface;

        template <typename /* Rod Traits */, typename /*Block*/>
        class LinearHyperElasticityFacade;

        template <typename /* Rod Traits */, typename /* Block */,
                  class /*ElasticityModelWithBlock*/,
                  template <typename /* Rod Traits */> class /*VariableInfo*/
                  >
        class ExplicitDampingAdapterFacade;

        template <typename /* Rod Traits */, typename /*Block*/,
                  class /*ElasticityModel*/>
        class ExplicitDampingAdapter;

        template <typename /* Rod Traits */, typename /*Block*/,
                  class /*ElasticityModel*/>
        class ExplicitDampingAdapterPerRod;

        /*! \endcond */
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of functions
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename Traits, typename ComputationalBlock>
    auto compute_effective_area_along_principal_directions(
        component::detail::LinearHyperElasticityFacade<
            Traits, ComputationalBlock>& block_like,
        std::size_t i) noexcept -> Vec3;
    /*! \endcond */
    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica
