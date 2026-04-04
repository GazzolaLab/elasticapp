//==============================================================================
/*!
//  \file
//  \brief An empty batcher for quick assembly of \elastica Simulator
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cstddef>

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"

namespace elastica {

  namespace interface {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      struct DefaultBatchingOperation {
        template <typename Traits>
        DefaultBatchingOperation(Traits const&) {}
      };

      struct DefaultBatcher {
        using type = DefaultBatchingOperation;
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica
