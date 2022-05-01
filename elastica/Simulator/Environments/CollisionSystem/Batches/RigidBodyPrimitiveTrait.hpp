//==============================================================================
/*!
//  \file
//  \brief Customization of rigid-body class based on the batch generation
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

#include <atomic>
#include <cstddef>  // for size_t
//
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
      /*!\brief Rigid body customization class for the batch generation.
       * \ingroup batch_generation
       *
       * The batch generation RigidBodyPrimitiveTrait class template is used to
       * adapt the RigidBody class to the used batch generation algorithm.
       * Depending on the used algorithm, a rigid body requires additional data
       * or functionality to efficiently support the batch generation process.
       *
       * In order to adapt the RigidBody class to a particular algorithm, the
       * base template needs to be specialized. For each specialization of the
       * RigidBodyPrimitiveTrait class template, we need the following functions
       * \snippet this expected_functions
       */
      template <typename C>  // Type of the configuration
      class RigidBodyPrimitiveTrait {
       protected:
        //**Node functions******************************************************
        /*!\name Node functions */
        //@{
        // [expected_functions]
        inline void reset_node() const noexcept;
        // [expected_functions]
        //@}
        //**********************************************************************
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Resetting the data members of the node.
       *
       * \return void
       *
       * Default implementation of the reset_node() function.
       */
      template <typename C>  // Type of the configuration
      inline void RigidBodyPrimitiveTrait<C>::reset_node() const noexcept {}
      //************************************************************************

      //========================================================================
      //
      //  SPECIALIZATION FOR THE UNION FIND ALGORITHM
      //
      //========================================================================

      //************************************************************************
      /*!\brief Specialization of the BodyTrait class template for the 'Union
       * Find' algorithm.
       * \ingroup batch_generation
       *
       * This specialization of the BodyTrait class template adapts rigid bodies
       * to the 'Union Find' batch generation algorithm. In this algorithm,
       * rigid bodies act as nodes in a graph and contacts between the rigid
       * bodies act as edges.
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
      class RigidBodyPrimitiveTrait<C<CD, FD, UnionFind, CR>> {
       public:
        //**Type definitions****************************************************
        //! Type of the actual configuration.
        using CollisionConfig_ = C<CD, FD, UnionFind, CR>;
        //! This type
        using This = RigidBodyPrimitiveTrait<CollisionConfig_>;
        //! Type of the corresponding RigidBodyPrimitiveTrait class
        using Node = This;
        //! Handle to a Node object
        using NodeID = Node*;
        //! Handle to a constant Node object
        using ConstNodeID = Node const*;
        //! Thread safe handle to a constant Node object
        using ThreadSafeConstNodeID = std::atomic<ConstNodeID>;
        //! Thread Safe int
        using ThreadSafeInt = std::atomic<::std::size_t>;
        //! Batch type
        using Batch = std::size_t;
        //**********************************************************************

        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        explicit inline RigidBodyPrimitiveTrait();
        // cannot use other's properties since its root (the this pointer) may
        // no longer be valid
        RigidBodyPrimitiveTrait(RigidBodyPrimitiveTrait&&) noexcept
            : RigidBodyPrimitiveTrait() {}
        //@}
        //**********************************************************************

        //**Node functions******************************************************
        /*!\name Node functions */
        //@{
        inline ConstNodeID get_root() /*const*/ noexcept;
        inline void reset_node() /*const*/ noexcept;

        //**********************************************************************
        /*!\brief Set the root of the current node.
         */
        void set_root(ConstNodeID other) noexcept { root_.store(other); }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Increment rank of current node.
         */
        void increment_rank() noexcept { ++rank_; }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get rank of current node.
         */
        auto rank() const noexcept -> typename ThreadSafeInt::value_type {
          return rank_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Set batch of current node.
         */
        inline void set_batch(Batch value) noexcept { batch_ = value; };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get batch of current node.
         */
        auto batch() const noexcept -> Batch { return batch_; }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        /*!\name Member variables */
        //@{
        //! The root of the contact graph containing the rigid body
        ThreadSafeConstNodeID root_;
        //! The current rank of the rigid body in the contact graph
        ThreadSafeInt rank_;
        //! Index of the batch containing all contacts in the contact graph.
        Batch batch_;  // never accessed by multiple threads, so not atomic
        //@}
        //**********************************************************************

        /*rank needs to be atomic as we increment it every time we add a
         * contact*/

        //**Friend declarations*************************************************
        /*! \cond ELASTICA_INTERNAL */
        friend class ContactTrait<CollisionConfig>;
        template <typename>
        friend class UnionFind;
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Default constructor for the BodyTrait<UnionFind> specialization.
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
      inline RigidBodyPrimitiveTrait<
          C<CD, FD, UnionFind, CR>>::RigidBodyPrimitiveTrait()
          : root_(static_cast<ConstNodeID>(this)), rank_(0), batch_(0) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Calculating the current root of the contact graph.
       *
       * \return The current root of the contact graph.
       *
       * This function returns the current root of the contact graph and
       * compresses the graph for a fast access to the root node.
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
      inline typename RigidBodyPrimitiveTrait<
          C<CD, FD, UnionFind, CR>>::ConstNodeID
      RigidBodyPrimitiveTrait<C<CD, FD, UnionFind, CR>>::get_root()
          /*const*/ noexcept {
        // this is a thread-local variable, so it need not be atomic
        using X = ConstNodeID;
        X root(this);

        // Traverse the contact graph to find the root node
        while (root->root_.load() != root) {
          //          root.store(root.load()->root_.load());
          root = root->root_.load();
        }

        // Retraverse the graph for graph compression
        //
        /*
         A contact graph like this:
         ```dot
         digraph graph_root {
            rankdir=LR;
            size="8,5"
            node [shape = doublecircle]; Root;
            node [shape = circle];
            Node0 -> Node1 [ ];
            Node1 -> Node2 [ ];
            Node2 -> Root [ ];
         }
         ```
        becomes this:
        ```dot
        digraph graph_root {
            rankdir=LR;
            size="8,5"
            node [shape = doublecircle]; Root;
            node [shape = circle];
            Node0 -> Root [ ];
            Node1 -> Root [ ];
            Node2 -> Root [ ];
        }
        ```
         */
        /*
         * While the following compresses the graph there is no guarantee that
         * it is safe if accessed from two threads at the same time.
         * So we make the root_ atomic to force updates one-by-one
         */

        using Y = NodeID;
        Y tmp(this);
        while (tmp->root_.load() != tmp) {
          // auto last = tmp;
          // tmp = tmp->root_.load();
          // last->root_.store(root);
          tmp = const_cast<Y>(tmp->root_.exchange(root));
        }

        return root;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Resetting the data members of the node.
       *
       * \return void
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
      inline void RigidBodyPrimitiveTrait<
          C<CD, FD, UnionFind, CR>>::reset_node() /* const*/ noexcept {
        root_ = this;
        rank_ = 0;
        batch_ = 0;
      }
      //************************************************************************

      //========================================================================
      //
      //  SPECIALIZATION FOR THE HYBRID BATCH ALGORITHM
      //
      //========================================================================

      //************************************************************************
      /*!\brief Specialization of the BodyTrait class template for the 'Hybrid
       * Batch' algorithm.
       * \ingroup batch_generation
       *
       * This specialization of the BodyTrait class template adapts rigid bodies
       * to the 'Hybrid Batching' batch generation algorithm. In this algorithm,
       * rigid bodies act as nodes in a graph and contacts between the rigid
       * bodies act as edges.
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
      class RigidBodyPrimitiveTrait<C<CD, FD, HybridBatch, CR>>
          : public RigidBodyPrimitiveTrait<C<CD, FD, UnionFind, CR>> {};
      //************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
