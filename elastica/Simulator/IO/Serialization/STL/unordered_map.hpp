#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <unordered_map>

#include "Simulator/IO/Serialization/STL/associative_containers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::unordered_map
    // \ingroup stl_support
    */
    template <typename... Args>
    struct Serialize<std::unordered_map<Args...>> {
      using type = std::unordered_map<Args...>;

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& map) {
        save_map(ar, map);
      }

      template <typename OutArchive>
      static inline void load(OutArchive& ar, type& map) {
        load_map(ar, map);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica