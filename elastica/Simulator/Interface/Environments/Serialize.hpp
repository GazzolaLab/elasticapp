//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Environments module
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
#include <stdexcept>  // runtime_error
#include <string>
#include <type_traits>  // remove_const

///
#include "Simulator/Interface/Environments/Types.hpp"
///
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Simulator/Interface/Environments/Module.hpp"
#include "Simulator/Interface/Environments/WithImposedPrimitives.hpp"
#include "Simulator/Interface/Environments/WithPrimitives.hpp"
//
#include "Simulator/Environments/Primitives/Serialize.hpp"
//
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for the ImposedSystems
     * \ingroup interface
     */
    template <class SimTraits, class Sim>
    struct Serialize<::elastica::interface::environments::WithImposedPrimitives<
        SimTraits, Sim>> {
      using type =
          ::elastica::interface::environments::WithImposedPrimitives<SimTraits,
                                                                     Sim>;
      template <typename InArchive>
      static inline void load(InArchive& ar, type& imposed_primitives_module) {
        const auto n_imposed_primitives_from_config =
            imposed_primitives_module.n_imposed_primitives();
        std::remove_const_t<decltype(n_imposed_primitives_from_config)>
            n_imposed_primitives_from_file{};
        ar(n_imposed_primitives_from_file);

        if (n_imposed_primitives_from_file !=
            n_imposed_primitives_from_config) {
          throw std::runtime_error(std::string(
              MakeString{}
              << "Number of imposed_primitives from the configuration file : "
              << n_imposed_primitives_from_config
              << " does not match the number of imposed_primitives from the "
                 "earlier saved archive : "
              << n_imposed_primitives_from_file));
        }
        ar(imposed_primitives_module.imposed_primitives());
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar,
                              type const& imposed_primitives_module) {
        ar(imposed_primitives_module.n_imposed_primitives());
        // save systems data
        ar(imposed_primitives_module.imposed_primitives());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for the ImposedSystems
     * \ingroup interface
     */
    template <class SimTraits, class Sim>
    struct Serialize<
        ::elastica::interface::environments::WithPrimitives<SimTraits, Sim>> {
      using type =
          ::elastica::interface::environments::WithPrimitives<SimTraits, Sim>;
      template <typename InArchive>
      static inline void load(InArchive& ar, type& primitives_module) {
        const auto n_primitives_from_config = primitives_module.n_primitives();
        std::remove_const_t<decltype(n_primitives_from_config)>
            n_primitives_from_file{};
        ar(n_primitives_from_file);

        if (n_primitives_from_file != n_primitives_from_config) {
          throw std::runtime_error(
              std::string(
                  MakeString{}
                  << "Number of primitives from the configuration file : "
                  << n_primitives_from_config
                  << " does not match the number of primitives from the "
                     "earlier saved archive : "
                  << n_primitives_from_file));
        }

        // from n_primitives and systems, reload all data
        primitives_module.finalize(typename type::TimeType{});
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& primitives_module) {
        ar(primitives_module.n_primitives());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for the Simulator
     * \ingroup interface
     */
    template <class SimTraits, class Sim>
    struct Serialize<::elastica::modules::Environments<SimTraits, Sim>> {
      using type = ::elastica::modules::Environments<SimTraits, Sim>;
      using ImposedPrimitivesInterface =
          typename type::ImposedPrimitivesInterface;

      // 1. check if blocking is the same, else abort
      template <typename InArchive>
      static inline void load(InArchive& ar, type& environments_module) {
        // 1. save the policies blocking first
        ar(static_cast<ImposedPrimitivesInterface const&>(environments_module));
        // TODO : 2. load systems data
        // ar(systems_module.environments()));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& environments_module) {
        // 1. save the primitives first
        ar(static_cast<ImposedPrimitivesInterface const&>(environments_module));
        // TODO 2. save the environments as a buffer
        // ar(environments_module.enviroments());
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
