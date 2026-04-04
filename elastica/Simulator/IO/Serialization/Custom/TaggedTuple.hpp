//==============================================================================
/*!
//  \file
//  \brief Serialization support for types found in \<TaggedTuple.h\>
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

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for TaggedTuple
    // \ingroup custom_serialize
    */
    template <class... Tags>
    struct Serialize<tuples::TaggedTuple<Tags...>> {
      using type = tuples::TaggedTuple<Tags...>;
      /* static constexpr std::size_t size = sizeof...(Types); */

      template <typename InArchive>
      static inline void load(InArchive& ar, type& tagged_tuple) {
        tmpl::for_each<tmpl::list<Tags...>>([&](auto v) {
          using Tag = tmpl::type_from<decltype(v)>;
          auto name{pretty_type::name<Tag>()};
          try {
            ar(make_nvp<InArchive>(name.c_str(),
                                   tuples::get<Tag>(tagged_tuple)));
          } catch (const std::runtime_error& e) {
            std::throw_with_nested(std::runtime_error(
                "Error while loading data tagged with " + name));
          }
        });
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& tagged_tuple) {
        tmpl::for_each<tmpl::list<Tags...>>([&](auto v) {
          using Tag = tmpl::type_from<decltype(v)>;
          auto name{pretty_type::name<Tag>()};
          try {
            ar(make_nvp<OutArchive>(name.c_str(),
                                    tuples::get<Tag>(tagged_tuple)));
          } catch (const std::runtime_error& e) {
            std::throw_with_nested(std::runtime_error(
                "Error while saving data tagged with " + name));
          }
        });
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
