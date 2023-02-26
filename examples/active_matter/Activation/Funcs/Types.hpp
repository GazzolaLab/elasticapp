//==============================================================================
/*!
//  \file
//  \brief Types belonging to Activation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "InterpolatingSpline.hpp"
#include "Sinusoid.hpp"

namespace elastica {

  namespace forcing {

    namespace activation {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of activation types
      //
      //////////////////////////////////////////////////////////////////////////

      //**All other types*******************************************************
      /*! \cond ELASTICA_INTERNAL */
      class Sinusoid;
      class InterpolatingSpline;
      template <typename Op, typename... F>
      class Compose;
      template <typename F>
      struct SpatialActivationAdapter;
      template <typename F>
      struct TemporalActivationAdapter;
      /*! \endcond */
      //************************************************************************

    }  // namespace activation

  }    // namespace forcing

}  // namespace elastica
