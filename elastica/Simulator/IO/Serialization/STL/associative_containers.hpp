#pragma once

#include <cstdint>
#include <utility>

#include "Simulator/IO/Serialization/Helpers.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    //! Save a map-like type
    //! \ingroup serialization
    template <typename OutArchive, template <typename...> class Map,
              typename... Args, typename = typename Map<Args...>::mapped_type>
    void save_map(OutArchive& ar, Map<Args...> const& map) {
      ar(make_size_tag(static_cast<std::size_t>(map.size())));

      for (const auto& i : map)
        ar(make_map_item(i.first, i.second));
    }
    //**************************************************************************

    //**************************************************************************
    //! Load a map-like type
    //! \ingroup serialization
    template <typename OutArchive, template <typename...> class Map,
              typename... Args, typename = typename Map<Args...>::mapped_type>
    void load_map(OutArchive& ar, Map<Args...>& map) {
      std::size_t size;
      ar(make_size_tag(size));

      map.clear();

      auto hint = map.begin();
      for (std::size_t i = 0; i < size; ++i) {
        typename Map<Args...>::key_type key;
        typename Map<Args...>::mapped_type value;

        ar(make_map_item(key, value));
        hint = map.emplace_hint(hint, std::move(key), std::move(value));
      }
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
