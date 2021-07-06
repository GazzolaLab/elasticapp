//
// Created by Tejaswin Parthasarathy on 1/22/21.
//

#include <type_traits>

#include "Simulator/Connections/Connection.hpp"
#include "Simulator/Connections/Registration.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionForTestingRegistration) {
     public:
      explicit ConnectionForTestingRegistration() noexcept = default;
      ConnectionForTestingRegistration(ConnectionForTestingRegistration &&
                                       other) noexcept = default;
      ~ConnectionForTestingRegistration() override = default;
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType, FirstSystem&, int, SecondSystem&, int) const {
      }
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType, FirstSystem&, int, SecondSystem&, int)
          const {}
      static std::string name() noexcept {
        return "ConstrainForTestingRegistration";
      }
    };

  }  // namespace connections

}  // namespace elastica

using ConnectionType =
    elastica::connections::ConnectionForTestingRegistration<>;
static_assert(
    std::is_base_of<elastica::connections::Connection<
                        tmpl::list<elastica::connections::Registrars::
                                       ConnectionForTestingRegistration>,
                        ConnectionType>,
                    ConnectionType>::value,
    "ELASTICA_DEFINE_CONNECTION() error!");
