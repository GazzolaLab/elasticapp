//==============================================================================
/*!
//  \file
//  \brief Serialization support for a UUID
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

/// types to serialize
#include "Simulator/Interface/Wrappers/UUID.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a UUID
     * \ingroup interface
     */
    template <typename ParameterTag>
    struct Serialize<::elastica::interface::detail::UUID<ParameterTag>> {
      using type = ::elastica::interface::detail::UUID<ParameterTag>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& uuid) {
        const auto uuid_from_config = uuid;
        ar(uuid.uuid);
        ar(uuid.name);

        if (uuid_from_config != uuid) {
          auto pretty_print = [](type const& oo_oo_id) {
            using ::elastica::interface::detail::id;
            using ::elastica::interface::detail::name;
            return MakeString{} << "(" << id(oo_oo_id) << ", " << name(oo_oo_id)
                                << ")";
          };
          throw std::runtime_error(std::string(
              MakeString{} << "Restored UUID : " << pretty_print(uuid_from_config)
                           << " does not match the"
                           << " saved UUID : " << pretty_print(uuid)));
        }
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& uuid) {
        using ::elastica::interface::detail::id;
        using ::elastica::interface::detail::name;
        ar(id(uuid));
        ar(name(uuid));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica