//
// Created by Tejaswin Parthasarathy on 1/22/21.
//

#include "Simulator/Forcing/Forcing.hpp"
#include "Simulator/Forcing/Registration.hpp"
#include "Utilities/TMPL.hpp"

#include <type_traits>

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingForTestingRegistration) {
     public:
      explicit ForcingForTestingRegistration() noexcept = default;
      ForcingForTestingRegistration(ForcingForTestingRegistration &&
                                    other) noexcept = default;
      ~ForcingForTestingRegistration() override = default;
      template <typename TimeType, typename System>
      void apply_forces(TimeType, System&, int) const {}
      template <typename TimeType, typename System>
      void apply_torques(TimeType, System&, int) const {}
      static std::string name() noexcept {
        return "ConstrainForTestingRegistration";
      }
    };

  }  // namespace forcing

}  // namespace elastica

using ForcingType = elastica::forcing::ForcingForTestingRegistration<>;
static_assert(std::is_base_of<elastica::forcing::Forcing<
                                  tmpl::list<elastica::forcing::Registrars::
                                                 ForcingForTestingRegistration>,
                                  ForcingType>,
                              ForcingType>::value,
              "ELASTICA_DEFINE_FORCING() error!");
