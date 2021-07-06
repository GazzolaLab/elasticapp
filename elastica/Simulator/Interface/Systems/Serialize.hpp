//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Systems module
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

///
#include "Simulator/Interface/Systems/Types.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Simulator/Interface/Systems/Module.hpp"
/// policy serialization
#include "Simulator/Configuration/ModuleConfiguration/Serialize.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for the Simulator
     * \ingroup interface
     */
    template <class SimTraits, class Sim>
    struct Serialize<::elastica::modules::Systems<SimTraits, Sim>> {
      using type = ::elastica::modules::Systems<SimTraits, Sim>;
      // 1. check if blocking is the same, else abort
      template <typename InArchive>
      static inline void load(InArchive& ar, type& systems_module) {
        // 1. save the policies blocking first
        ar(systems_module.blocking_policy(), systems_module.iteration_policy());
        // 2. save systems data
        ar(systems_module.get_system_collection_storage());
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& systems_module) {
        // 1. save the policies blocking first
        ar(systems_module.blocking_policy(), systems_module.iteration_policy());
        // 2. save systems data
        ar(systems_module.get_system_collection_storage());
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica