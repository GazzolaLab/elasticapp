//==============================================================================
/*!
//  \file
//  \brief Contact specialization for batch generation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
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
**/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/CollisionSystem/Batches/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"

namespace elastica {

  namespace collision {

    namespace batches {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Contact customization class for the batch generation.
       * \ingroup batch_generation
       *
       * The batch generation ContactTrait class template is used to
       * adapt the Contact class to the used fine collision detection
       * algorithm. Depending on the used algorithm, a contact requires
       * additional data or functionality to efficiently support the batch
       * generation process.
       *
       * In order to adapt the Contact class to a particular algorithm, the
       * base template needs to be specialized. For each specialization of the
       * ContactTrait class template, we need the following types
       * \snippet this expected_types
       * and the following functions
       * \snippet this expected_functions
       */
      template <typename C>  // Type of the configuration
      class ContactTrait {
       public:
        //**Type definitions****************************************************
        // [expected_types]
        //! Type of the corresponding RigidBodyPrimitiveTrait class
        using Node = RigidBodyPrimitiveTrait<C>;
        //! Handle to a Node object
        using NodeID = Node*;
        //! Handle to a constant Node object
        using ConstNodeID = const Node*;
        //! Type of this ContactTrait instance
        using Edge = ContactTrait<C>;
        // [expected_types]
        //**********************************************************************

        //**Edge functions******************************************************
        /*!\name Edge functions */
        //@{
        // [expected_functions]
        inline void merge_nodes(ConstNodeID node1,
                                ConstNodeID node2) const noexcept;
        // [expected_functions]
        //@}
        //**********************************************************************
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Merging two adjacent contact graphs.
       *
       * \param node1 The first contacting node.
       * \param node2 The second contacting node.
       * \return void
       *
       * Default implementation of the merge_nodes() function.
       */
      template <typename C>  // Type of the configuration
      inline void ContactTrait<C>::merge_nodes(
          ConstNodeID /*node1*/, ConstNodeID /*node2*/) const noexcept {}
      //************************************************************************

      //========================================================================
      //
      //  SPECIALIZATION FOR THE UNION FIND ALGORITHM
      //
      //========================================================================

      //************************************************************************
      /*!\brief Specialization of the ContactTrait class template for the
       * UnionFind algorithm.
       * \ingroup batch_generation
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
        //**Type definitions****************************************************
        //! Type of the actual configuration.
        using CollisionConfig_ = C<CD, FD, UnionFind, CR>;
        //! Type of the corresponding RigidBodyPrimitiveTrait class
        using Node = RigidBodyPrimitiveTrait<CollisionConfig_>;
        //! Handle to a Node object
        using NodeID = Node*;
        //! Handle to a constant Node object
        using ConstNodeID = const Node*;
        //! Type of this ContactTrait instance
        using Edge = ContactTrait<CollisionConfig_>;
        //**********************************************************************

        //**Edge functions******************************************************
        /*!\name Edge functions */
        //@{
        inline void merge_nodes(NodeID node1, NodeID node2) const noexcept;
        //@}
        //**********************************************************************
      };
      //************************************************************************

      //************************************************************************
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
      inline void ContactTrait<C<CD, FD, UnionFind, CR> >::merge_nodes(
          NodeID node1, NodeID node2) const noexcept {
        // convert pointer to const contact to simply a contact pointer
        auto root1(const_cast<NodeID>(node1->get_root()));
        auto root2(const_cast<NodeID>(node2->get_root()));

        const auto rank_of_root1(root1->rank());
        const auto rank_of_root2(root2->rank());

        if (root1 == root2) {
          return;
        } else if (rank_of_root1 > rank_of_root2) {
          root2->set_root(root1);
        } else if (rank_of_root2 > rank_of_root1) {
          root1->set_root(root2);
        } else {
          // equality give preference to root1
          root2->set_root(root1);
          root1->increment_rank();
        }
      }
      //************************************************************************

      //========================================================================
      //
      //  SPECIALIZATION FOR THE HYBRID BATCH ALGORITHM
      //
      //========================================================================

      //************************************************************************
      /*!\brief Specialization of the ContactTrait class template for the
       * HybridBatch algorithm.
       * \ingroup batch_generation
       *
       * This specialization of the ContactTrait class template adapts rigid
       * bodies to the 'Hybrid Batch' batch generation algorithm. In this
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
      struct ContactTrait<C<CD, FD, HybridBatch, CR> >
          : public ContactTrait<C<CD, FD, UnionFind, CR> > {
        /* The contact trait should be actually specialized. However, we don't
         * know at build-time what the parallel configuration would be. Only
         * during compile-time will we get configuration. So we just forward
         * and use the UnionFind ContactTrait.
         */
      };
      //************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
