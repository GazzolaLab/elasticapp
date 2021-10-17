//
// Created by Tejaswin Parthasarathy on 10/15/21.
//

#pragma once

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

      struct DefaultBatcher{
        using type = DefaultBatchingOperation;
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica