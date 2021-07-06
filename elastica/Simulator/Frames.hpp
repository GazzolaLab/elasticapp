//==============================================================================
/*!
//  \file
//  \brief Frames in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Frames/EulerianFrame.hpp"
#include "Simulator/Frames/LagrangianFrame.hpp"

namespace elastica {

  using EulerianFrame = detail::EulerianFrame;
  using detail::cast_along;

  using LagrangianFrame = detail::LagrangianFrame;


  struct Frames {
    static_assert(LagrangianFrame::Dimension == EulerianFrame::Dimension,
                  "Wrong dimension configuration!");
    static constexpr auto Dimension = LagrangianFrame::Dimension;
  };

}  // namespace elastica