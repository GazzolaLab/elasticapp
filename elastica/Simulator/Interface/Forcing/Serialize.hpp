//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Forcing module
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

#include <algorithm>

///
#include "Simulator/Interface/Forcing/Types.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Simulator/Interface/Forcing/ForcerBase.hpp"
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"
#include "Simulator/Interface/Forcing/Module.hpp"
#include "Simulator/Interface/Wrappers/Serialize.hpp"
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a ForcingUUID
     * \ingroup interface
     */
    template <>
    struct Serialize<::elastica::interface::forcing::ForcingUUID> {
      using type = ::elastica::interface::forcing::ForcingUUID;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& uuid) {
        ar(static_cast<typename type::Parent&>(uuid));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& uuid) {
        ar(static_cast<typename type::Parent const&>(uuid));
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a ForcerBase
     * \ingroup interface
     */
    template <>
    struct Serialize<::elastica::interface::forcing::ForcerBase> {
      using type = ::elastica::interface::forcing::ForcerBase;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& forcer) {
        ar(forcer.id());
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& forcer) {
        ar(forcer.id());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a modules::Forcing
     * \ingroup interface
     */
    template <class SimTraits, class Sim>
    struct Serialize<::elastica::modules::Forcing<SimTraits, Sim>> {
      using type = ::elastica::modules::Forcing<SimTraits, Sim>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& forcings_module) {
        const auto n_forcings_from_config = forcings_module.n_forcings();
        std::size_t n_forcings_from_file(0UL);
        ar(n_forcings_from_file);

        if (n_forcings_from_file != n_forcings_from_config) {
          throw std::runtime_error(
              std::string(MakeString{}
                          << "Number of forcings from the configuration file : "
                          << n_forcings_from_config
                          << " does not match the number of forcings from the "
                             "earlier saved archive : "
                          << n_forcings_from_file));
        }

        ar(forcings_module.batch_map());
        const std::size_t map_size = forcings_module.batch_map().size();
        if (map_size != n_forcings_from_file) {
          throw std::runtime_error(std::string(
              MakeString{} << "Number of forcings from the saved archive : "
                           << n_forcings_from_file
                           << " does not match the number of forcings from the "
                              "obtained from the mapping : "
                           << map_size));
        }

        // instead of de-serializing the vector of unique pointers, we call
        // for each, because the memory for the unique pointers is already
        // allocated

        // this can also throw if the ID does not match
        for (auto& batch : forcings_module.forcer_batches())
          for (auto& forcer_ptr : batch)
            ar(*forcer_ptr);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& forcings_module) {
        ar(forcings_module.n_forcings());
        ar(forcings_module.batch_map());

        for (auto const& batch : forcings_module.forcer_batches())
          for (auto const& forcer_ptr : batch)
            ar(*forcer_ptr);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
