//==============================================================================
/*!
//  \file
//  \brief Types of coarse detectors
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        ////////////////////////////////////////////////////////////////////////
        //
        // Forward declarations of elastica::collision::detection::coarse types
        //
        ////////////////////////////////////////////////////////////////////////

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        template <typename>
        class ExhaustiveSearch;
        template <typename>
        class HashGrids;
        //        template <typename>
        //        class SweepAndPrune;
        template <typename C>
        class RigidBodyPrimitiveTrait;
        template <typename C>
        class ContactTrait;
        /*! \endcond */
        //**********************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
