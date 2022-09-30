//==============================================================================
/*!
//  \file
//  \brief Parameters of the hierarchical hash grids coarse collision
//  detection algorithm
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

#include "ModuleSettings/HashGrids.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

//******************************************************************************
/*!\brief Stamp parameters
 * \ingroup coarse_collision_detection
 */
#define STAMP_PARAMETER(VAR) \
  VAR { elastica::collision::detection::coarse::hhg::VAR() }

        //**********************************************************************
        /*!\brief Parameters for the 'Hierarchical Hash Grids' coarse
         * collision detection algorithm.
         * \ingroup coarse_collision_detection
         */
        struct HashGridsConfigurationParameters {
          //! \copydoc initial_x_cell_count
          std::size_t STAMP_PARAMETER(initial_x_cell_count);
          //! \copydoc initial_y_cell_count
          std::size_t STAMP_PARAMETER(initial_y_cell_count);
          //! \copydoc initial_z_cell_count
          std::size_t STAMP_PARAMETER(initial_z_cell_count);
          //! \copydoc initial_cell_vector_size
          std::size_t STAMP_PARAMETER(initial_cell_vector_size);
          //! \copydoc initial_occupied_cells_vector_size
          std::size_t STAMP_PARAMETER(initial_occupied_cells_vector_size);
          //! \copydoc minimal_grid_density
          std::size_t STAMP_PARAMETER(minimal_grid_density);
          //! \copydoc hashgrid_activation_threshold
          std::size_t STAMP_PARAMETER(hashgrid_activation_threshold);
          //! \copydoc hierarchy_factor
          real_t STAMP_PARAMETER(hierarchy_factor);
        };
        //**********************************************************************

#undef STAMP_PARAMETER

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica