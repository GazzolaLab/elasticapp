//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<array\>
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

#include <array>
#include <type_traits>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/TypeTraits.hpp"

#pragma once

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::arraytor
    // \ingroup stl_support
    */
    template <typename T, std::size_t N>
    struct Serialize<std::array<T, N>> {
      using type = std::array<T, N>;

      // specialize for arithmetic types and non-arithmetic types
      // helpful tags for specialization
      struct arithmetic_tag {};
      struct non_arithmetic_tag {};

      using specialized_type =
          std::conditional_t<std::is_arithmetic<T>::value, arithmetic_tag,
                             non_arithmetic_tag>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& array) {
        make_overloader(
            [&](arithmetic_tag /* meta */) {
              ar(binary_data(array.data(), sizeof(type)));
            },
            [&](non_arithmetic_tag /* meta */) {
              for (auto&& v : array)
                ar(v);
            })(specialized_type{});
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& array) {
        make_overloader(
            [&](arithmetic_tag /* meta */) {
              ar(binary_data(array.data(), sizeof(type)));
            },
            [&](non_arithmetic_tag /* meta */) {
              for (auto&& v : array)
                ar(v);
            })(specialized_type{});
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
