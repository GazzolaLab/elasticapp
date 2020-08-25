#pragma once

namespace elastica {

  namespace collision {

    // TODO
    namespace detection {

      namespace coarse {

        namespace detail {
          template <typename SimulatorConfig, typename DomainType>
          class DomainPolicyImpl;

          // For static members only
          class DomainPolicyImplBase;
        }

        template <typename SimulatorConfig>
        using DomainPolicy =
            detail::DomainPolicyImpl<SimulatorConfig,
                                     typename SimulatorConfig::DomainType>;

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
