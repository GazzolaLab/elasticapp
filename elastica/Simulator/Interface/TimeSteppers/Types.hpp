//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Time-stepper module
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

  namespace interface {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of interface types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <class SimTraits, class Sim>
    class InterfaceWithSymplecticSteppers;

    template <class SimTraits, class Sim>
    class InterfaceWithExplicitSteppers;

    struct AlgorithmMetadata;
    /*! \endcond */
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica
