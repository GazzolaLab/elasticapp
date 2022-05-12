//
// Created by Tejaswin Parthasarathy on 5/11/22.
//

#pragma once

#include "Simulator/Environments/Primitives/Access.hpp"

namespace testing {

  template <typename AnyPrimitive>
  void check_access_API(AnyPrimitive& primitive) {
    (void)(::elastica::position(primitive));
    (void)(::elastica::director(primitive));
    (void)(::elastica::velocity(primitive));
    (void)(::elastica::angular_velocity(primitive));
    (void)(::elastica::external_loads(primitive));
    (void)(::elastica::external_torques(primitive));
  }

}  // namespace testing
