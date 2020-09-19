//==============================================================================
/*!
//  \file
//  \brief Transfer wizard
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <iterator>
#include <type_traits>

//// Declare all types
#include "Simulator/Environments/CollisionSystem/Transfer/Types.hpp"
////
#include "Simulator/Environments/CollisionSystem/Transfer/Transfer.hpp"
#include "Simulator/Environments/CollisionSystem/Transfer/TransferTraits.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Interface class for coordinating \c transfer operations
    // \ingroup transfer
    //
    // The TransferWizard template class helps correctly perform transfers
    // between a \system of an \elastica Simulator and a \primitive of the
    // \elastica collision library. It involves some compile-time type-checking
    // to ensure correct semantics for interfacing with the collision system. As
    // a result, all \c transfer operations are encouraged to be performed using
    // the wizard. As an example to implement transfer from SphereCollection to
    // a sequence of SpherePrimitive, one can use the TransferWizard in the
    // following way, using its static #transfer members.
    //
    // \example
       \code{.cpp}
           SphereCollection collection;
           std::vector<SpherePrimitive> primitives;
           TransferWizard<SphereTag>::transfer(from,
                                               collection,
                                               to,
                                               primitives.begin(),
                                               primitives.end());
       \endcode
    //
    // \see Transfer, TransferTraits
    //
    // \tparam SystemTag A valid \elastica system tag
    */
    template <typename SystemTag>  // A valid system tag
    struct TransferWizard : public TransferTraits<SystemTag> {
     public:
      //**Type definitions******************************************************
      using Parent = TransferTraits<SystemTag>;  //!< Parent type

      using typename Parent::from_t;  //!< From type of transfer
      using typename Parent::to_t;    //!< To type of transfer

      //! Type of system
      using typename Parent::SystemType;
      //! Type of contained primitive
      using typename Parent::PrimitiveType;
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Helper type trait for determining if the given \c Iterator type
      // is a const iterator
      //
      // \tparam Iterator
      // \note https://stackoverflow.com/a/10546077
      */
      template <typename Iterator>  // iterator type to be checked
      struct IsConstIterator {
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using value_type = typename std::remove_pointer<pointer>::type;
        static constexpr bool value = std::is_const<value_type>::value;
      };
      /*! \endcond */
      //************************************************************************

      //**Reset function********************************************************
      /*!\brief resets forces and torques
      //
      // Resets forces and torques for a range of primitives
      //
      // \param begin  Begin iterator of the primitive sequence
      // \param end    End iterator of the primitive sequence
      // \return void
      //
      // \tparam PrimitiveIterator Random access iterator for a primitive range
      */
      template <typename PrimitiveIterator>
      static void reset_forces_and_torques(PrimitiveIterator begin,
                                           PrimitiveIterator end) {
        std::for_each(begin, end, [](auto& primitive) -> void {
          primitive.reset_forces_and_torques();
        });
      }
      //************************************************************************

      //**Transfer function*****************************************************
      /*!\brief Transfers information from \systems to \primitives
      //
      // Interface for transferring information from a \system (collection) to a
      // range of \primitives
      //
      // \param collection  The \system collection
      // \param begin       Begin iterator of the \primitive sequence
      // \param end         End iterator of the \primitive sequence
      // \return void
      //
      // \tparam PrimitiveIterator Random access iterator for a primitive range
      */
      template <typename PrimitiveIterator>
      static void transfer(from_t /* meta */, SystemType const& collection,
                           to_t /* meta */, PrimitiveIterator begin,
                           PrimitiveIterator end) {
        using namespace tags;
        static_assert(std::is_same<PrimitiveType,
                                   typename std::iterator_traits<
                                       PrimitiveIterator>::value_type>::value,
                      "Wrong transfer detected!");

        // Reset previous time-steps effects
        reset_forces_and_torques(begin, end);

        // Generic implementation to fall back to, can use public members
        Transfer<SystemTag>::template do_transfer<PrimitiveIterator>(
            from, collection, to, begin, end);
      }
      //************************************************************************

      //**Transfer function*****************************************************
      /*!\brief Transfers information to \systems from \primitives
      //
      // Interface for transferring information to a \system (collection) from a
      // range of \primitives
      //
      // \param collection  The \system collection
      // \param begin       Begin iterator of the \primitive sequence
      // \param end         End iterator of the \primitive sequence
      // \return void
      //
      // \tparam ConstPrimitiveIterator Random access const iterator for a
      //                                primitive range
      */
      template <typename ConstPrimitiveIterator>
      static void transfer(to_t /* meta */, SystemType& collection,
                           from_t /* meta */, ConstPrimitiveIterator begin,
                           ConstPrimitiveIterator end) {
        using namespace tags;

        using IsPrimitiveIteratorConst =
            IsConstIterator<ConstPrimitiveIterator>;
        // gives a const P
        using value_type = typename IsPrimitiveIteratorConst::value_type;

        // Check first
        static_assert(IsPrimitiveIteratorConst::value,
                      "Const correctness failed!");
        static_assert(
            std::is_same<PrimitiveType,
                         typename std::remove_const<value_type>::type>::value,
            "Oops, wrong primitive!");

        Transfer<SystemTag>::template do_transfer<ConstPrimitiveIterator>(
            to, collection, from, begin, end);
      }
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
