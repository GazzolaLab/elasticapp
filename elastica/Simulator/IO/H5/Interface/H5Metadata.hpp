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

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Simulator/IO/Interface/MetadataInterface.hpp"
//
#include "Simulator/IO/H5/Interface/Types.hpp"
//
#include "Time/Types.hpp"
//
#include <highfive/H5File.hpp>
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
    /*!\brief Manager for \elastica wide H5 file metadata
     * \ingroup io
     *
     * \details
     * H5Metadata is used as an entry point to get visualizers and archives for
     * to maintain metadata for an \elastica simulation when using H5. It writes
     * a `elastica_metadata.h5` file that keeps track of the temporal details
     * while forwarding other implementation to the visualizer/archive.
     */
    class H5Metadata : public MetadataInterface {
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
        return "elastica_metadata.h5";
      }
      //! Extension filename
      static auto extension() -> std::string { return "h5"; }
      //************************************************************************

      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      explicit H5Metadata(HighFive::File h5metadata,
                          NamingConvention naming_convention = {},
                          WorkingDirectory cwd = {});
      explicit H5Metadata(NamingConvention naming_convention,
                          WorkingDirectory cwd = {});
      H5Metadata();
      //@}
      //************************************************************************

     public:
      //**Access operators******************************************************
      /*!\name Access operators */
      //@{

      //************************************************************************
      /*!\brief Access to the metadata file.
       */
      inline auto metadata_file() & noexcept -> HighFive::File& {
        return metadata_;
      }
      inline auto metadata_file() const& noexcept -> HighFive::File const& {
        return metadata_;
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
      //! Generates a H5 writer at given time
      auto write_at(TimeMetadata time_metadata) -> H5Writer;
      //      auto write_at(TimeMetadata time_metadata) -> HighFive::File;
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Metadata file
      HighFive::File metadata_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
