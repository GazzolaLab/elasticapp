//=================================================================================================
/*!
 *  \file pe/core/batches/ContactTrait.h
 *  \brief Contact customization class for the batch generation
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

//*************************************************************************************************
// Includes
//*************************************************************************************************

//#include <pe/core/batches/BodyTrait.h>
//#include <pe/core/batches/Types.h>
//#include <pe/util/Types.h>

#include "elastica/Simulator/Environments/CollisionSystem/Batches/RigidBodyPrimitiveTrait.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Batches/Types.hpp"

namespace elastica {

  namespace collision {

    namespace batches {

      //=================================================================================================
      //
      //  CLASS DEFINITION
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Contact customization class for the batch generation.
       * \ingroup batch_generation
       *
       * The batch generation ContactTrait class template is used to adapt the
       * Contact class to the used batch generation algorithm. Depending on the
       * used algorithm, a contact requires additional data or functionality to
       * efficiently support the batch generation process.\n In order to adapt
       * the Contact class to a particular algorithm, the base template needs to
       * be specialized. The following members are expected for each
       * specialization of the ContactTrait class template:
       *
       *  - the 'mergeNodes()' function
       */
      template <typename C>  // Type of the configuration
      class ContactTrait {
       public:
        //**Type
        // definitions****************************************************************************
        using Node = RigidBodyPrimitiveTrait<C>;  //!< Type of the corresponding
                                                  //!< BodyTrait class.
        using NodeID = Node*;                     //!< Handle to a Node object.
        using ConstNodeID = const Node*;  //!< Handle to a constant Node object.
        using Edge = ContactTrait<C>;  //!< Type of this ContactTrait instance.
        //**********************************************************************************************

        //**Edge
        // functions******************************************************************************
        /*!\name Edge functions */
        //@{
        inline void mergeNodes(ConstNodeID node1, ConstNodeID node2) const;
        //@}
        //**********************************************************************************************
      };
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Merging two adjacent contact graphs.
       *
       * \param node1 The first contacting node.
       * \param node2 The second contacting node.
       * \return void
       *
       * Default implementation of the mergeNodes() function.
       */
      template <typename C>  // Type of the configuration
      inline void ContactTrait<C>::mergeNodes(ConstNodeID /*node1*/,
                                              ConstNodeID /*node2*/) const {}
      //*************************************************************************************************

      //=================================================================================================
      //
      //  SPECIALIZATION FOR THE UNION FIND ALGORITHM
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Specialization of the ContactTrait class template for the
       * UnionFind algorithm. \ingroup batch_generation
       *
       * This specialization of the ContactTrait class template adapts rigid
       * bodies to the 'Union Find' batch generation algorithm. In this
       * algorithm, rigid bodies act as nodes in a graph and contacts between
       * the rigid bodies act as edges.
       */
      template <
          // Type of the coarse collision detection algorithm
          template <typename> class CD,
          // Type of the fine collision detection algorithm
          typename FD,
          // Type of the collision response algorithm
          template <typename, typename, typename> class CR,
          template <
              // Template signature of the coarse collision detection algorithm
              template <typename> class,
              // Template signature of the fine collision detection algorithm
              typename,
              // Template signature of the batch generation algorithm
              template <typename> class,
              // Template signature of the collision response algorithm
              template <typename, typename, typename> class>
          class C>  // Type of the configuration
      struct ContactTrait<C<CD, FD, UnionFind, CR> > {
       public:
        //**Type
        // definitions****************************************************************************
        typedef C<CD, FD, UnionFind, CR>
            CollisionConfig;  //!< Type of the actual configuration.
        using Node = RigidBodyPrimitiveTrait<CollisionConfig>;  //!< Type of the corresponding
                                                  //!< BodyTrait class.
        using NodeID = Node*;                     //!< Handle to a Node object.
        using ConstNodeID = const Node*;  //!< Handle to a constant Node object.
        using Edge = ContactTrait<CollisionConfig>;  //!< Type of this ContactTrait instance.
        //**********************************************************************************************

        //**Edge
        // functions******************************************************************************
        /*!\name Edge functions */
        //@{
        inline void mergeNodes(ConstNodeID node1, ConstNodeID node2) const;
        //@}
        //**********************************************************************************************
      };
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Merging two adjacent contact graphs.
       *
       * \param node1 The first contacting node.
       * \param node2 The second contacting node.
       * \return void
       *
       * This function merges the two contact graphs containing \a node1 and \a
       * node2 into a single contact graph.
       */
      template <
          // Type of the coarse collision detection algorithm
          template <typename> class CD,
          // Type of the fine collision detection algorithm
          typename FD,
          // Type of the collision response algorithm
          template <typename, typename, typename> class CR,
          template <
              // Template signature of the coarse collision detection algorithm
              template <typename> class,
              // Template signature of the fine collision detection algorithm
              typename,
              // Template signature of the batch generation algorithm
              template <typename> class,
              // Template signature of the collision response algorithm
              template <typename, typename, typename> class>
          class C>  // Type of the configuration
      inline void ContactTrait<C<CD, FD, UnionFind, CR> >::mergeNodes(
          ConstNodeID node1, ConstNodeID node2) const {
        const ConstNodeID root1(node1->getRoot());
        const ConstNodeID root2(node2->getRoot());

        if (root1 == root2) {
          return;
        } else if (root1->rank_ > root2->rank_) {
          root2->root_ = root1;
        } else if (root2->rank_ > root1->rank_) {
          root1->root_ = root2;
        } else {
          root2->root_ = root1;
          ++root1->rank_;
        }
      }
      //*************************************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
