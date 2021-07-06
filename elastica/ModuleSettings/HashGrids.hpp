#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/ValueTraits/IsPowerOf.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace hhg {

          //====================================================================
          //
          //  HIERARCHICAL HASH GRID COARSE COLLISION DETECTION CONFIGURATION
          //
          //====================================================================

#include "Configuration/HashGrids.hpp"

          namespace {

            // TODO : Descriptive error messages
            // Initial cell count > 4 so that they have valid neighbors
            // to test against
            /* Quoting from "Hierarchical Hash Grids for Coarse Collision
             * Detection " Schornbaum, Page 33
             *
             * " Every cell in a two-dimensional
             * hash grid must have eight unique neighbors. In a
             * three-dimensional hash grid, every cell must have 26 unique
             * neighbors. Because of periodic boundary conditions, this means
             * that there must be a minimum number of three cells in each
             * coordinate direction. Moreover, since the spatial dimensions of a
             * hash grid are restricted to powers of two, every hash grid must
             * consist of at least four cells in each coordinate direction."
             */
            static_assert(initial_x_cell_count() >= 4 and
                              is_power_of_2(initial_x_cell_count()),
                          "undesirable initial x cell count values!");
            static_assert(initial_y_cell_count() >= 4 and
                              is_power_of_2(initial_y_cell_count()),
                          "undesirable initial y cell count values!");
            static_assert(initial_z_cell_count() >= 4 and
                              is_power_of_2(initial_z_cell_count()),
                          "undesirable initial z cell count values!");
            static_assert(minimal_grid_density() > 0,
                          "Minimal grid density < 0!");

          }  // namespace

        }  // namespace hhg

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica