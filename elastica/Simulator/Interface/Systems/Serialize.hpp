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
        const auto n_systems_from_config = systems_module.n_total_systems();
        std::remove_const_t<decltype(n_systems_from_config)>
            n_systems_from_file{};
        ar(n_systems_from_file);

        for (std::size_t index = 0UL;
             index < systems_module.get_number_of_system_collections();
             ++index) {
          if (n_systems_from_file[index] != n_systems_from_config[index]) {
            throw std::runtime_error(std::string(
                MakeString{}
                << "Number of systems from the configuration file : "
                << n_systems_from_config[index]
                << " does not match the number of systems from the "
                   "earlier saved archive : "
                << n_systems_from_file[index] << " for the index " << index));
          }
        }

        // 1. save the policies blocking first
        ar(*systems_module.blocking_policy());
        ar(systems_module.iteration_policy());
        // 2. save systems data
        ar(systems_module.get_system_collection_container());
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& systems_module) {
        // 0. save the n_systems first
        ar(systems_module.n_total_systems());
        // 1. save the policies blocking first
        ar(*systems_module.blocking_policy());
        ar(systems_module.iteration_policy());
        // 2. save systems data
        ar(systems_module.get_system_collection_container());
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica