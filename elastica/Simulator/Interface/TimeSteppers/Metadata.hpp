//==============================================================================
/*!
//  \file
//  \brief Marks an algorithmic metadata type
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

///
#include "Simulator/Interface/TimeSteppers/Types.hpp"
//
#include "Utilities/WidthStream.hpp"
//
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace elastica {

  namespace interface {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Algorithm metadata
     * \ingroup interface
     *
     * Carries meta-data information about steps in the algorithm
     */
    struct AlgorithmMetadata {
      using Result = std::pair<std::string, std::string>;
      using Metadata = std::vector<Result>;
      Metadata data;
    };
    //**************************************************************************

    // TODO : push to CPP
    inline std::ostream& operator<<(std::ostream& os,
                                    AlgorithmMetadata const& metadata) {
      // This is a very simple YAML formatting.
      // For a better one, see YAML Archive
      constexpr std::size_t line_length = 120UL;
      ::elastica::widthstream stream{line_length, os};

      stream << "Algorithm"
             << " : " << std::endl;
      stream.indent(2);

      for (auto const& data : metadata.data) {
        stream << "- " << std::endl;
        stream.indent(2);
        stream << data.first << ": "
               << "\"" << data.second << "\"" << std::endl;
        stream.indent(-2);
      }

      stream.indent(-2);
      return os;
    }

  }  // namespace interface

}  // namespace elastica
