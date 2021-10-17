//
// Created by Tejaswin Parthasarathy on 10/13/21.
//

#pragma once

#include "ModuleSettings/Profiling.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Timing.h"

namespace elastica {

  namespace interface {

    namespace tags {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename Step>
      struct WcProfilerTag {
        using type = ::elastica::timing::WcTimer;

        static std::string log_name() noexcept {
          return pretty_type::short_name<Step>();
        }
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace tags

  }  // namespace interface

}  // namespace elastica