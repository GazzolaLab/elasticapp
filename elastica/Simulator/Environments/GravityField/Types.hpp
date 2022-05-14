//==============================================================================
/*!
//  \file
//  \brief All gravity field types
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Environments/GravityField/ModuleConfiguration/Types.hpp"

namespace elastica {

  namespace environments {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename /*Traits*/>
    class GravityField;

    namespace tags {
      struct GravityField;
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica