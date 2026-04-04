//==============================================================================
/*!
//  \file
//  \brief Serialization support for common policy types
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
#include <cstddef>
#include <stdexcept>

/// Types
#include "Simulator/Configuration/ModuleConfiguration/common/common.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace detail {

    template <typename OutArchive, typename Policy>
    void load_impl(
        OutArchive& ar,
        ::elastica::configuration::SingleSizePolicy<Policy>& policy) {
      const auto value_from_config = policy.value();
      std::size_t value_from_file(0UL);
      ar(value_from_file);

      if (value_from_file != value_from_config) {
        throw std::runtime_error(
            std::string(
                MakeString{}
                << "Value from the configuration file : " << value_from_config
                << " does not match the value from the archive saved prior : "
                << value_from_file));
      }
    }

    template <typename OutArchive, typename Policy>
    void save_impl(
        OutArchive& ar,
        ::elastica::configuration::SingleSizePolicy<Policy> const& policy) {
      ar(policy.value());
    }

  }  // namespace detail

}  // namespace elastica

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a CompositeCriteria
     * \ingroup config
     */
    template <class... Criteria>
    struct Serialize<
        ::elastica::configuration::CompositeCriteria<Criteria...>> {
      using type = ::elastica::configuration::CompositeCriteria<Criteria...>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& composite) {
        ar(static_cast<Criteria&>(composite)...);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& composite) {
        ar(static_cast<Criteria const&>(composite)...);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
