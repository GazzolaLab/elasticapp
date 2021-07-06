//
// Created by Tejaswin Parthasarathy on 6/24/21.
//

#pragma once

#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/MockSimulator.hpp"

namespace elastica {

  namespace io {
    // specialize simulator serialize for checking modules load/fail
    template <typename SimTraits,
        template <typename /*SimTraits*/, typename /*Sim*/> class Module>
    struct Serialize<::mock::MockSingleComponentSimulator<SimTraits, Module>> {
    using type = ::mock::MockSingleComponentSimulator<SimTraits, Module>;

    // copy lofic
    template <typename InArchive>
    static inline void load(InArchive& ar, type& simulator) {
      // 4. Travel up the modules hierarchy and call serialize on them
      // all
      try {
        ar(static_cast<Module<SimTraits, type>&>(simulator));
      } catch (...) {
        const char* error_message = "Loading the Simulator state failed :";
        std::throw_with_nested(std::runtime_error(error_message));
      }
    }

    template <typename OutArchive>
    static inline void save(OutArchive& ar, type const& simulator) {
      try {
        ar(static_cast<Module<SimTraits, type> const&>(simulator));
      } catch (...) {
        const char* error_message = "Saving the Simulator state failed :";
        std::throw_with_nested(std::runtime_error(error_message));
      }
    }
  };

}  // namespace io

}  // namespace elastica
