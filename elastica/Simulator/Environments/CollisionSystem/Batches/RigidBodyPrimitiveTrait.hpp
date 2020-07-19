//=================================================================================================
/*!
 *  \file pe/core/batches/RigidBodyPrimitiveTrait.h
 *  \brief Rigid body customization class for the batch generation
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

#include "Types.hpp"
#include <cstddef>

namespace elastica {

  namespace collision {

    namespace batches {

      //=================================================================================================
      //
      //  ::elastica::batches NAMESPACE FORWARD DECLARATIONS
      //
      //=================================================================================================

      template <typename C>
      class ContactTrait;

      //=================================================================================================
      //
      //  CLASS DEFINITION
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Rigid body customization class for the batch generation.
       * \ingroup batch_generation
       *
       * The batch generation RigidBodyPrimitiveTrait class template is used to
       * adapt the RigidBody class to the used batch generation algorithm.
       * Depending on the used algorithm, a rigid body requires additional data
       * or functionality to efficiently support the batch generation process.\n
       * In order to adapt the RigidBody class to a particular algorithm, the
       * base template needs to be specialized. The following members are
       * expected for each specialization of the RigidBodyPrimitiveTrait class
       * template:
       *
       *  - the 'resetNode()' function
       */
      template <typename C>  // Type of the configuration
      class RigidBodyPrimitiveTrait {
       protected:
        //**Node
        // functions******************************************************************************
        /*!\name Node functions */
        //@{
        inline void resetNode() const;
        //@}
        //**********************************************************************************************
      };
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Resetting the data members of the node.
       *
       * \return void
       *
       * Default implementation of the resetNode() function.
       */
      template <typename C>  // Type of the configuration
      inline void RigidBodyPrimitiveTrait<C>::resetNode() const {}
      //*************************************************************************************************

      //=================================================================================================
      //
      //  SPECIALIZATION FOR THE UNION FIND ALGORITHM
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Specialization of the BodyTrait class template for the 'Union
       * Find' algorithm. \ingroup batch_generation
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
      class RigidBodyPrimitiveTrait<C<CD, FD, UnionFind, CR> > {
       public:
        //**Type
        // definitions****************************************************************************
        using CollisionConfig =
            C<CD, FD, UnionFind, CR>;  //!< Type of the actual configuration.
        using Node =
            RigidBodyPrimitiveTrait<CollisionConfig>;  //!< Type of this
                                                       //!< BodyTrait
                                                       //!< specialization.
        typedef Node* NodeID;  //!< Handle to a BodyTrait instance.
        typedef const Node*
            ConstNodeID;  //!< Handle to a constant BodyTrait instance.
        //**********************************************************************************************

        //**Constructor*********************************************************************************
        /*!\name Constructor */
        //@{
        explicit inline RigidBodyPrimitiveTrait();
        //@}
        //**********************************************************************************************

        //**Node
        // functions******************************************************************************
        /*!\name Node functions */
        //@{
        inline ConstNodeID getRoot() const;
        inline void resetNode() const;
        //@}
        //**********************************************************************************************

       private:
        //**Member
        // variables****************************************************************************
        /*!\name Member variables */
        //@{
        mutable ConstNodeID root_;  //!< The root of the contact graph
                                    //!< containing the rigid body.
        mutable ::std::size_t rank_;   //!< The current rank of the rigid body in the
                                //!< contact graph.
        mutable ::std::size_t batch_;  //!< Index of the batch containing all contacts
                                //!< in the contact graph.
        //@}
        //**********************************************************************************************

        //**Friend
        // declarations*************************************************************************
        /*! \cond PE_INTERNAL */
        friend class ContactTrait<CollisionConfig>;
        template <typename>
        friend class UnionFind;
        /*! \endcond */
        //**********************************************************************************************
      };
      //*************************************************************************************************

      //*************************************************************************************************
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
          C<CD, FD, UnionFind, CR> >::RigidBodyPrimitiveTrait()
          : root_(this), rank_(0), batch_(0) {}
      //*************************************************************************************************

      //*************************************************************************************************
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
          C<CD, FD, UnionFind, CR> >::ConstNodeID
      RigidBodyPrimitiveTrait<C<CD, FD, UnionFind, CR> >::getRoot() const {
        ConstNodeID root(this);

        // Traverse the contact graph to find the root node
        while (root->root_ != root) {
          root = root->root_;
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
        ConstNodeID tmp(this);
        while (tmp->root_ != tmp) {
          ConstNodeID last = tmp;
          tmp = tmp->root_;
          last->root_ = root;
        }

        return root;
      }
      //*************************************************************************************************

      //*************************************************************************************************
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
      inline void
      RigidBodyPrimitiveTrait<C<CD, FD, UnionFind, CR> >::resetNode() const {
        root_ = this;
        rank_ = 0;
        batch_ = 0;
      }
      //*************************************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
