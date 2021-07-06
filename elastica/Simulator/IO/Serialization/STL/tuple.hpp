//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<tuple\>
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

// Reused from cereal : https://uscilab.github.io/cereal/index.html
// Distributed under the BSD License.
// See https://github.com/USCiLab/cereal/blob/master/LICENSE for details.

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <tuple>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::tuple
    // \ingroup stl_support
    */
    template <class... Types>
    struct Serialize<std::tuple<Types...>> {
      using type = std::tuple<Types...>;
      /* static constexpr std::size_t size = sizeof...(Types); */

      template <typename InArchive>
      static inline void load(InArchive& ar, type& tuple) {
        /* works, but non-idomatic
                index_apply<size>([&](auto... Is) {
                  EXPAND_PACK_LEFT_TO_RIGHT(ar(std::get<Is>(tuple)));
                });
        */
        cpp17::apply([&](Types&... tuple_args) { ar(tuple_args...); }, tuple);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& tuple) {
        /* works, but non-idomatic
        index_apply<size>([&](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(ar(std::get<Is>(tuple)));
        });
        */

        cpp17::apply([&](Types const&... tuple_args) { ar(tuple_args...); },
                     tuple);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
