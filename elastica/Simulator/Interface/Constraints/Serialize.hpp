//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Constraints module
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

#include <algorithm>

///
#include "Simulator/Interface/Constraints/Types.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Simulator/Interface/Constraints/ConstrainerBase.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
#include "Simulator/Interface/Constraints/Module.hpp"
#include "Simulator/Interface/Wrappers/Serialize.hpp"
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a ConstraintUUID
     * \ingroup interface
     */
    template <>
    struct Serialize<::elastica::interface::constraints::ConstraintUUID> {
      using type = ::elastica::interface::constraints::ConstraintUUID;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& uuid) {
        ar(static_cast<typename type::Parent&>(uuid));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& uuid) {
        ar(static_cast<typename type::Parent const&>(uuid));
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a ConstrainerBase
     * \ingroup interface
     */
    template <>
    struct Serialize<::elastica::interface::constraints::ConstrainerBase> {
      using type = ::elastica::interface::constraints::ConstrainerBase;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& constrainer) {
        ar(constrainer.id());
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& constrainer) {
        ar(constrainer.id());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a modules::Constraints
     * \ingroup interface
     */
    template <class SimTraits, class Sim>
    struct Serialize<::elastica::modules::Constraints<SimTraits, Sim>> {
      using type = ::elastica::modules::Constraints<SimTraits, Sim>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& constraints_module) {
        const auto n_constraints_from_config =
            constraints_module.n_constraints();
        std::size_t n_constraints_from_file(0UL);
        ar(n_constraints_from_file);

        if (n_constraints_from_file != n_constraints_from_config) {
          throw std::runtime_error(
              std::string(
                  MakeString{}
                  << "Number of constraints from the configuration file : "
                  << n_constraints_from_config
                  << " which does not match the number of constraints from the "
                     "earlier saved archive : "
                  << n_constraints_from_file));
        }

        ar(constraints_module.batch_map());
        const std::size_t map_size = constraints_module.batch_map().size();
        if (map_size != n_constraints_from_file) {
          throw std::runtime_error(
              std::string(
                  MakeString{}
                  << "Number of constraints from the saved archive : "
                  << n_constraints_from_file
                  << " does not match the number of constraints from the "
                     "obtained from the mapping : "
                  << map_size));
        }

        // instead of de-serializing the vector of unique pointers, we call
        // for each, because the memory for the unique pointers is already
        // allocated
        // this can also throw if the ID does not match
        for (auto& batch : constraints_module.constrainer_batches())
          for (auto& constrainer_ptr : batch)
            ar(*constrainer_ptr);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& constraints_module) {
        ar(constraints_module.n_constraints());
        ar(constraints_module.batch_map());

        for (auto& batch : constraints_module.constrainer_batches())
          for (auto& constrainer_ptr : batch)
            ar(*constrainer_ptr);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
