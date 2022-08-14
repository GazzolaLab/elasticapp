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

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/IO/Interface/Types.hpp"
//
#include <cstddef>
#include <string>

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Customization for output file names in \elastica
     * \ingroup io
     *
     * \details
     * FileNamingConvention concretizes and customizes the naming convention of
     * output files written by \elastica. We adopt file names of the form
     * `{prefix}_{content}_{suffix}.{extension}`
     * with the `content` being padded by a `pad` character of `padding_length`.
     *
     * \example
     * With FileNamingConvention {"elastica", "viz", "h5"} and the content being
     * "50", then the output can be customized as `elastica_000050_viz.h5`.
     */
    struct FileNamingConvention {
      //! Prefix to attach to file names
      std::string prefix{"elastica"};
      //! Suffix to attach to file names
      std::string suffix;
      //! Extension to attach to file names
      std::string extension;
      struct FileNamingFormatting {
        //! Character for padding
        char pad = '0';
        //! Length of padding the content with the above character
        std::uint8_t padding_length = 6U;
      } format{};
      //! Delimiter
      char delimiter = '_';
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Format a name using the interface
     * \ingroup io
     */
    std::string format(std::string unformatted_name,
                       FileNamingConvention const& convention);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Get file name using the interface
     * \ingroup io
     */
    std::string get_file_name(std::string formatted_name,
                              FileNamingConvention const& convention);
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
