//==============================================================================
/*!
//  \file
//  \brief Types in the collision policy submodule
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