//==============================================================================
/*!
//  \file
//  \brief Serialization support for a BlockFacade
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

#include <exception>

#include "Simulator/IO/Serialization.hpp" // all custom, stl and so on
//
#include "Systems/Block/Block/BlockFacade.hpp"
#include "Systems/Block/Block/Types.hpp"
//
#include "Utilities/PrettyType.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a BlockFacade
    // \ingroup blocks
    */
    template <class Plugin>
    struct Serialize<blocks::BlockFacade<Plugin>> {
      using type = blocks::BlockFacade<Plugin>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& block) {
        try {
          ar(block.data());
        } catch (const std::runtime_error& e) {
          std::throw_with_nested(std::runtime_error(
              "Error while loading block data associated with " +
              pretty_type::short_name<Plugin>()));
        }
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& block) {
        try {
          ar(block.data());
        } catch (const std::runtime_error& e) {
          std::throw_with_nested(std::runtime_error(
              "Error while saving block data associated with " +
              pretty_type::short_name<Plugin>()));
        }
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
