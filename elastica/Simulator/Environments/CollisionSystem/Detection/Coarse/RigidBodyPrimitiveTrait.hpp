//=================================================================================================
/*!
 *  \file pe/core/detection/coarse/RigidBodyPrimitiveTrait.h
 *  \brief Rigid body customization class for the coarse collision detection
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

#include "BoundingBox.hpp"
#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //=================================================================================================
        //
        //  COARSE COLLISION DETECTION BODY TRAIT
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Rigid body customization class for the coarse collision
         * detection. \ingroup coarse_collision_detection
         *
         * The coarse detection module RigidBodyPrimitiveTrait class template is
         * used to adapt the RigidBody class to the used coarse collision
         * detection algorithm. For instance, this class configures the
         * axis-aligned bounding box for rigid bodies depending on the
         * requirements of the selected coarse collision detection algorithm.\n
         * In order to adapt the RigidBody class to a particular algorithm, the
         * base template needs to be specialized.
         */
        template <typename C>  // Type of the configuration
        class RigidBodyPrimitiveTrait {
         public:
          //**Type definitions**************************************************
          using AABB =
              BoundingBox<real_t>;  //!< Type of the axis-aligned bounding box.
          //********************************************************************

         protected:
          //**Constructor*********************************************************************************
          /*!\name Constructor */
          //@{
          explicit inline RigidBodyPrimitiveTrait(BodyID body);
          //@}
          //**********************************************************************************************

          //**Member
          // variables****************************************************************************
          /*!\name Member variables */
          //@{
          AABB aabb_;  //!< Axis-aligned bounding box for the rigid body.
                       //@}
          //**********************************************************************************************
        };
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Default implementation of the RigidBodyPrimitiveTrait
         * constructor.
         *
         * \param body The rigid body containing this bounding box.
         */
        template <typename C>  // Type of the configuration
        inline RigidBodyPrimitiveTrait<C>::RigidBodyPrimitiveTrait(
            BodyID /*body*/)
            : aabb_()  // Axis-aligned bounding box for the rigid body
        {}
        //*************************************************************************************************

        //=================================================================================================
        //
        //  SPECIALIZATION FOR THE HIERARCHICAL HASH GRID ALGORITHM
        //
        //=================================================================================================

        //*************************************************************************************************
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
        class RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR> > {
         public:
          //**Type definitions**************************************************
          using AABB =
              BoundingBox<real_t>;  //!< Type of the axis-aligned bounding box.
          //********************************************************************

         protected:
          //**Constructor*******************************************************
          /*!\name Constructor */
          //@{
          explicit inline RigidBodyPrimitiveTrait(BodyID body);
          //@}
          //********************************************************************

         public:
          //**Get
          // functions***********************************************************
          /*!\name Get functions */
          //@{
          inline real_t get_AABB_size() const;
          inline void* get_grid() const;
          inline std::size_t get_hash() const;
          inline std::size_t get_cell_ID() const;
          //@}
          //********************************************************************

          //**Set
          // functions***********************************************************
          /*!\name Set functions */
          //@{
          inline void set_grid(void* grid);
          inline void set_hash(std::size_t hash);
          inline void set_cell_ID(std::size_t cell);
          //@}
          //********************************************************************

         protected:
          //**Member
          // variables***********************************************************
          /*!\name Member variables */
          //@{
          AABB aabb_;         //!< Axis-aligned bounding box for the rigid body.
          void* grid_;        //!< Pointer to the hash grid this rigid body is
                              //!< currently assigned to.
          std::size_t hash_;  //!< Current hash value of this rigid body.
          std::size_t
              cell_ID_;  //!< The body's index in the body container of the grid
                         //!< cell this rigid body is currently assigned to.
                         //@}
          //********************************************************************
        };
        //**********************************************************************

        //*************************************************************************************************
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
            C<HashGrids, FD, BG, CR> >::RigidBodyPrimitiveTrait(BodyID /*body*/)
            : aabb_(),     // Axis-aligned bounding box for the rigid body
              grid_(0),    // Pointer to the hash grid this rigid body is
                           // currently assigned to
              hash_(0),    // Current hash value of this rigid body
              cell_ID_(0)  // The body's index in the body container of the grid
                           // cell this rigid body is currently assigned to
        {}
        //**********************************************************************

        //*************************************************************************************************
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
            C<HashGrids, FD, BG, CR> >::get_AABB_size() const {
          real_t size = aabb_[3] - aabb_[0];
          real_t yDiff = aabb_[4] - aabb_[1];
          real_t zDiff = aabb_[5] - aabb_[2];

          if (yDiff > size)
            size = yDiff;
          if (zDiff > size)
            size = zDiff;

          return size;
        }
        //*************************************************************************************************

        //*************************************************************************************************
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
        inline void*
        RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR> >::get_grid() const {
          return grid_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
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
        inline std::size_t
        RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR> >::get_hash() const {
          return hash_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
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
            C<HashGrids, FD, BG, CR> >::get_cell_ID() const {
          return cell_ID_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Setting the grid the rigid body is associated with.
         *
         * \param grid The grid the rigid body is assigned to.
         * \return void
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
        RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR> >::set_grid(
            void* grid) {
          grid_ = grid;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Setting the hash value of the rigid body.
         *
         * \param hash The new hash value of the rigid body.
         * \return void
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
        RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR> >::set_hash(
            std::size_t hash) {
          hash_ = hash;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Setting the body container index within the cell the body is
         * currently assigned to.
         *
         * \param cell The new body container index.
         * \return void
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
        RigidBodyPrimitiveTrait<C<HashGrids, FD, BG, CR> >::set_cell_ID(
            std::size_t cell) {
          cell_ID_ = cell;
        }
        //*************************************************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
