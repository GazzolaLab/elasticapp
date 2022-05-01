//==============================================================================
/*!
//  \file
//  \brief Sequential container policy
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
#include <functional>
#include <utility>
#include <vector>

//
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/ContactCache/ContainerPolicy/Types.hpp"
//
#include "Utilities/TMPL.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of PairwiseCacheContainerPolicy for sequential policy
   * \ingroup collision_system
   */
  template <>
  struct PairwiseCacheContainerPolicy<
      ::elastica::configuration::sequential_policy> {
    template <typename T>
    struct apply;

    template <typename T1, typename T2>
    struct apply<tmpl::pair<T1, T2>> {
      // ugly, I don't know how to use defer yet
      using type = std::vector<
          std::pair<std::reference_wrapper<T1>, std::reference_wrapper<T2>>>;
    };
  };
  /*! \endcond */
  //****************************************************************************

}  // namespace elastica