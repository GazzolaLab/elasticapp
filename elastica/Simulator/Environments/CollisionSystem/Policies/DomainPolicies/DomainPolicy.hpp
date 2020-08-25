#pragma once

namespace elastica {

  namespace collision {

    // TODO

    namespace detail {
      template <typename SimulatorConfig, typename DomainType>
      class DomainPolicyImpl;
    }

    template <typename SimulatorConfig>
    using DomainPolicy =
        detail::DomainPolicyImpl<SimulatorConfig,
                                 typename SimulatorConfig::DomainType>;

  }  // namespace collision

}  // namespace elastica
