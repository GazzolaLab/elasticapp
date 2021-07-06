//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Connections module
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
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Simulator/Interface/Connections/ConnecterBase.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"
#include "Simulator/Interface/Connections/Module.hpp"
#include "Simulator/Interface/Wrappers/Serialize.hpp"
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a ConnectionUUID
     * \ingroup interface
     */
    template <>
    struct Serialize<::elastica::interface::connections::ConnectionUUID> {
      using type = ::elastica::interface::connections::ConnectionUUID;

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
    /*!\brief Specialization of Serialize template for a ConnecterBase
     * \ingroup interface
     */
    template <>
    struct Serialize<::elastica::interface::connections::ConnecterBase> {
      using type = ::elastica::interface::connections::ConnecterBase;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& connecter) {
        ar(connecter.id());
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& connecter) {
        ar(connecter.id());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a modules::Connections
     * \ingroup interface
     */
    template <class SimTraits, class Sim>
    struct Serialize<::elastica::modules::Connections<SimTraits, Sim>> {
      using type = ::elastica::modules::Connections<SimTraits, Sim>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& connections_module) {
        const auto n_connections_from_config =
            connections_module.n_connections();
        std::size_t n_connections_from_file(0UL);
        ar(n_connections_from_file);

        if (n_connections_from_file != n_connections_from_config) {
          throw std::runtime_error(
              std::string(
                  MakeString{}
                  << "Number of connections from the configuration file : "
                  << n_connections_from_config
                  << " which does not match the number of connections from the "
                     "earlier saved archive : "
                  << n_connections_from_file));
        }

        // instead of de-serializing the vector of unique pointers, we call
        // for each, because the memory for the unique pointers is already
        // allocated

        // this can also throw if the ID does not match
        auto const& connecters = connections_module.connecters();
        std::for_each(connecters.begin(), connecters.end(),
                      [&](auto const& connecter_ptr) { ar(*connecter_ptr); });
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& connections_module) {
        ar(connections_module.n_connections());
        auto const& connecters = connections_module.connecters();
        std::for_each(connecters.cbegin(), connecters.cend(),
                      [&](auto const& connecter_ptr) { ar(*connecter_ptr); });
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
