//==============================================================================
/*!
//  \file
//  \brief Manager for simulation-wide H5 metadata
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "Simulator/IO/H5/Interface/H5Metadata.hpp"

#include "Simulator/IO/H5/Time.hpp"
//
#include "Time/SimulationTime.hpp"
#include "Time/TimeMetadata.hpp"
#include "Time/TimeStepID.hpp"
//
#include <utility>

#include "Simulator/IO/H5/Interface/H5FileHelpers.hpp"
#include "Simulator/IO/H5/Interface/H5Interface.hpp"
#include "Simulator/IO/Interface/Helpers.hpp"  // format

namespace elastica {

  namespace io {

    H5Metadata::H5Metadata(HighFive::File h5metadata,
                           NamingConvention naming_convention,
                           WorkingDirectory cwd)
        : Interface(std::move(naming_convention), std::move(cwd)),
          metadata_(std::move(h5metadata)) {
      this->naming_convention().extension = extension();
    }

    H5Metadata::H5Metadata(NamingConvention naming_convention,
                           WorkingDirectory cwd)
        : H5Metadata(open_h5_file(get_file_path(metadata_filename(), cwd)),
                     std::move(naming_convention), std::move(cwd)) {}

    H5Metadata::H5Metadata()
        : H5Metadata(NamingConvention{}, WorkingDirectory{}) {}

    auto H5Metadata::register_metadata(::elastica::TimeMetadata time_metadata)
        -> FileName {
      auto formatted_id =
          format(std::to_string(time_metadata.id.step()), *this);

      auto metadata_group = metadata_file().createGroup(formatted_id);
      write_data(metadata_group, time_metadata, "TimeMetadata");

      // form a link
      std::string file_name = get_file_name(std::move(formatted_id), *this);
      metadata_group.createExternalLink("data", file_name,
                                        /*root path within H5 file*/ "/");
      // Flush metadata file so that it can be read in while its being written out.
      metadata_file().flush();
      return file_name;
    }

    auto H5Metadata::write_at(TimeMetadata time_metadata) -> H5Writer {
      return {
          open_h5_file(get_file_path(register_metadata(time_metadata), *this))};
    }

  }  // namespace io

}  // namespace elastica