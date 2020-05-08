#pragma once

namespace elastica{
  namespace systems{
    struct SystemBase {
      SystemBase(double ks, double ds) : kinematic_states(ks), dynamic_states(ds){};
      virtual ~SystemBase() = default;
      virtual double kinematic_rates(double time_v) noexcept = 0;
      virtual double dynamic_rates(double time_v) noexcept = 0;
      double kinematic_states, dynamic_states;
    };
  }

  using systems::SystemBase;
}
