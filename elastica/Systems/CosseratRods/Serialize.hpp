//==============================================================================
/*!
//  \file
//  \brief Serialization support for a CosseratRodBlock
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

#include <exception>

#include "Simulator/IO/Serialization.hpp"  // all custom serialization
#include "Systems/Block/Serialize.hpp"     // for facade
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Types.hpp"
#include "Utilities/PrettyType.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for TaggedTuple
     *  \ingroup cosserat_rod
     *
     * \tparam CRT        The traits class for a Cosserat rod
     * \tparam Components Components customizing a Cosserat rod (such as
     * geometry, elasticity)
     */
    template <typename CRT,  // Cosserat Rod Traits
              template <typename /*CRT*/, typename /*InitializedBlock*/>
              class... Components>
    struct Serialize<
        ::blocks::Block<::elastica::cosserat_rod::CosseratRodPlugin<
            CRT, ::blocks::Block, Components...>>> {
      using type = ::blocks::Block<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ::blocks::Block, Components...>>;
      using Facade = typename type::Parent;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& block) {
        try {
          ar(static_cast<Facade&>(block));
          ar(block.get_ghost_node_buffer());
        } catch (const std::runtime_error& e) {
          std::throw_with_nested(
              std::runtime_error("Error while loading Cosserat Rod data"));
        }
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& block) {
        try {
          ar(static_cast<Facade const&>(block));
          ar(block.get_ghost_node_buffer());
        } catch (const std::runtime_error& e) {
          std::throw_with_nested(
              std::runtime_error("Error while saving Cosserat Rod data"));
        }
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
