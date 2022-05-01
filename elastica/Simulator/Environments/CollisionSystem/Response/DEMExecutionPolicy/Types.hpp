//==============================================================================
/*!
//  \file
//  \brief Types of execution policy for collision response based on
// Discrete Element Method (DEM)
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

    namespace response {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of helper types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename F, typename Policy>
      class ContactResolver;
      /*! \endcond */
      //************************************************************************

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of operational types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // the default template parameter may need revisiting at some later point
      // in time
      template <typename C,
                typename ParallelPolicy =
                    typename C::KernelExecutionPolicy::ExecutionPolicy>
      class DEMSolverOp;
      /*! \endcond */
      //************************************************************************

    }  // namespace response

  }  // namespace collision

}  // namespace elastica