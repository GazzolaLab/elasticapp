//==============================================================================
/*!
//  \file
//  \brief Helpers to parse common elastica types
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include "Options/Options.hpp"
#include "Utilities/Math/Vec3.hpp"

// customize options for elastica::Vec3 and
template <>
struct Options::create_from_yaml<elastica::Vec3> {
  template <typename Metavariables>
  static elastica::Vec3 create(const Options::Option& options) {
    using intermediate_type = std::array<double, 3>;
    const auto array = options.parse_as<intermediate_type>();
    return elastica::Vec3(array);
    // return create<void>(options);
  }
};

// template <>
// elastica::Vec3 Options::create_from_yaml<elastica::Vec3>::create<void>(
//     const Options::Option& options) {
////  using intermediate_type = std::array<double, 3>;
////  const auto array = options.parse_as<intermediate_type>();
////  return elastica::Vec3(array);
//}
