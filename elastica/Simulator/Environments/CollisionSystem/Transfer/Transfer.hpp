//==============================================================================
/*!
//  \file
//  \brief Transfer implementation
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

#include <algorithm>

//// Declare all types
#include "Simulator/Environments/CollisionSystem/Transfer/Types.hpp"
////
#include "ErrorHandling/Assert.hpp"
#include "Simulator/Environments/CollisionSystem/Transfer/TransferTraits.hpp"
#include "Simulator/Environments/CollisionSystem/Transfer/TypeTraits.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    // Provide generic implementation here
    //**************************************************************************
    /*!\brief Implements \c transfer operations
    // \ingroup transfer
    //
    // The Transfer template class implements transfer operations between a
    // \system of an \elastica Simulator and a \primitive of the
    // \elastica collision library. The Transfer template class is designed to
    // be used by the TransferWizard to orchestrate the transfer. It is the main
    // customization point for implementing customized transfer operations
    // between the \system and its
    // \primitive. If no customized implementation is provided, \elastica falls
    // back to the operations implemented by this default (generic) Transfer
    // class. For this default implementation to be valid, the \c SystemTag
    // contents needs to satisfy the constraints expected by
    // IsSystemDefaultTransferable and IsPrimitiveDefaultTransferable, else a
    // hard compiler error is generated.
    //
    // For examples to implement transfer from SphereCollection to
    // a sequence of SpherePrimitive, see TransferWizard.
    // In the above example, the following operation is used internally.
    //
    // \example
       \code
           // expanded from TransferWizard<SphereTag>::transfer(...)
           Transfer<SphereTag>::do_transfer(from,
                                            collection,
                                            to,
                                            begin,
                                            end);
       \endcode
    //
    // \n \section transfer_customization Transfer customization
    //
    // As seen in the example above, the Transfer class's #do_transfer function
    // needs to be defined. Typically you can do this in the translation unit
    // where the SystemTag template type is defined---this way the OCP from the
    // SOLID design principles is enforced. As an example of implementing
    // Transfer for a custom tag, consider the following example
    //
    // \example
       \code
           //! the include
           #include "Simulator/Environments/Transfer.hpp"

           struct MyBodyCollectionTag{
               //! user-defined body type
               using SystemType = MyBodyCollection;
               //! Primitive from collision library
               using PrimitiveType = collision::SpherePrimitive;
           };

           //! customization of Transfer to MyBodyCollectionTag
           template <>
           struct Transfer<MyBodyCollectionTag>
           //! Derive from TransferTraits to gain access to convenient typedefs
           : public TransferTraits<MyBodyCollectionTag>{

               //! Implement both ways transfer
               //! From system to primitive
               template <typename PrimIter>
               static void
               do_transfer( from_t,
                            SystemType const& collection,
                            to_t ,
                            PrimIter start,
                            PrimIter end) {
               // Fill in implementation details
               };

               //! From primitives to system
               template <typename ConstPrimIter>
               static void
               do_transfer( to_t,
                            SystemType & collection,
                            from_t ,
                            ConstPrimIter start,
                            ConstPrimIter end) {
              // Fill in implementation details
              };
       \endcode
    //
    // Notice that in this example one has access to aliases \c from_t, \c to_t
    // and \c SystemType by deriving from TransferTraits. For a complete
    // production-code example of such a customization, see SphereCollectionTag
    // specialization of Transfer in SphereCollectionTag.hpp
    //
    // \see TransferWizard, TransferTraits
    //
    // \tparam SystemTag A valid \elastica system tag
    */
    template <typename SystemTag>  // A valid system tag
    struct Transfer : public TransferTraits<SystemTag> {
     public:
      //**Type definitions******************************************************
      using Parent = TransferTraits<SystemTag>;  //!< Parent type

      using typename Parent::from_t;  //!< From type of transfer
      using typename Parent::to_t;    //!< To type of transfer

      using typename Parent::SystemType; //!< Type of system
      using typename Parent::PrimitiveType; //!< Type of contained primitive
      //************************************************************************

      //**Compile-time checks***************************************************
      static_assert(
          tt::IsSystemDefaultTransferable<SystemType>::value,
          "(Default) Transfer failed because the Collection fails to provide "
          "suitable members. Either provide all members necessary: "
          "-get_dimensions -"
          "get_position -get_velocity ... or "
          "provide a a specialization to the Transfer class");

      static_assert(
          tt::IsPrimitiveDefaultTransferable<PrimitiveType>::value,
          "(Default) Transfer failed because the primitive fails to provide "
          "suitable"
          "members. Either derive from RigidBodyPrimitive and provide a "
          "`transfer` function"
          "or provide a a specialization to the Transfer class");
      //************************************************************************

      //**Transfer function*****************************************************
      /*!\brief Transfers information from \systems to \primitives
      //
      // do_transfer implements information transfer from a \system
      // (collection) to a range of \primitives
      //
      // \param collection  The \system collection
      // \param begin       Begin iterator of the \primitive sequence
      // \param end         End iterator of the \primitive sequence
      // \return void
      //
      // \tparam PrimitiveIterator Random access iterator for a primitive range
      */
      template <typename PrimitiveIterator>
      static void do_transfer(from_t /* meta */, SystemType const& collection,
                              to_t /* meta */, PrimitiveIterator begin,
                              PrimitiveIterator end) {
        // Generic implementation to fall back to, can use public members
        ELASTICA_ASSERT(end > begin, "Oops");
        ELASTICA_ASSERT(
            collection.size() == static_cast<std::size_t>(end - begin), "Oops");

        // Uses primitives transfer member
        std::for_each(
            begin, end, [&collection, index = 0UL](auto& primitive) mutable {
              primitive.transfer(collection.get_dimensions(index),
                                 collection.get_position(index),
                                 collection.get_velocity(index),
                                 collection.get_angular_velocity(index));
              ++index;
            });
      }

      //**Transfer function*****************************************************
      /*!\brief Transfers information to \systems from \primitives
      //
      // do_transfer implements information transfer to a \system (collection)
      // from a range of \primitives
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
      static void do_transfer(to_t /* meta */, SystemType& collection,
                              from_t /* meta */, ConstPrimitiveIterator begin,
                              ConstPrimitiveIterator end) {
        // Generic implementation to fall back to, can use public members
        ELASTICA_ASSERT(end > begin, "Oops");
        ELASTICA_ASSERT(
            collection.size() == static_cast<std::size_t>(end - begin), "Oops");

        // Friended
        std::for_each(begin, end,
                      [&collection, index = 0UL](auto& primitive) mutable {
                        collection.get_force(index) += primitive.get_force();
                        collection.get_torque(index) += primitive.get_torque();
                        ++index;
                      });
      }
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica