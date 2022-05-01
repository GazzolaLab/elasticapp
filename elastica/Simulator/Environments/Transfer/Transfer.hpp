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
#include "Types.hpp"
////
#include "ErrorHandling/Assert.hpp"
#include "TransferTraits.hpp"
#include "TypeTraits.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*!\brief Customization of transfer operations
     * \ingroup customization
     *
     * \details
     * The Transfer template class is a customization point for implementing
     * transfer operations between a \system of an \elastica Simulator and a
     * \primitive of the \elastica collision library.
     *
     * \section custom Customization
     * Customization is achieved by specializing this class template. If no
     * customization is found for a type via ADL lookup, \elastica falls
     * back to the operations implemented by a default class. If requirements
     * for being default transferable are not met (see
     * elastica::collision::tt::IsDefaultTransferable
     * ), then a compiler error
     * is raised.
     *
     * \example
     * The following shows an example of customization.
     * \snippet Transfer/Test_Transfer.cpp customization_eg
     *
     * As seen in the example above, the Transfer class's #do_transfer function
     * needs to be implemented. Typically you can do this in the translation
     * unit where the `System` is defined.
     *
     * \see elastica::collision::Transfer
     */
    template <typename System>
    struct Transfer;

  }  // namespace customization

}  // namespace elastica

namespace elastica {

  namespace collision {

    namespace detail {

      //************************************************************************
      template <typename System,
                typename = typename tt::IsDefaultTransferable<System>::type>
      struct TransferEval;
      //************************************************************************

      //************************************************************************
      template <typename System>
      struct TransferEval<System, std::true_type>
          : public TransferTraits<System> {
       public:
        //**Type definitions****************************************************
        using Parent = TransferTraits<System>;  //!< Parent type

        using typename Parent::from_t;  //!< From type of transfer
        using typename Parent::to_t;    //!< To type of transfer

        using typename Parent::PrimitiveType;  //!< Type of contained primitive
        using typename Parent::SystemType;     //!< Type of system
        //**********************************************************************

        //**Transfer functions**************************************************
        /*!\name Transfer functions */
        //@{

        /*!\brief Transfers information from \systems to \primitives
         *
         * do_transfer implements information transfer from a \system
         * (collection) to a range of \primitives
         *
         * \param collection  The \system collection
         * \param begin       Begin iterator of the \primitive sequence
         * \param end         End iterator of the \primitive sequence
         * \return void
         *
         * \tparam PrimitiveIterator Random access iterator for a primitive
         * range
         */
        template <typename PrimitiveIterator>
        static void do_transfer(from_t /* meta */, SystemType const& collection,
                                to_t /* meta */, PrimitiveIterator begin,
                                PrimitiveIterator end) {
          // Generic implementation to fall back to, can use public members
          ELASTICA_ASSERT(end > begin, "Oops");
          ELASTICA_ASSERT(
              collection.size() == static_cast<std::size_t>(end - begin),
              "Oops");

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
        //**********************************************************************

        //**********************************************************************
        /*!\brief Transfers information to \systems from \primitives
         *
         * do_transfer implements information transfer to a \system (collection)
         * from a range of \primitives
         *
         * \param collection  The \system collection
         * \param begin       Begin iterator of the \primitive sequence
         * \param end         End iterator of the \primitive sequence
         * \return void
         *
         * \tparam ConstPrimitiveIterator Random access const iterator for a
         *                                primitive range
         */
        template <typename ConstPrimitiveIterator>
        static void do_transfer(to_t /* meta */, SystemType& collection,
                                from_t /* meta */, ConstPrimitiveIterator begin,
                                ConstPrimitiveIterator end) {
          // Generic implementation to fall back to, can use public members
          ELASTICA_ASSERT(end > begin, "Oops");
          ELASTICA_ASSERT(
              collection.size() == static_cast<std::size_t>(end - begin),
              "Oops");

          // Friended
          std::for_each(
              begin, end, [&collection, index = 0UL](auto& primitive) mutable {
                collection.get_force(index) += primitive.get_force();
                collection.get_torque(index) += primitive.get_torque();
                ++index;
              });
        }
        //@}
        //**********************************************************************
      };
      //************************************************************************

      template <typename System>
      struct TransferEval<System, std::false_type>
          : public ::elastica::customization::Transfer<System> {};

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Implements \c transfer operations
     * \ingroup transfer
     *
     * The Transfer template class implements transfer operations between a
     * \system of an \elastica Simulator and a \primitive of the
     * \elastica collision library. The Transfer template class is designed to
     * be used by the TransferWizard to orchestrate the transfer. It forwards
     * to the available customizations of transfer.
     *
     * \example
     * The following shows an example of using Transfer.
     * \snippet Transfer/Test_Transfer.cpp transfer_eg
     *
     * \see TransferWizard, elastica::customization::Transfer
     *
     * \tparam SystemTag A valid \elastica system tag
     */
    template <typename System>
    struct Transfer : public detail::TransferEval<System> {};
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica