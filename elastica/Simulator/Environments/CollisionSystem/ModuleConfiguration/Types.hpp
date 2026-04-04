//==============================================================================
/*!
//  \file
//  \brief Types of configuration for the collision system in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace configuration {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of configuration types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename ForEachExecutionPolicy_,
              typename OverPairsExecutionPolicy_, typename ForceModel_>
    class CollisionSystemUserConfiguration;
    /*! \endcond */
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica