//==============================================================================
/*!
//  \file
//  \brief Optimization level for operations within Cosserat Rod kernels
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
#include <cstdint>

namespace elastica {

  namespace cosserat_rod {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Optimization levels
     * \ingroup cosserat_rods
     *
     * Define level of optimization for the kernels employed within the
     * cosserat rod module
     */
    enum class OptimizationLevel : std::uint8_t { basic = 0, optimized };
    /*! \endcond */
    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica
