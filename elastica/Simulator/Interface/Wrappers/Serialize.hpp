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
#include "Simulator/Interface/Wrappers/UUIDMap.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Utilities/MakeString.hpp"
#include "Utilities/AsConst.hpp"

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
          throw std::runtime_error(
              std::string(MakeString{} << "Restored UUID : " << uuid_from_config
                                       << " does not match the"
                                       << " saved UUID : " << uuid));
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

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a UUID Map
     * \ingroup interface
     */
    template <typename UID>
    struct Serialize<::elastica::interface::detail::UUIDToBatch<UID>> {
      using type = ::elastica::interface::detail::UUIDToBatch<UID>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& uuid_map) {
        using L = decltype(cpp17::as_const(uuid_map).as_vectors());
        L loader;
        ar(loader);
        uuid_map.from_vectors(std::move(loader));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& uuid_map) {
        // If you directly save the data, it causes heartbreak
        /*
         * Loading UUIDToBatch failed :
         * Loading unordered_map failed :
         * Loading ForcingUUID failed :
         * Loading UUID failed :
         * Restored UUID : (18446744073709551615, ) does not match the saved
         * UUID : (0, )
         *
         * Because UUID types have a check while restoring. Hence we use
         * special methods here
         */
        ar(uuid_map.as_vectors());
        // ar(uuid_map.data());
      }
    };

  }  // namespace io

}  // namespace elastica