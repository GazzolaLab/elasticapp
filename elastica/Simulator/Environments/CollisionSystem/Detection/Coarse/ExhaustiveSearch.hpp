//==============================================================================
/*!
//  \file
//  \brief Implementation of the 'Exhaustive Search' coarse collision detection
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

#include <type_traits>

//
#include "Simulator/Domain/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/BoundingBox.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/TypeTraits.hpp"
//
#include "Simulator/Environments/Primitives/PrimitivesCollection.hpp"
//
#include "Simulator/Environments/Transfer/TransferTags.hpp"
//
#include "Utilities/CountingIterator.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/Logging/Logger.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Implementation of the 'Exhaustive Search' coarse collision
         * detection algorithm.
         * \ingroup coarse_collision_detection
         *
         * \details
         * The ExhaustiveSearch class represents the implementation of the
         * 'Exhaustive Search' coarse collision detection algorithm. This
         * algorithm tests for an overlap of the axis-aligned bounding boxes for
         * each pair of rigid bodies and performs a fine collision detection in
         * case the bounding boxes overlap. Since every pair of rigid bodies is
         * handled, the algorithm has a complexity of \f$ O(n^2) \f$. Although
         * this complexity is worse than the complexity of other algorithms
         * (compared to, for example, the SweepAndPrune algorithm), the
         * ExhaustiveSearch algorithm performs very good in situations where
         * only a small number of rigid bodies is required. This is because of
         * no additional data management overhead.
         *
         * \tparam Config_ Type of the collision configuration (enhanced from
         * the collision module with system properties)
         */
        template <typename Config_>
        class ExhaustiveSearch : private NonCopyable {
         public:
          //**Type definitions**************************************************
          //! Type of the configuration
          using Config = Config_;
          //! Type of this ExhaustiveSearch instance
          using This = ExhaustiveSearch<Config>;
          //! Type of the admissible primitives
          using AdmissiblePrimitives = typename Config::AdmissiblePrimitives;
          //! Type of the primitive-imposed primitive collision matrix
          using PrimitiveImposedPrimitiveCombinations =
              typename Config::PrimitiveImposedPrimitiveCombinations;
          //! Combinations of primitive-primitive collision matrix
          using PrimitiveCombinations = typename Config::PrimitiveCombinations;
          //! Type of the simulation domain
          using DomainType = typename Config::DomainType;
          // TODO: informatino repeated from elsewher, fix?
          //! Execution policy for the fine detection module
          using Exec = typename Config_::KernelExecutionPolicy;
          //********************************************************************

         private:
          //**Compile time checks***********************************************
          /*! \cond ELASTICA_INTERNAL */
          static_assert(not std::is_same_v<DomainType, PeriodicDomain>,
                        R"error(
The exhaustive search algorithm cannot be used with a periodic domain. Consider
changing the coarse-detection algorithm.
)error");
          /*! \endcond */
          //********************************************************************

         public:
          //**Constructor*******************************************************
          /*!\name Constructor */
          //@{
          explicit ExhaustiveSearch() : domain_(nullptr){};
          explicit ExhaustiveSearch(const DomainType*);
          //@}
          //********************************************************************

          //**Add/remove functions**********************************************
          /*!\name Add/remove functions */
          //@{
          template <typename Simulator_>
          inline void add_all_primitives(::elastica::collision::tags::from_t,
                                         Simulator_ const&);
          //@}
          //********************************************************************

          //**Utility functions*************************************************
          /*!\name Utility functions */
          //@{
         public:
          template <typename PrimitivesCollection_,
                    typename ImposedPrimitivesCollection_,
                    typename ContactsCache_>
          void operator()(PrimitivesCollection_&, ImposedPrimitivesCollection_&,
                          ContactsCache_&);

          template <typename PrimitivesCollection_, typename ContactsCache_>
          void process(PrimitivesCollection_&, ContactsCache_&);

          template <typename PrimitivesCollection_,
                    typename ImposedPrimitivesCollection_,
                    typename ContactsCache_>
          void process(PrimitivesCollection_&, ImposedPrimitivesCollection_&,
                       ContactsCache_&);

         protected:
          template <typename FCP, typename SCP, typename Contacts>
          static void generate_contact(Contacts& contacts, FCP& first,
                                       SCP& second);

          template <typename FCPCollection, typename SCPCollection,
                    typename Contacts>
          void generate_contacts(Contacts& contacts, FCPCollection& collection,
                                 SCPCollection&, tt::SamePrimitivesTag);
          template <typename FCPCollection, typename SCPCollection,
                    typename Contacts>
          void generate_contacts(Contacts& contacts,
                                 FCPCollection& first_collection,
                                 SCPCollection& second_collection,
                                 tt::DifferentPrimitivesTag);

          template <typename FCPCollection, typename SCPCollection,
                    typename Contacts>
          inline auto generate_contacts(Contacts& contacts,
                                        FCPCollection& first_collection,
                                        SCPCollection& second_collection)
              -> void {
            using TagDispatch =
                tt::IsSamePrimitive<typename FCPCollection::value_type,
                                    typename SCPCollection::value_type>;
            generate_contacts(contacts, first_collection, second_collection,
                              TagDispatch{});
          }

         public:
          void clear();
          //@}
          //********************************************************************

          // fuck
          const DomainType*& domain() & noexcept { return domain_; }
          const DomainType* const& domain() const& noexcept { return domain_; }

         protected:
          //**Add/remove functions**********************************************
          /*!\name Add/remove functions */
          //@{
          template <typename PrimitiveType>
          inline void add(PrimitiveType* primitive);

          template <typename PrimitiveType>
          inline void remove(PrimitiveType* primitive);
          //@}
          //********************************************************************

         private:
          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          /*
           * dev note: The lifetime of the variable references here
           * will exceed the coarse detectors lifetime, so we can "safely"
           * store them
           */
          //! Reference to the domain.
          const DomainType* domain_;
          //@}
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  CONSTRUCTOR
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Constructor for exhaustive search collision detector.
         *
         * \param domain The domain of the simulator
         * \param primitive_storage The storage of primitives
         * \param ghosts NA
         * \param imposed_primitive_storage The storage for primitives that are
         * imposed (includes infinite objects as well)
         */
        template <typename C>  // Type of the configuration
        ExhaustiveSearch<C>::ExhaustiveSearch(const DomainType* domain)
            : domain_(domain) {}
        //**********************************************************************

        //======================================================================
        //
        //  ADD/REMOVE FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Adds all primitives to the coarse detector
         */
        template <typename C>  // Type of the configuration
        template <typename Simulator_>
        inline void ExhaustiveSearch<C>::add_all_primitives(
            ::elastica::collision::tags::from_t, Simulator_ const&){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Registering a rigid primitive with the coarse collision
         * detector.
         *
         * \param body The body to be registered with the coarse collision
         * detector.
         * \return void
         *
         * This function is a requirement for all coarse collision detectors. It
         * is automatically called every time a new rigid body is added to the
         * simulation world.
         */
        template <typename C>  // Type of the configuration
        template <typename PrimitiveType>
        void ExhaustiveSearch<C>::add(PrimitiveType* /*primitive*/) {}
        //**********************************************************************

        //**********************************************************************
        /*!\brief Deregistering a rigid body from the coarse collision detector.
         *
         * \param body The body to be deregistered from the coarse collision
         * detector.
         * \return void
         *
         * This function is a requirement for all coarse collision detectors. It
         * is automatically called every time a rigid body is removed from the
         * simulation world.
         */
        template <typename C>  // Type of the configuration
        template <typename PrimitiveType>
        void ExhaustiveSearch<C>::remove(PrimitiveType* /*primitive*/) {}
        //**********************************************************************

        //======================================================================
        //
        //  UTILITY FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Logic for contact generation between colliding primitives
         *
         * \details
         * This function is the customization point for changing the logic
         * as to how contacts are processed. Here, we can:
         * 1. Add potential contacts to a data buffer (current choice) or
         * 2. Run fine-contacts immediately, which populates another buffer
         *
         * Currently we prefer 1 as parallelization is straightforward, although
         * we may need to modify this in the future if real pipeline and data
         * parallelism are required (by putting it in a graph that processes
         * each primitive pairs at one time)
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>                  // Type of the configuration
        template <typename FCP,                // First colliding primitive
                  typename SCP,                // Second colliding primitive
                  typename ContactsContainer>  // Contact container type
        void ExhaustiveSearch<C>::generate_contact(ContactsContainer& contacts,
                                                   FCP& first, SCP& second) {
          if (
              // Testing for overlapping bounding boxes, more probable
              overlaps(first.get_AABB(), second.get_AABB())
              // Ignoring contacts between two fixed bodies
              // and (!fb->is_fixed() or !sb->is_fixed())
          )
            contacts.emplace_back(first, second);  // modify
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between colliding rigid primitives of the
         * same type
         *
         * \details
         * This function generates all contacts between rigid primitives of the
         * same type contained in the simulation world. The contacts are added
         * to the contact container \a contacts.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>                  // Type of the configuration
        template <typename FCCollection,       // First colliding primitive
                  typename SCCollection,       // Second colliding primitive
                  typename ContactsContainer>  // Contact container type
        void ExhaustiveSearch<C>::generate_contacts(
            ContactsContainer& contacts, FCCollection& collection,
            SCCollection& /*meta*/, tt::SamePrimitivesTag /* meta */
        ) {
          /* devNOTE : The routine is not const qualified because
           * FineDetector::collide stores pointers to the primitives involved in
           * the collision
           */
          static_assert(std::is_same_v<FCCollection, SCCollection>,
                        "Exhaustive-search Meta failure!");

          // Colliding local rigid bodies
          // auto& collection = get_collection<FCP>();
          const auto s = std::size(collection);

          // unfortunately here we need to do it over indices here
          // so using a counting iterator
          Exec::for_each(
              elastica::make_counting_iterator(0UL),
              elastica::make_counting_iterator(s),
              [&, s](std::size_t const outer_idx) {
                Exec::for_each(
                    elastica::make_counting_iterator(outer_idx + 1UL),
                    elastica::make_counting_iterator(s),
                    [&, outer_idx](std::size_t const inner_idx) {
                      //
                      generate_contact(contacts, collection[outer_idx],
                                       collection[inner_idx]);
                    });
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between colliding rigid primitives of
         * different types
         *
         * \details
         * This function generates all contacts between rigid primitives of
         * different type contained in the simulation world. The contacts
         * generated are added to the container \a contacts.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>              // Type of the configuration
        template <typename FCPCollection,  // First colliding primitive
                  typename SCPCollection,  // Second colliding primitive
                  typename Contacts>       // Contact container type
        void ExhaustiveSearch<C>::generate_contacts(
            Contacts& contacts, FCPCollection& first_primitive_collection,
            SCPCollection& second_primitive_collection,
            tt::DifferentPrimitivesTag /* meta */
        ) {
          /*
          static_assert(std::is_same_v<FCP, SCP>,
                        "Exhaustive-search Meta failure!");
          */
          // auto& first_primitive_collection = get_collection<FCP>();
          // auto& second_primitive_collection = get_collection<SCP>();
          using std::begin;
          using std::end;

          // TODO :
          // need a cartesian product iterator
          // or
          // use for_each_2D() or something similar to blocked_range_2D
          Exec::for_each(
              begin(first_primitive_collection),
              end(first_primitive_collection), [&](auto& first_primitive) {
                Exec::for_each(begin(second_primitive_collection),
                               end(second_primitive_collection),
                               [&](auto& second_primitive) {
                                 generate_contact(contacts, first_primitive,
                                                  second_primitive);
                               });
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a list of colliding primitives
         */
        template <typename C>  // Type of the configuration
        template <typename PrimitivesCollection_,
                  typename Contacts>  // Contact container type
        inline void ExhaustiveSearch<C>::process(
            PrimitivesCollection_& primitives_collection, Contacts& contacts) {
          Exec::template pairwise_for_each<PrimitiveCombinations>(
              [&](auto meta_p1, auto meta_p2) {
                using elastica::collision::get_collection;
                using P1 = tmpl::type_from<decltype(meta_p1)>;
                using P2 = tmpl::type_from<decltype(meta_p2)>;
                // auto& first_primitive_collection = get_collection<FCP>();
                // auto& second_primitive_collection = get_collection<SCP>();
                generate_contacts(contacts,
                                  get_collection<P1>(primitives_collection),
                                  get_collection<P2>(primitives_collection));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a primitive and imposed primitive
         */
        template <typename C>  // Type of the configuration
        template <typename PrimitivesCollection_,
                  typename ImposedPrimitivesCollection_,
                  typename Contacts>  // Contact container type
        inline void ExhaustiveSearch<C>::process(
            PrimitivesCollection_& primitives_collection,
            ImposedPrimitivesCollection_& imposed_primitives_collection,
            Contacts& contacts) {
          Exec::template pairwise_for_each<
              PrimitiveImposedPrimitiveCombinations>([&](auto meta_p1,
                                                         auto meta_p2) {
            using elastica::collision::get_collection;
            using P1 = tmpl::type_from<decltype(meta_p1)>;
            using P2 = tmpl::type_from<decltype(meta_p2)>;
            // These are always considered different as they have
            // different semantics, even if the type system says otherwise
            generate_contacts(contacts,
                              get_collection<P1>(primitives_collection),
                              get_collection<P2>(imposed_primitives_collection),
                              tt::DifferentPrimitivesTag{});
          });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between colliding rigid bodies.
         *
         * \details
         * This function generates all contacts between the rigid bodies
         * contained in the simulation world. The contacts are added to the
         * contact container \a contacts.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>  // Type of the configuration
        template <typename PrimitivesCollection_,
                  typename ImposedPrimitivesCollection_,
                  typename Contacts>  // Contact container type
        inline void ExhaustiveSearch<C>::operator()(
            PrimitivesCollection_& primitives_collection,
            ImposedPrimitivesCollection_& imposed_primitives_collection,
            Contacts& contacts) {
          process(primitives_collection, contacts);
          process(primitives_collection, imposed_primitives_collection,
                  contacts);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Clears the coarse collision detector.
         *
         * \return void
         */
        template <typename C>  // Type of the configuration
        void ExhaustiveSearch<C>::clear() {}
        //**********************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica

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
