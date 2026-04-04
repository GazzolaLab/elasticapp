//==============================================================================
/*!
//  \file
//  \brief Sequential container policy
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
#include <vector>
//
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/Types.hpp"
//

namespace elastica {

  namespace collision {

    namespace detail {

      //==========================================================================
      //
      //  CLASS DEFINITION
      //
      //==========================================================================

      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of ContactContainerPolicy for sequential policy
       * \ingroup collision_system
       */
      template <>
      struct ContactContainerPolicy<
          ::elastica::configuration::sequential_policy> {
        template <typename C>
        using type = std::vector<C>;
      };
      /*! \endcond */
      //**************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
