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

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Simulator/IO/Interface/MetadataInterface.hpp"
//
#include "Simulator/IO/YAML/Types.hpp"
//
#include "Time/Types.hpp"
//
#include "Simulator/IO/Serialization/Archives/YAML.hpp"
//
#include <fstream>
#include <string>
//

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Manager for \elastica wide YAML file metadata
     * \ingroup io
     *
     * \details
     * YAMLMetadata is used as an entry point to get visualizers and archives
     * for to maintain metadata for an \elastica simulation when using YAML. It
     * writes a `elastica_metadata.h5` file that keeps track of the temporal
     * details while forwarding other implementation to the visualizer/archive.
     */
    class YAMLMetadata : public MetadataInterface {
     protected:
      //**Type definitions******************************************************
      //! Internal naming
      using FileName = std::string;
      //! Interface Type
      using Interface = MetadataInterface;
      //************************************************************************

     public:
      //**Static members********************************************************
      //! Default meta-data filename
      static auto metadata_filename() -> std::string {
        return "elastica_metadata.yml";
      }
      //! Extension filename
      static auto extension() -> std::string { return "yml"; }
      //************************************************************************

      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      // YamlCPP : YAMLOutputArchive archive cannot be copied or moved
      //      explicit YAMLMetadata(YAMLOutputArchive yaml_metadata,
      //                            NamingConvention naming_convention = {},
      //                            WorkingDirectory cwd = {});
      explicit YAMLMetadata(NamingConvention naming_convention,
                            WorkingDirectory cwd = {});
      YAMLMetadata();
      //@}
      //************************************************************************

     public:
      //**Access operators******************************************************
      /*!\name Access operators */
      //@{

      //************************************************************************
      /*!\brief Access to the metadata file.
       */
      inline auto metadata_file() & noexcept -> std::ofstream& {
        return metadata_;
      }
      inline auto metadata_file() const& noexcept -> std::ofstream const& {
        return metadata_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the metadata archive.
       */
      inline auto metadata_archive() & noexcept -> YAMLOutputArchive& {
        return metadata_archive_;
      }
      inline auto metadata_archive() const& noexcept
          -> YAMLOutputArchive const& {
        return metadata_archive_;
      }
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //************************************************************************
      //! Internal registry for temporal metadata
      auto register_metadata(::elastica::TimeMetadata time_metadata)
          -> FileName;
      //************************************************************************

     public:
      //**Generator methods*****************************************************
      /*!\name Generator methods */
      //@{
      //! Generates a YAML writer at given time
      auto write_at(TimeMetadata time_metadata) -> std::ofstream&;
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Metadata stream
      std::ofstream metadata_;
      //! Metadata archive
      YAMLOutputArchive metadata_archive_;
      //! Recording stream
      std::ofstream record_stream_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
