//==============================================================================
/*!
//  \file
//  \brief Working directory for the metadata interface
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/IO/Interface/WorkingDirectory.hpp"
//
// TODO : filesystem here
//
#include <utility>  // move

namespace elastica {

  namespace io {

    std::string get_file_path(std::string formatted_file_name,
                              WorkingDirectory const& directory) {
      // TODO : replace with a proper filesystem
      return directory.working_directory + "/" + std::move(formatted_file_name);
    }

  }  // namespace io

}  // namespace elastica