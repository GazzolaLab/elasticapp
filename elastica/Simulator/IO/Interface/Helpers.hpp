//==============================================================================
/*!
//  \file
//  \brief Helpers for formatting filenames
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
//
#include <cstddef>
#include <string>

namespace elastica {

  namespace io {

    //**Internal load***********************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Helpers to format names
    // \ingroup io
    */
    auto format_id(std::size_t n_zeros, std::string idstr, char pad = '0')
        -> std::string;
    auto format_suffix(std::string const& suffix, char pad = '_')
        -> std::string;
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica