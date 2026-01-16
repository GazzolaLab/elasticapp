//==============================================================================
/*!
//  \file
//  \brief Manager for simulation-wide YAML metadata
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "Simulator/IO/YAML/YAMLMetadata.hpp"

//
#include "Time/Serialize.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/TimeMetadata.hpp"
#include "Time/TimeStepID.hpp"
//
#include "Simulator/IO/Interface/Helpers.hpp"  // format
#include "Simulator/IO/Serialization/STL.hpp"
//
#include <utility>

namespace elastica {

  namespace io {

    struct Meta {
      ::elastica::TimeMetadata timedata;
      std::string filename;
    };

    template <>
    struct Serialize<Meta> {
      using type = Meta;

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& t) {
        ar(make_nvp("TimeMetadata", t.timedata),
           make_nvp("Filename", t.filename));
      }

      template <typename InArchive>
      static inline void load(InArchive& ar, type& t) {
        ar(make_nvp("TimeMetadata", t.timedata),
           make_nvp("Filename", t.filename));
      }
    };

    // YamlCPP : YAMLOutputArchive archive cannot be copied or moved
    //    YAMLMetadata::YAMLMetadata(YAMLOutputArchive yamlmetadata,
    //                               NamingConvention naming_convention,
    //                               WorkingDirectory cwd)
    //        : Interface(std::move(naming_convention), std::move(cwd)),
    //          metadata_(),  // leave the stream empty, it won't be used for
    //                        // execution
    //          metadata_archive_(std::move(yamlmetadata)) {
    //      this->naming_convention().extension = extension();
    //    }

    YAMLMetadata::YAMLMetadata(NamingConvention naming_convention,
                               WorkingDirectory cwd)
        : Interface(std::move(naming_convention), std::move(cwd)),
          metadata_(get_file_path(metadata_filename(), this->cwd()),
                    std::ios::out | std::ios::trunc),
          metadata_archive_(metadata_),
          record_stream_() {
      this->naming_convention().extension = extension();
    }

    YAMLMetadata::YAMLMetadata()
        : YAMLMetadata(NamingConvention{}, WorkingDirectory{}) {}

    auto YAMLMetadata::register_metadata(::elastica::TimeMetadata time_metadata)
        -> FileName {
      auto const raw_id = std::to_string(time_metadata.id.step());
      auto formatted_id = ::elastica::io::format(raw_id, *this);
      std::string file_name = get_file_name(std::move(formatted_id), *this);

      // Prepending 0 in front of the string makes YAML interpret results
      // differently. Quoting also does not help in YamlCPP.
      // https://stackoverflow.com/questions/54820610/how-to-dump-a-digit-string-with-leading-zeros-as-a-valid-yaml-string-in-yaml-cp
      metadata_archive()(
          make_nvp(raw_id, Meta{time_metadata, file_name}));
      return file_name;
    }

    auto YAMLMetadata::write_at(TimeMetadata time_metadata) -> std::ofstream& {
      if (record_stream_.is_open()) {
        record_stream_.close();
        record_stream_.clear();
      }

      record_stream_.open(
          get_file_path(register_metadata(time_metadata), *this),
          std::ios::out | std::ios::trunc);

      return record_stream_;
    }

  }  // namespace io

}  // namespace elastica
