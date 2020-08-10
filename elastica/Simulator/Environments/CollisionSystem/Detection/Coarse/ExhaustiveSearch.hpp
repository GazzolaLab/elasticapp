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

#include "elastica/Simulator/Domain/Types.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/Tags.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/TypeTraits.hpp"
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

          /*
          // For the primitive - imposed primitive matrix
          // don't check only flips, also check for primitive ordering
          // TODO : FIX this, don't think its possible to fix too
          // WARNING : Not correct always
          // Only gives correct answers when both the lists being
          // cartesian product are same. In case one list is different
          // and we don't there exists a (E1, E2) pair but we want (E2, E1)
          // this doesn't even put it in the list and proceeds to compile
          template <typename S, typename E>
          struct OrderedCartesianHelper;

          template <typename S, typename E1, typename E2>
          struct OrderedCartesianHelper<S, tmpl::list<E1, E2>> {
            // Don't use conjunction, causes issue
            using type = typename std::conditional<
                // If reverse exists, don't bother
                std::is_same<tmpl::index_of<S, tmpl::list<E2, E1>>,
                             tmpl::no_such_type_>::value and
                    // If not in correct ordering don't bother
                    tmpl::less_equal<
                        tmpl::index_of<PrimitiveOrdering, E1>,
                        tmpl::index_of<PrimitiveOrdering, E2>>::value,
                tmpl::push_back<S, tmpl::list<E1, E2>>, S>::type;
          };
          */

          // SEE WARNING Above : Fix by having imposed primtiives always
          // trailing admissible primitives or making both equal and having
          // empty vectors
          //      TypeDisplayer<OrderedImposedPrimitiveCollection> td;
          //          using LowerTriangularPrimitiveImposedPrimitiveMatrix =
          //          tmpl::fold<
          //              PrimitiveImposedPrimitiveMatrix, tmpl::list<>,
          //              RemoveRepeatedCartesianHelper<tmpl::_state,
          //              tmpl::_element>>;
          using AdmissiblePrimitives =
              typename EnhancedCollisionConfig::AdmissiblePrimitives;

          // Matrix types
          //!< Type of the primitive-primitive collision matrix.
          using PrimitivePrimitiveMatrix =
              typename EnhancedCollisionConfig::PrimitivePrimitiveMatrix;
          //!< Type of the primitive-imposed primitive collision matrix.
          using PrimitiveImposedPrimitiveMatrix =
              typename EnhancedCollisionConfig::PrimitiveImposedPrimitiveMatrix;
          //!< Lower triangular part of the primitive-primitive collision
          //!< matrix.
          using LowerTriangularPrimitivePrimitiveMatrix =
              typename EnhancedCollisionConfig::
                  LowerTriangularPrimitivePrimitiveMatrix;

          //!< Type of the primitive storage.
          using PrimitiveStorage =
              typename EnhancedCollisionConfig::PrimitiveStorage;
          //!< Type of the imposed primitive storage.
          using ImposedPrimitiveStorage =
              typename EnhancedCollisionConfig::ImposedPrimitiveStorage;

          //!< Alias to add storage type given the primitive type
          template <typename PrimitiveType>
          using AddPrimitiveStorageType = typename EnhancedCollisionConfig::
              template AddPrimitiveStorageType<PrimitiveType>;

          //!< Alias to add storage type given the imposed primitive type
          template <typename ImposedPrimitiveType>
          using AddImposedPrimitiveStorageType =
              typename EnhancedCollisionConfig::
                  template AddImposedPrimitiveStorageType<ImposedPrimitiveType>;

          using BodyID =
              typename EnhancedCollisionConfig::BodyID;  //!< Handle for a rigid
                                                         //!< body.
          using FineDetector = typename EnhancedCollisionConfig::
              FineDetector;  //!< Type of the fine collision
          //!< detector.

          //!< Type of the simulation domain
          using DomainType = typename EnhancedCollisionConfig::DomainType;
          //**********************************************************************************************

          //**Constructor*********************************************************************************
          /*!\name Constructor */
          //@{
          explicit ExhaustiveSearch(const DomainType*&, PrimitiveStorage&,
                                    ImposedPrimitiveStorage*&);
          //@}
          //**********************************************************************************************

          //**Add/remove functions**********************************************
          /*!\name Add/remove functions */
          //@{
          inline void add_all_primitives();
          //@}
          //********************************************************************

          //**Utility
          // functions***************************************************************************
          /*!\name Utility functions */
          //@{
          template <typename Contacts>
          void find_contacts(Contacts& contacts);

          template <typename Contacts, typename... ListOfCollidingPrimitives>
          void find_contacts(Contacts& contacts,
                             tmpl::list<ListOfCollidingPrimitives...>);

          template <typename Contacts,
                    typename... ListOfCollidingMovingAndImposedPrimitives>
          void find_contacts_with_imposed_primitives(
              Contacts& contacts,
              tmpl::list<ListOfCollidingMovingAndImposedPrimitives...>);

          void clear();
          //@}
          //**********************************************************************************************

         private:
          //**Add/remove functions**********************************************
          /*!\name Add/remove functions */
          //@{
          template <typename PrimitiveType>
          inline void add(PrimitiveType* primitive);

          template <typename PrimitiveType>
          inline void remove(PrimitiveType* primitive);
          //@}
          //********************************************************************

          //**Get functions*****************************************************
          /*!\name Get functions */
          //@{
          template <typename PrimitiveType>
          inline constexpr auto get_collection() noexcept
              -> AddPrimitiveStorageType<PrimitiveType>&;

          template <typename ImposedPrimitiveType>
          inline constexpr auto get_imposed_collection() noexcept
              -> AddImposedPrimitiveStorageType<ImposedPrimitiveType>&;
          //@}
          //********************************************************************

          template <typename Contacts, typename FCPS, typename SCPS>
          void find_contacts_impl(Contacts& contacts, tmpl::list<FCPS, SCPS>,
                                  tags::SamePrimitivesTag);
          template <typename Contacts, typename FCPS, typename SCPS>
          void find_contacts_impl(Contacts& contacts, tmpl::list<FCPS, SCPS>,
                                  tags::DifferentPrimitivesTag);
          template <typename Contacts, typename FCPS, typename SCPS>
          void find_contacts_with_imposed_primitives_impl(
              Contacts& contacts, tmpl::list<FCPS, SCPS>);
          // TODO : Noy symmetric the primitive and imposed primitive storages
          //        Make them symmeteric
          //**Member
          // variables****************************************************************************
          /*!\name Member variables */
          //@{
          // The lifetime of the referred variable will exceed the
          // CoarseDetectors lifetime Hence this operation is valid
          const DomainType*& domain_;
          PrimitiveStorage&
              primitives_;  //!< Reference to the primitive storage.
          ImposedPrimitiveStorage*&
              imposed_primitives_;  //!< Pointer to the primitives that are
                                    //!< fixed as
          //!< the simulation progresses

          //@}
          //**********************************************************************************************

          //**Compile time checks***********************************************
          /*! \cond ELASTICA_INTERNAL */

          static_assert(
              not std::is_same<DomainType, PeriodicDomain>::value,
              "The exhaustive search algorithm cannot be used with a periodic "
              "domain. Consider changing the fine-detection algorithm.");

          // TODO : How to include Exhaustive search as a member of HashGrids
          // while retaining the facility to check?
          //          static_assert(
          //              std::is_same<
          //                  This, typename CollisionConfig::template
          //                  CoarseDetectorFamily<
          //                            EnhancedCollisionConfig>>::value,
          //              "Should not be reached!");
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
            const DomainType*& domain, PrimitiveStorage& primitivestorage,
            ImposedPrimitiveStorage*& imposed_primitive_storage)
            : domain_(domain),
              primitives_(primitivestorage),
              imposed_primitives_(imposed_primitive_storage) {}
        //*************************************************************************************************

        //=================================================================================================
        //
        //  ADD/REMOVE FUNCTIONS
        //
        //=================================================================================================

        template <typename C>  // Type of the configuration
        inline void ExhaustiveSearch<C>::add_all_primitives() {}

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
        template <typename PrimitiveType>
        void ExhaustiveSearch<C>::add(PrimitiveType* /*primitive*/) {}
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
        template <typename PrimitiveType>
        void ExhaustiveSearch<C>::remove(PrimitiveType* /*primitive*/) {}
        //*************************************************************************************************

        //=================================================================================================
        //
        //  UTILITY FUNCTIONS
        //
        //=================================================================================================

        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        inline constexpr auto ExhaustiveSearch<C>::get_collection() noexcept
            -> AddPrimitiveStorageType<PrimitiveType>& {
          using PrimitiveStorageType = AddPrimitiveStorageType<PrimitiveType>;
          return std::get<PrimitiveStorageType>(primitives_);
        }

        template <typename C>  // Type of the configuration
        template <
            typename ImposedPrimitiveType>  // Type of the imposed primitive
        inline constexpr auto
        ExhaustiveSearch<C>::get_imposed_collection() noexcept
            -> AddImposedPrimitiveStorageType<ImposedPrimitiveType>& {
          using ImposedPrimitiveStorageType =
              AddImposedPrimitiveStorageType<ImposedPrimitiveType>;
          return std::get<ImposedPrimitiveStorageType>(*imposed_primitives_);
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
        // TODO : Protocol check
        // Overload for same primtive collision
        // NOTE : The routine is not const qualified because
        // FineDetector::collide stores pointers to the primitives involved in
        // the collision
        template <typename C>         // Type of the configuration
        template <typename Contacts,  // Contact container type
                  typename FCP,       // First colliding primitive
                  typename SCP>       // Second colliding primitive
        void ExhaustiveSearch<C>::find_contacts_impl(
            Contacts& contacts, tmpl::list<FCP, SCP> /*meta*/,
            tags::SamePrimitivesTag /* meta */
        ) {
          static_assert(std::is_same<FCP, SCP>::value, "Oopsie!");

          // Colliding local rigid bodies
          auto& first_collection = get_collection<FCP>();

          // No const iterators
          auto first_collection_begin(first_collection.begin());
          auto first_collection_end(first_collection.end());

          // TODO This should be in a parallel loop, but at what level?
          for (auto fb = first_collection_begin; fb != first_collection_end;
               ++fb) {
            for (auto sb = fb + 1; sb != first_collection_end; ++sb) {
              if (
                  // Testing for overlapping bounding boxes, more probable
                  (fb->get_AABB().overlaps(sb->get_AABB())) and
                  // Ignoring contacts between two fixed bodies
                  (!fb->is_fixed() or !sb->is_fixed()))
                FineDetector::collide(*fb, *sb, contacts);
            }
          }
        }
        //*************************************************************************************************

        // Overload for different primitive collision
        template <typename C>         // Type of the configuration
        template <typename Contacts,  // Contact container type
                  typename FCP,       // First colliding primitive
                  typename SCP>       // Second colliding primitive
        void ExhaustiveSearch<C>::find_contacts_impl(
            Contacts& contacts, tmpl::list<FCP, SCP> /*meta*/,
            tags::DifferentPrimitivesTag /* meta */
        ) {
          static_assert(!std::is_same<FCP, SCP>::value, "Oopsie!");

          // Colliding local rigid bodies
          auto& first_collection = get_collection<FCP>();
          auto& second_collection = get_collection<SCP>();

          // No const iterators
          auto first_collection_begin(first_collection.begin());
          auto first_collection_end(first_collection.end());

          auto second_collection_begin(second_collection.begin());
          auto second_collection_end(second_collection.end());

          // TODO This should be in a parallel loop, but at what level?
          for (auto fb = first_collection_begin; fb != first_collection_end;
               ++fb) {
            for (auto sb = second_collection_begin; sb != second_collection_end;
                 ++sb) {
              if (
                  // Testing for overlapping bounding boxes, more probable
                  (fb->get_AABB().overlaps(sb->get_AABB())) and
                  // Ignoring contacts between two fixed bodies
                  (!fb->is_fixed() or !sb->is_fixed()))
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
          /*
           TODO : here we can do a gather operation
           currently for each and every primitive pair we launch a loop
           but rather we can take the first primitive and launch one
           loop which does the processing for all its pairs
           The roadblock is that the tag dispatch mechanism won't work
           effectively which means we have to split into unique
           e.g. say we have

           tmpl::list<tmpl::list<Sphere, Sphere>,
                      tmpl::list<Sphere, Cylinder>,
                      tmpl::list<Sphere, Box>>

           Then to process, we split into two lists

           1) tmpl::list<tmpl::list<Sphere, Sphere>>

           For this we call the same primitive loop like we already do

           2) tmpl::list<tmpl::list<Sphere, Cylinder>, tmpl::list<Sphere, Box>>
           For this we can modify the loop, extract the second primitives and
           call it like so:

           auto fb = first_collection_begin
           auto reducer = [&fb](auto Is){
                // Get second collection
                // Loop over second collection using sb iterator
                collide(*fb, *sb);
           };

            for (; fb != first_collection_end;
                 ++fb) {
                 index_apply<n_second_primitives>([&reducer](auto ...Is){
                  EXPAND_PACK(reducer(Is));
                 })
            }

          */

          /*
           using FCP = tmpl::at_c<Primitives, Is>;
           using SecondPrimitives = tmpl::fold<
               LowerTriangularPrimitivePrimitiveMatrix, tmpl::list<>,
               typename tt::ExtractSecondPrimitive<FCP>::
                   template apply<tmpl::_state, tmpl::_element>>;
          */

          // You can do parallel stuff here
          EXPAND_PACK_LEFT_TO_RIGHT(find_contacts_impl(
              contacts, ListOfCollidingPrimitives{},
              tt::PrimitivesDispatchType<ListOfCollidingPrimitives>{}));
        }

        template <typename C>         // Type of the configuration
        template <typename Contacts,  // Contact container type
                  typename FCP,       // First colliding primitive
                  typename SCP>       // Second colliding primitive
        void ExhaustiveSearch<C>::find_contacts_with_imposed_primitives_impl(
            Contacts& contacts, tmpl::list<FCP, SCP> /*meta*/) {
          // Colliding local rigid bodies
          auto& first_collection = get_collection<FCP>();
          // Get from collection of imposed primitives now
          auto& second_collection = get_imposed_collection<SCP>();

          // No const iterators
          auto first_collection_begin(first_collection.begin());
          auto first_collection_end(first_collection.end());

          // No need to offset because the collections are different
          auto second_collection_begin(second_collection.begin());
          auto second_collection_end(second_collection.end());

          // TODO This should be in a parallel loop, but at what level?
          for (auto fb = first_collection_begin; fb != first_collection_end;
               ++fb) {
            for (auto sb = second_collection_begin; sb != second_collection_end;
                 ++sb) {
              if (
                  // Testing for overlapping bounding boxes, more probable
                  (fb->get_AABB().overlaps(sb->get_AABB())) and
                  // Ignoring contacts between two fixed bodies
                  (!fb->is_fixed() or !sb->is_fixed()))
                FineDetector::collide(*fb, *sb, contacts);
            }
          }
        }

        template <typename C>  // Type of the configuration
        template <typename Contacts,
                  typename... ListOfCollidingMovingAndImposedPrimitives>
        inline void ExhaustiveSearch<C>::find_contacts_with_imposed_primitives(
            Contacts& contacts,
            tmpl::list<ListOfCollidingMovingAndImposedPrimitives...> /*meta*/) {
          // You can do parallel stuff here
          EXPAND_PACK_LEFT_TO_RIGHT(find_contacts_with_imposed_primitives_impl(
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
          find_contacts(contacts, LowerTriangularPrimitivePrimitiveMatrix{});

          // Find contacts between active primitives and imposed primitives
          find_contacts_with_imposed_primitives(
              contacts, PrimitiveImposedPrimitiveMatrix{});

          // Serial
          LOG_DEBUG_SECTION(log) {
            if (contacts.empty())
              log << "      no contacts found!";
            else {
              for (auto const& c : contacts) {
                log << "      State of the contacts:\n";
                log << "         Contact " << c->get_ID() << " ("
                    << c->get_contact_type() << "):"
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
