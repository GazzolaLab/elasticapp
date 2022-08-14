//==============================================================================
/*!
//  \file
//  \brief Helpers for dealing with H5 files
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
#include <highfive/H5File.hpp>
#include <string>

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Helpers to format names
    // \ingroup io
    */
    inline auto open_h5_file(std::string const& fname) -> HighFive::File {
      return HighFive::File(fname, HighFive::File::ReadWrite |
                                       HighFive::File::Create |
                                       HighFive::File::Truncate);
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
