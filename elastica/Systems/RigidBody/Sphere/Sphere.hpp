//==============================================================================
/*!
//  \file
//  \brief \elastica Sphere
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
// common
#include "Systems/common/Types.hpp"
// blocks
#include "Systems/Block/Types.hpp"
// rod components
#include "Systems/RigidBody/Types.hpp"
//
#include "Systems/RigidBody/Components/WithRigidBodyKinematics.hpp"
#include "Systems/RigidBody/Sphere/Name.hpp"
#include "Systems/RigidBody/Sphere/WithSphereGeometry.hpp"

namespace elastica {

  namespace rigid_body {

    namespace detail {

      template <typename T, typename B>
      using SphereComponents = ::elastica::SymplecticPolicy<
          T, B, ::elastica::rigid_body::WithSphereGeometry,
          ::elastica::rigid_body::WithRigidBodyKinematics,
          ::elastica::rigid_body::SphereNameAdapted>;

      // template <typename T, typename B>
      // using SphereComponents = ::elastica::rigid_body::WithSphereGeometry<T,
      // B>;

      using Sphere = ::elastica::rigid_body::RigidBodyPlugin<
          ::elastica::rigid_body::DefaultRigidBodyTraits, ::blocks::Block,
          SphereComponents>;
      using SphereSlice = ::elastica::rigid_body::RigidBodyPlugin<
          ::elastica::rigid_body::DefaultRigidBodyTraits, ::blocks::BlockSlice,
          SphereComponents>;
      using SphereConstSlice = ::elastica::rigid_body::RigidBodyPlugin<
          ::elastica::rigid_body::DefaultRigidBodyTraits,
          ::blocks::ConstBlockSlice, SphereComponents>;
      using SphereView = ::elastica::rigid_body::RigidBodyPlugin<
          ::elastica::rigid_body::DefaultRigidBodyTraits, ::blocks::BlockView,
          SphereComponents>;
      using SphereConstView = ::elastica::rigid_body::RigidBodyPlugin<
          ::elastica::rigid_body::DefaultRigidBodyTraits,
          ::blocks::ConstBlockView, SphereComponents>;

      using SphereBlock = ::blocks::Block<Sphere>;
      using SphereBlockSlice = ::blocks::BlockSlice<SphereSlice>;
      using SphereBlockConstSlice = ::blocks::ConstBlockSlice<SphereConstSlice>;
      using SphereBlockView = ::blocks::BlockView<SphereView>;
      using SphereBlockConstView = ::blocks::ConstBlockView<SphereConstView>;

    }  // namespace detail

  }  // namespace rigid_body

}  // namespace elastica