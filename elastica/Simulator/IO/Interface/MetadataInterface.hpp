//==============================================================================
/*!
//  \file
//  \brief Interface for recording metadata and launching observers
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
#include "Simulator/IO/Interface/Types.hpp"
//
#include "Simulator/IO/Interface/FileNamingConvention.hpp"
#include "Simulator/IO/Interface/WorkingDirectory.hpp"
//
#include "Time/Types.hpp"
//
#include <string>
#include <utility>  // move
//

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Interface for \elastica wide file observation/metadata
     * \ingroup io
     *
     * \details
     * MetadataInterface is used as an entry point to get visualizers and
     * archives to maintain metadata for an \elastica simulation.
     */
    class MetadataInterface {
     public:
      //**Type definitions******************************************************
      //! Naming convention
      using NamingConvention = FileNamingConvention;
      //! Time data
      using TimeMetadata = ::elastica::TimeMetadata;
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      explicit MetadataInterface(NamingConvention naming_convention = {},
                                 WorkingDirectory cwd = {})
          : naming_convention_(std::move(naming_convention)),
            cwd_(std::move(cwd)) {}
      //@}
      //************************************************************************

      //**Access operators******************************************************
      /*!\name Access operators */
      //@{

      //************************************************************************
      /*!\brief Access to the working directory.
       */
      inline auto cwd() & noexcept -> WorkingDirectory& { return cwd_; }
      inline auto cwd() const& noexcept -> WorkingDirectory const& {
        return cwd_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Access to the naming convention.
       */
      inline auto naming_convention() & noexcept -> NamingConvention& {
        return naming_convention_;
      }
      inline auto naming_convention() const& noexcept
          -> NamingConvention const& {
        return naming_convention_;
      }
      //************************************************************************

      //@}
      //************************************************************************
     
     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Naming convention for the files generated
      NamingConvention naming_convention_;
      //! Working directory
      WorkingDirectory cwd_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Format a name using the interface
     * \ingroup io
     */
    inline std::string format(std::string unformatted_name,
                              MetadataInterface const& interface) {
      return format(std::move(unformatted_name), interface.naming_convention());
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Get file name using the interface
     * \ingroup io
     */
    inline std::string get_file_name(std::string formatted_name,
                                     MetadataInterface const& interface) {
      return get_file_name(std::move(formatted_name),
                           interface.naming_convention());
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Get file path using the interface
     * \ingroup io
     */
    inline std::string get_file_path(std::string formatted_file_name,
                                     MetadataInterface const& interface) {
      return get_file_path(std::move(formatted_file_name), interface.cwd());
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica