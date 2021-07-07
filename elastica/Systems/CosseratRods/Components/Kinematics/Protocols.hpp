//==============================================================================
/*!
//  \file
//  \brief Protocols for kinematics component within @ref cosserat_rod_component
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

#include <type_traits>
#include <utility>

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      // This is useful in unit-testing
      namespace protocols {


        struct Kinematics1D {
          // todo
        };

      }

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica