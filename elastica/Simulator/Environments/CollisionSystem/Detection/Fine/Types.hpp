//==============================================================================
/*!
//  \file
//  \brief Types of fine detectors
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        ////////////////////////////////////////////////////////////////////////
        //
        // Forward declarations of elastica::collision::detection::fine types
        //
        ////////////////////////////////////////////////////////////////////////

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        class MaxContacts;
        class EPA;
        class GJK;
        template <typename C>
        class FineDetectionInterface;
        template <typename C>
        class RigidBodyPrimitiveTrait;
        template <typename C>
        class ContactTrait;
        /*! \endcond */
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
