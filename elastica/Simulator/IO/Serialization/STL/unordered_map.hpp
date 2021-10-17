//
// Created by Tejaswin Parthasarathy on 10/11/21.
//

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <type_traits>
#include <unordered_map>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::arraytor
    // \ingroup stl_support
    */
    template <typename... Args>
    struct Serialize<std::unordered_map<Args...>> {
      using type = std::unordered_map<Args...>;

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& map) {
        ar(make_size_tag(static_cast<std::size_t>(map.size())));

        for (const auto& i : map)
          ar(i.first, i.second);
      }

      template <typename OutArchive>
      static inline void load(OutArchive& ar, type& map) {
        std::size_t size;
        ar(make_size_tag(size));

        map.clear();

        auto hint = map.begin();
        for (std::size_t i = 0; i < size; ++i) {
          typename type::key_type key;
          typename type::mapped_type value;

          ar(key, value);
          hint = map.emplace_hint(hint, std::move(key), std::move(value));
        }
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica