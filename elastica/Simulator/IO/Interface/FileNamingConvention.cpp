//==============================================================================
/*!
//  \file
//  \brief Defines a naming convention used for \elastica output files
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/IO/Interface/FileNamingConvention.hpp"
//
#include "Simulator/IO/Interface/Helpers.hpp"  // format
//
#include <utility>  // move

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Format a name using the interface
     * \ingroup io
     */
    std::string format(std::string unformatted_name,
                       FileNamingConvention const& convention) {
      return format_id(convention.format.padding_length,
                       std::move(unformatted_name), convention.format.pad);
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Get file name using the interface
     * \ingroup io
     */
    std::string get_file_name(std::string formatted_name,
                              FileNamingConvention const& convention) {
      return convention.prefix + convention.delimiter +
             std::move(formatted_name) +
             format_suffix(convention.suffix, convention.delimiter) + "." +
             convention.extension;
    };
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
