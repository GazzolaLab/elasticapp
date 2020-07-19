#pragma once

namespace elastica {
  namespace systems {
    struct SystemBase {
      SystemBase(double ks, double ds)
          : kinematic_states_(ks), dynamic_states_(ds){};
      virtual ~SystemBase() = default;
      virtual double kinematic_rates(double time_v) noexcept = 0;
      virtual double dynamic_rates(double time_v) noexcept = 0;
      double& kinematic_states() noexcept { return kinematic_states_; };
      double& dynamic_states() noexcept { return dynamic_states_; };
      double kinematic_states_, dynamic_states_;
    };
  }  // namespace systems

  using systems::SystemBase;
}  // namespace elastica
