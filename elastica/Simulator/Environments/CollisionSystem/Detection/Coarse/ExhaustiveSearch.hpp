//=================================================================================================
/*!
 *  \file pe/core/detection/coarse/ExhaustiveSearch.h
 *  \brief Implementation of the 'Exhaustive Search' coarse collision detection
 * algorithm
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

#include <type_traits>

//#include <pe/core/rigidbody/BodyStorage.h>
//#include <pe/util/NonCopyable.h>
//#include <pe/util/constraints/SameType.h>
//#include <pe/util/logging/DebugSection.h>

#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/NonCopyable.hpp"
#include "elastica/Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //=================================================================================================
        //
        //  CLASS DEFINITION
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Implementation of the 'Exhaustive Search' coarse collision
         * detection algorithm. \ingroup coarse_collision_detection
         *
         * The ExhaustiveSearch class represents the implementation of the
         * 'Exhaustive Search' coarse collision detection algorithm. This
         * algorithm tests for an overlap of the axis-aligned bounding boxes for
         * each pair of rigid bodies and performs a fine collision detection in
         * case the bounding boxes overlap. Since every pair of rigid bodies is
         * handled, the algorithm has a complexity of \f$ O(n^2) \f$. Although
         * the complexity is worse than the complexity of other algorithms
         * (compared to for example the SweepAndPrune algorithm), the
         * ExhaustiveSearch algorithm performs very good in situations where
         * only a small number of rigid bodies is required. This is because of
         * no additional data management overhead.
         */
        template <typename C>  // Type of the configuration
        class ExhaustiveSearch : private NonCopyable {
         public:
          //**Type
          // definitions****************************************************************************
          using EnhancedCollisionConfig = C;  //!< Type of the configuration.
          using This =
              ExhaustiveSearch<EnhancedCollisionConfig>;  //!< Type of this
                                                          //!< ExhaustiveSearch
                                                          //!< instance.

          //          using Primtives = typename EnhancedCollisionConfig
          //          ::Primitives;

          // Matrix types
          using PrimitivePrimitiveMatrix =
              typename EnhancedCollisionConfig::PrimitivePrimitiveMatrix;
          using PrimitiveImposedPrimitiveMatrix =
              typename EnhancedCollisionConfig::PrimitiveImposedPrimitiveMatrix;

          //!< Type of the primitive storage.
          using PrimitiveStorage =
              typename EnhancedCollisionConfig::PrimitiveStorage;
          //!< Type of the imposed primitive storage.
          using ImposedPrimitiveStorage =
              typename EnhancedCollisionConfig::ImposedPrimitiveStorage;

          using BodyID =
              typename EnhancedCollisionConfig::BodyID;  //!< Handle for a rigid
                                                         //!< body.
          using FineDetector = typename EnhancedCollisionConfig::
              FineDetector;  //!< Type of the fine collision
                             //!< detector.
          //**********************************************************************************************

          //**Constructor*********************************************************************************
          /*!\name Constructor */
          //@{
          explicit ExhaustiveSearch(PrimitiveStorage&,
                                    ImposedPrimitiveStorage*&);
          //@}
          //**********************************************************************************************

          //**Add/remove
          // functions************************************************************************
          /*!\name Add/remove functions */
          //@{
          inline void add(BodyID body);
          inline void remove(BodyID body);
          //@}
          //**********************************************************************************************

          //**Utility
          // functions***************************************************************************
          /*!\name Utility functions */
          //@{
          template <typename Contacts>
          void find_contacts(Contacts& contacts);

          void clear();
          //@}
          //**********************************************************************************************

         private:
          template <typename Contacts, typename... ListOfCollidingPrimitives>
          void find_contacts(Contacts& contacts,
                             tmpl::list<ListOfCollidingPrimitives...>);
          template <typename Contacts, typename FCP, typename SCP>
          void find_contacts_impl(Contacts& contacts, tmpl::list<FCP, SCP>);
          template <typename Contacts,
                    typename... ListOfCollidingMovingAndImposedPrimitives>
          void find_contacts_between_moving_and_imposed_primtives(
              Contacts& contacts,
              tmpl::list<ListOfCollidingMovingAndImposedPrimitives...>);
          template <typename Contacts, typename FCPS, typename SCPS>
          void find_contacts_between_moving_and_imposed_primtives_impl(
              Contacts& contacts, tmpl::list<FCPS, SCPS>);
          // TODO : Noy symmetric the primitive and imposed primitive storages
          //        Make them symmeteric
          //**Member
          // variables****************************************************************************
          /*!\name Member variables */
          //@{
          // The lifetime of the referred variable will exceed the
          // CoarseDetectors lifetime Hence this operation is valid
          PrimitiveStorage&
              primitives_;  //!< Reference to the primitive storage.
          ImposedPrimitiveStorage*&
              imposed_primitives_;  //!< Pointer to the primitives that are
                                    //!< fixed as
          //!< the simulation progresses

          //@}
          //**********************************************************************************************

          //**Compile time checks***********************************************
          /*! \cond PE_INTERNAL */
          static_assert(
              std::is_same<
                  This, typename CollisionConfig::template CoarseDetectorFamily<
                            EnhancedCollisionConfig>>::value,
              "Should not be reached!");
          /*! \endcond */
          //********************************************************************
        };
        //*************************************************************************************************

        //=================================================================================================
        //
        //  CONSTRUCTOR
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Constructor for exhaustive search collision detector.
         *
         * \param bodystorage Reference to the general body storage.
         *
         * Note that all (local, global and remote) must be contained in the
         * body storage.
         */
        template <typename C>  // Type of the configuration
        ExhaustiveSearch<C>::ExhaustiveSearch(
            PrimitiveStorage& primitivestorage,
            ImposedPrimitiveStorage*& imposed_primitive_storage)
            : primitives_(primitivestorage),
              imposed_primitives_(imposed_primitive_storage) {}
        //*************************************************************************************************

        //=================================================================================================
        //
        //  ADD/REMOVE FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Registering a rigid body with the coarse collision detector.
         *
         * \param body The body to be registered with the coarse collision
         * detector. \return void
         *
         * This function is a requirement for all coarse collision detectors. It
         * is automatically called every time a new rigid body is added to the
         * simulation world.
         */
        template <typename C>  // Type of the configuration
        void ExhaustiveSearch<C>::add(BodyID /*body*/) {}
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Deregistering a rigid body from the coarse collision detector.
         *
         * \param body The body to be deregistered from the coarse collision
         * detector. \return void
         *
         * This function is a requirement for all coarse collision detectors. It
         * is automatically called every time a rigid body is removed from the
         * simulation world.
         */
        template <typename C>  // Type of the configuration
        void ExhaustiveSearch<C>::remove(BodyID /*body*/) {}
        //*************************************************************************************************

        //=================================================================================================
        //
        //  UTILITY FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Contact generation between colliding rigid bodies.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * This function generates all contacts between the rigid bodies
         * contained in the simulation world. The contacts are added to the
         * contact container \a contacts.
         */
        // TODO : Protocol check
        // NOTE : The routine is not const qualified because
        // FineDetector::collide stores pointers to the primitives involved in
        // the collision
        template <typename C>         // Type of the configuration
        template <typename Contacts,  // Contact container type
                  typename FCPS,  // Storage type of first colliding primitive
                  typename SCPS>  // Storage type of second colliding primitive
        void ExhaustiveSearch<C>::find_contacts_impl(
            Contacts& contacts, tmpl::list<FCPS, SCPS> /*meta*/) {
          // Colliding local rigid bodies
          auto& first_collection = std::get<FCPS>(primitives_);
          auto& second_collection = std::get<SCPS>(primitives_);

          // No const iterators
          auto first_collection_begin(first_collection.begin());
          auto first_collection_end(first_collection.end());

          // If they are of the same type, don't collide the object with itself
          constexpr int offset = std::is_same<FCPS, SCPS>::value;
          auto second_collection_begin(second_collection.begin() + offset);
          auto second_collection_end(second_collection.end());

          // TODO This should be in a parallel loop, but at what level?
          for (auto fb = first_collection_begin; fb != first_collection_end;
               ++fb) {
            for (auto sb = second_collection_begin; sb != second_collection_end;
                 ++sb) {
              if (
                  // Testing for overlapping bounding boxes, more probable
                  (fb->get_AABB().overlaps(sb->get_AABB())) &&
                  // Ignoring contacts between two fixed bodies
                  (!fb->is_fixed() || !sb->is_fixed()))
                FineDetector::collide(*fb, *sb, contacts);
            }
          }
        }
        //*************************************************************************************************

        // NOTE : The routine is not const qualified because find_contacts_impl
        // is not const qualified
        template <typename C>  // Type of the configuration
        template <typename Contacts, typename... ListOfCollidingPrimitives>
        inline void ExhaustiveSearch<C>::find_contacts(
            Contacts& contacts,
            tmpl::list<ListOfCollidingPrimitives...> /*meta*/) {
          // You can do parallel stuff here
          EXPAND_PACK_LEFT_TO_RIGHT(
              find_contacts_impl(contacts, ListOfCollidingPrimitives{}));
        }

        template <typename C>         // Type of the configuration
        template <typename Contacts,  // Contact container type
                  typename FCPS,  // Storage type of first colliding primitive
                  typename SCPS>  // Storage type of second colliding primitive
        void ExhaustiveSearch<C>::
            find_contacts_between_moving_and_imposed_primtives_impl(
                Contacts& contacts, tmpl::list<FCPS, SCPS> /*meta*/) {
          // Colliding local rigid bodies
          auto& first_collection = std::get<FCPS>(primitives_);
          // Get from collection of imposed primitives now
          auto& second_collection = std::get<SCPS>(*imposed_primitives_);

          // No const iterators
          auto first_collection_begin(first_collection.begin());
          auto first_collection_end(first_collection.end());

          // No need to offset pts because the collections are different
          auto second_collection_begin(second_collection.begin());
          auto second_collection_end(second_collection.end());

          // TODO This should be in a parallel loop, but at what level?
          for (auto fb = first_collection_begin; fb != first_collection_end;
               ++fb) {
            for (auto sb = second_collection_begin; sb != second_collection_end;
                 ++sb) {
              if (
                  // Testing for overlapping bounding boxes, more probable
                  (fb->get_AABB().overlaps(sb->get_AABB())) &&
                  // Ignoring contacts between two fixed bodies
                  (!fb->is_fixed() || !sb->is_fixed()))
                FineDetector::collide(*fb, *sb, contacts);
            }
          }
        }

        template <typename C>  // Type of the configuration
        template <typename Contacts,
                  typename... ListOfCollidingMovingAndImposedPrimitives>
        inline void
        ExhaustiveSearch<C>::find_contacts_between_moving_and_imposed_primtives(
            Contacts& contacts,
            tmpl::list<ListOfCollidingMovingAndImposedPrimitives...> /*meta*/) {
          // You can do parallel stuff here
          EXPAND_PACK_LEFT_TO_RIGHT(
              find_contacts_between_moving_and_imposed_primtives_impl(
                  contacts, ListOfCollidingMovingAndImposedPrimitives{}));
        }

        //*************************************************************************************************
        /*!\brief Contact generation between colliding rigid bodies.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * This function generates all contacts between the rigid bodies
         * contained in the simulation world. The contacts are added to the
         * contact container \a contacts.
         */
        template <typename C>         // Type of the configuration
        template <typename Contacts>  // Contact container type
        inline void ExhaustiveSearch<C>::find_contacts(Contacts& contacts) {
          // Serial
          LOG_DEBUG_SECTION(log) {
            log << "   Finding the contacts via the exhaustive search "
                   "algorithm...";
          }

          // Cannot use lambda to expand private info
          find_contacts(contacts, PrimitivePrimitiveMatrix{});

          // Find contacts between active primitives and imposed primitives
          find_contacts_between_moving_and_imposed_primtives(
              contacts, PrimitiveImposedPrimitiveMatrix{});

          // Serial
          LOG_DEBUG_SECTION(log) {
            if (contacts.empty())
              log << "      no contacts found!";
            else {
              for (auto const& c : contacts) {
                log << "      State of the contacts:\n";
                log << "         Contact " << c->get_ID() << " (" << c->get_contact_type()
                    << "):"
                    << " pos = " << c->get_position()
                    << " , n = " << c->get_normal()
                    << " , dist = " << c->get_distance()
                    << " , vrel = " << c->get_normal_relative_velocity()
                    << " , cor = " << c->get_restitution() << "\n";
              }
            }
          }
        }

        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Clears the coarse collision detector.
         *
         * \return void
         */
        template <typename C>  // Type of the configuration
        void ExhaustiveSearch<C>::clear() {}
        //*************************************************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
