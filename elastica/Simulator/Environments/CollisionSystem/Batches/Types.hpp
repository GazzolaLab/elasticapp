//==============================================================================
/*!
//  \file
//  \brief Types of batch generators
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

#include "Simulator/Environments/CollisionSystem/Batches/UnionFindExecutionPolicy/Types.hpp"

namespace elastica {

  namespace collision {

    namespace batches {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of elastica::collision::batches types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename>
      class SingleBatch;
      template <typename>
      class UnionFind;
      template <typename>
      class HybridBatch;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename C>
      class ContactTrait;
      template <typename C>  // Type of the configuration
      class RigidBodyPrimitiveTrait;
      /*! \endcond */
      //************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
