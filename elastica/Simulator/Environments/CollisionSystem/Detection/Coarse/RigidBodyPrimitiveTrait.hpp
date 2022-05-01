//==============================================================================
/*!
//  \file
//  \brief Customization of rigid-body class based on coarse collision detection
//  algorithm
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
**
**  Project home: https://www.cs10.tf.fau.de/research/software/pe/
**
**  Copyright (C) 2009 Klaus Iglberger
**
**  This file is part of pe.
**
**  pe is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License as published by the Free Software
** Foundation, either version 3 of the License, or (at your option) any later
** version.
**
**  pe is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License along with
** pe. If not, see <http://www.gnu.org/licenses/>.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cmath>

#include "Simulator/Environments/CollisionSystem/Detection/Coarse/BoundingBox.hpp"
///
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/Types.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
#include "Simulator/Frames.hpp"
///
#include "Utilities/DefineTypes.h"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //========================================================================

        //************************************************************************
        /*!\brief Rigid body customization class for the collision response.
         * \ingroup collision_response
         *
         * The RigidBodyPrimitiveTrait class template is used to adapt the
         * RigidBody class to the used collision detection algorithm. Depending
         * on the used algorithm, a body may require additional data or
         * functionality to efficiently support the collision response
         * calculations.
         *
         * In order to adapt the RigidBody class to a particular algorithm, the
         * base template needs to be specialized.
         *
         * \tparam C The configuration of collision detection
         */
        template <typename C>
        class RigidBodyPrimitiveTrait {
         public:
          //**Type definitions**************************************************
          //! Type of the axis-aligned bounding box.
          using AABB = BoundingBox<real_t>;
          //********************************************************************

         protected:
          //**Constructor*******************************************************
          /*!\name Constructor */
          //@{

          //********************************************************************
          /*!\brief Default implementation of the RigidBodyPrimitiveTrait
           * constructor.
           *
           * \param body The rigid body containing this bounding box.
           */
          explicit inline RigidBodyPrimitiveTrait(BodyID /*body*/)
              : aabb_()  // Axis-aligned bounding box for the rigid body
                {};
          //@}
          //********************************************************************

          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          //! Axis-aligned bounding box for the rigid body.
          AABB aabb_;
          //@}
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  SPECIALIZATION OF RigidBodyPrimitiveTrait FOR
        //  THE HIERARCHICAL HASH GRID ALGORITHM
        //
        //======================================================================

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Specialization of the RigidBodyPrimitiveTrait class template
         * for the 'hierarchical hash-grid' algorithm. \ingroup
         * coarse_collision_detection
         *
         * This specialization of the RigidBodyPrimitiveTrait class template
         * adapts rigid bodies to the hierarchical hash-grid coarse collision
         * detection algorithm.
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        class RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR>> {
         public:
          //**Type definitions**************************************************
          //! Type of the axis-aligned bounding box.
          using AABB = BoundingBox<real_t>;
          //********************************************************************

         protected:
          //**Constructor*******************************************************
          /*!\name Constructor */
          //@{
          explicit inline RigidBodyPrimitiveTrait(BodyID body);
          //@}
          //********************************************************************

         public:
          //**Get functions*****************************************************
          /*!\name Get functions */
          //@{
          inline real_t get_AABB_size() const noexcept;
          inline void* get_grid() const noexcept;
          inline std::size_t get_hash() const noexcept;
          inline std::size_t get_cell_ID() const noexcept;
          //@}
          //********************************************************************

          //**Set functions*****************************************************
          /*!\name Set functions */
          //@{
          inline void set_grid(void* grid) noexcept;
          inline void set_hash(std::size_t hash) noexcept;
          inline void set_cell_ID(std::size_t cell) noexcept;
          //@}
          //********************************************************************

         protected:
          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          //! Axis-aligned bounding box for the rigid body.
          AABB aabb_;
          //! Pointer to the hash grid this rigid body is currently assigned to.
          void* grid_;
          //! Current hash value of this rigid body.
          std::size_t hash_;
          //! The body's index in the body container of the grid cell this rigid
          //! body is currently assigned to.
          std::size_t cell_ID_;
          //@}
          //********************************************************************
        };
        /*! \endcond */
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constructor for the RigidBodyPrimitiveTrait<HashGrids>
         * specialization.
         *
         * \param body The rigid body containing this bounding box.
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline RigidBodyPrimitiveTrait<
            C<HashGrids, FD, BG, CR>>::RigidBodyPrimitiveTrait(BodyID /*body*/)
            : aabb_(),         // Axis-aligned bounding box for the rigid body
              grid_(nullptr),  // Pointer to the hash grid this rigid body is
                               // currently assigned to
              hash_(0),        // Current hash value of this rigid body
              cell_ID_(0)  // The body's index in the body container of the grid
                           // cell this rigid body is currently assigned to
        {}
        //**********************************************************************

        //**********************************************************************
        /*!\brief Returns the length of the longest side of the AABB of the
         * rigid body.
         *
         * \return The length of the longest side of the AABB of the rigid body.
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline real_t RigidBodyPrimitiveTrait<
            C<HashGrids, FD, BG, CR>>::get_AABB_size() const noexcept {
          // real_t size = aabb_[3] - aabb_[0];
          // real_t yDiff = aabb_[4] - aabb_[1];
          // real_t zDiff = aabb_[5] - aabb_[2];
          const auto x_size = length_along<::elastica::EulerianFrame::X>(aabb_);
          const auto y_size = length_along<::elastica::EulerianFrame::Y>(aabb_);
          const auto z_size = length_along<::elastica::EulerianFrame::Z>(aabb_);
          return std::max(x_size, std::max(y_size, z_size));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Returns the grid the rigid body is currently assigned to.
         *
         * \return The grid the rigid body is currently assigned to.
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline void* RigidBodyPrimitiveTrait<
            C<HashGrids, FD, BG, CR>>::get_grid() const noexcept {
          return grid_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Returns the current hash value of the rigid body.
         *
         * \return The current hash value of the rigid body.
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline std::size_t RigidBodyPrimitiveTrait<
            C<HashGrids, FD, BG, CR>>::get_hash() const noexcept {
          return hash_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Returns the current body container index within the cell the
         * body is currently assigned to.
         *
         * \return The current body container index.
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline std::size_t RigidBodyPrimitiveTrait<
            C<HashGrids, FD, BG, CR>>::get_cell_ID() const noexcept {
          return cell_ID_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Setting the grid the rigid body is associated with.
         *
         * \param grid The grid the rigid body is assigned to.
         * \return void
         *
         * \todo
         * Update according to modern C++
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline void RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR>>::set_grid(
            void* grid) noexcept {
          grid_ = grid;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Setting the hash value of the rigid body.
         *
         * \param hash The new hash value of the rigid body.
         * \return void
         *
         * \todo
         * Update according to modern C++
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline void RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR>>::set_hash(
            std::size_t hash) noexcept {
          hash_ = hash;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Setting the body container index within the cell the body is
         * currently assigned to.
         *
         * \param cell The new body container index.
         * \return void
         *
         * \todo
         * Update according to modern C++
         */
        template <
            // Type of the fine collision detection algorithm
            typename FD,
            // Type of the batch generation algorithm
            template <typename> class BG,
            // Type of the collision response algorithm
            template <typename, typename, typename> class CR,
            template <
                // Template signature of the coarse collision detection
                // algorithm
                template <typename> class,
                // Template signature of the fine collision detection algorithm
                typename,
                // Template signature of the batch generation algorithm
                template <typename> class,
                // Template signature of the collision response algorithm
                template <typename, typename, typename> class>
            class C>  // Type of the configuration
        inline void
        RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR>>::set_cell_ID(
            std::size_t cell) noexcept {
          cell_ID_ = cell;
        }
        //**********************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
