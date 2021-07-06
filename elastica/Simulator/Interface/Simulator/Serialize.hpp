//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Simulator and its modules
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
#include <typeindex>

///
#include "Simulator/Interface/Types.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
/// base serialization
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
/// domain serialization
#include "Simulator/Domain/Serialize.hpp"
/// module serialization
#include "Simulator/Interface/Connections/Serialize.hpp"
#include "Simulator/Interface/Constraints/Serialize.hpp"
#include "Simulator/Interface/Environments/Serialize.hpp"
#include "Simulator/Interface/Forcing/Serialize.hpp"
#include "Simulator/Interface/Systems/Serialize.hpp"
#include "Simulator/Interface/Wrappers/Serialize.hpp"

namespace elastica {

  namespace io {

    // TODO : need to store time and time-step ID somewhere outside the main
    //        simulator
    // DONE : Ensure same simulation traits using some type_index(typeid())
    // https://en.cppreference.com/w/cpp/types/type_index
    // DONE : Ensure same modules by the same trick

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for the Simulator
     * \ingroup interface
     */
    template <typename SimTraits,
              template <typename /*SimTraits*/, typename /*Sim*/>
              class... Modules>
    struct Serialize<::elastica::interface::Simulator<SimTraits, Modules...>> {
      using type = ::elastica::interface::Simulator<SimTraits, Modules...>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& simulator) {
        try {
          try {
            // 0. check if simulator is actually ready for integration
            if (not simulator.ready()) {
              const char* error_message = R"error(
The simulator has not been finalized, yet the program is trying to load simulation
state from the archive. Please contact the Elastica++ developers!
)error";
              throw std::logic_error(error_message);
            }

            // 1. first save the hash based on the type index to confirm that
            // the modules needed are actually the ones that are being loaded.
            const auto simulator_uuid_from_config =
                std::type_index(typeid(type)).hash_code();
            std::size_t simulator_uuid_from_file(0UL);
            ar(simulator_uuid_from_file);

            if (simulator_uuid_from_file != simulator_uuid_from_config) {
              const char* error_message = R"error(
The type of Simulator (along with its modules) recorded in the configuration is
different from the requested type of Simulator (and its modules).
)error";
              throw std::runtime_error(error_message);
            }

            // 2. first load up all policies
            ar(simulator.domain());

            // 3. load the systems module
            ar(static_cast<typename type::SystemsModule&>(simulator));

            // 4. Travel up the modules hierarchy and call serialize on them
            // all
            ar(static_cast<Modules<SimTraits, type>&>(simulator)...);

            // trigger all the "computational parts" of the simulator, such as
            // transferring primitives and so on.
          } catch (...) {
            const char* error_message = "Loading the Simulator state failed :";
            std::throw_with_nested(std::runtime_error(error_message));
          }
        } catch (const std::exception& e) {
          abort_with_backtrace(e);
        }
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& simulator) {
        try {
          try {
            // 0. check if simulator ready
            if (not simulator.ready()) {
              const char* error_message = R"error(
The simulator has not been finalized, yet the program is trying to save simulation
state from the archive. Please contact the Elastica++ developers!
)error";
              throw std::logic_error(error_message);
            }

            // 1. first save the hash based on the type index to confirm that
            // the modules needed are actually the ones that are being loaded.
            ar(std::type_index(typeid(type)).hash_code());

            // 2. first load up all policies
            // Algorithms are not serialized by design. The reason for this is
            // two- fold
            // 1. The algorithm policy and any embedded parameters only decides
            // the scheduling---so even if this changes between serialization/
            // de-serialization, it does not have any effect on the validity of
            // the program
            // 2. Its hard to specialize the template of Serialize for a nested
            // class (which includes all the Algorithms), as type deduction
            // fails. While we can use tricks to overcome this (add another
            // layer of indirection, as usual), its not worth the effort and
            // upkeep.
            ar(simulator.domain());

            // 3. load the systems module
            ar(static_cast<typename type::SystemsModule const&>(simulator));

            // 4. load modules
            ar(static_cast<Modules<SimTraits, type> const&>(simulator)...);

            // have an option to continue working as if nothing happened if the
            // simulator fails to save
          } catch (...) {
            const char* error_message = "Saving the Simulator failed :";
            std::throw_with_nested(std::runtime_error(error_message));
          }
        } catch (const std::exception& e) {
          abort_with_backtrace(e);
        }
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
