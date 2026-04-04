//==============================================================================
/*!
//  \file
//  \brief Types of symplectic time-stepping algorithms
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
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
