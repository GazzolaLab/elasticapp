//==============================================================================
/*!
//  \file
//  \brief Tags indicating derivative orders of temporal states
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

/// Types always first
#include "Systems/States/Expressions/OrderTags/Types.hpp"
///
#include "Systems/States/Expressions/OrderTags/OrderTags.hpp"
#include "Systems/States/Expressions/OrderTags/TypeTraits.hpp"

namespace elastica {

  namespace states {

    //**************************************************************************
    /*!\brief Tags marking derivative orders for evolving states
    // \ingroup states
    */
    namespace tags{}
    //**************************************************************************
  }

}  // namespace elastica
