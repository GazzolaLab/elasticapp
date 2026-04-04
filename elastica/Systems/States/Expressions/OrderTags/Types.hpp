//==============================================================================
/*!
//  \file
//  \brief Declaration of tags indicating derivative orders of temporal states
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace states {

    namespace tags {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of order tags
      //
      //////////////////////////////////////////////////////////////////////////

      // NOTE : One can use an integral constant as exponent here, but for nicer
      //        error messages we stick with Tag types
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      struct PrimitiveTag;
      struct DerivativeTag;
      struct DoubleDerivativeTag;
      /*! \endcond */
      //************************************************************************

      // Don't expose to the user
      namespace internal{

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        struct DerivativeMultipliedByTimeTag;
        struct DoubleDerivativeMultipliedByTimeTag;
        /*! \endcond */
        //**********************************************************************

      }

    }  // namespace tags

  }  // namespace states

}  // namespace elastica
