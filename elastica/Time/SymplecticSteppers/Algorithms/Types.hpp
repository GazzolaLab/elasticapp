//==============================================================================
/*!
//  \file
//  \brief Types of symplectic time-stepping algorithms
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

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of symplectic time-stepping algorithms
    //
    ////////////////////////////////////////////////////////////////////////////

    namespace symplectic {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      struct PositionVerlet;
      struct VelocityVerlet;
      struct PEFRL;
      /*! \endcond */
      //************************************************************************

    }  // namespace symplectic

  }  // namespace timestepper

}  // namespace elastica
