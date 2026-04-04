//==============================================================================
/*!
//  \file
//  \brief Types in the collision policy submodule
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

    namespace detail {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of elastica::collision::batches types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename>
      class ForEachOp;
      template <typename>
      class ForEachTypeOp;
      template <typename>
      class OverPairsOp;
      /*! \endcond */
      //************************************************************************
    }  // namespace detail

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename, typename>
    class KernelExecutionPolicy;
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica