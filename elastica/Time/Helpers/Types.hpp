//==============================================================================
/*!
//  \file
//  \brief Helper types
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

  namespace timestepper {

    namespace detail {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of helper types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename D>
      struct StateUpdate;
      template <typename D>
      struct KinematicUpdate;
      template <typename D>
      struct DynamicUpdate;
      template <typename D>
      struct TimeUpdate;
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace timestepper

}  // namespace elastica