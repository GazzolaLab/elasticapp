//==============================================================================
/*!
//  \file
//  \brief Working directory for the metadata interface
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
#include <string>

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Experimental filesystem replacement
     * \ingroup io
     */
    struct WorkingDirectory {
      //! Current working directory
      std::string working_directory = ".";
      //! interface of file-system that we have to rely upon
      std::string string() const { return working_directory; }
      //! conversion operator
      explicit operator std::string() const { return working_directory; }
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Get file path using the interface
     * \ingroup io
     */
    std::string get_file_path(std::string formatted_file_name,
                              WorkingDirectory const& directory);
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
