//==============================================================================
/*!
//  \file
//  \brief Transfer tags
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

#include "Types.hpp"

namespace elastica {

  namespace collision {

    namespace tags {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Tag for source.
       * \ingroup transfer
       *
       * The from_t is a tag struct that marks the source of message in a
       * transfer between a \system of an \elastica Simulator and a
       * \primitive.  For example in a transfer from a Sphere to a
       * SpherePrimitive, one can declare the transfer function like so:
       * \code
       *    void transfer(from_t, Sphere, from_t, SpherePrimitive);
       * \endcode
       * to indicate that the direction of transfer is from Sphere to
       * SpherePrimitive
       *
       * \see to_t, elastica::collision::from
       */
      struct from_t {};
      //************************************************************************

      //************************************************************************
      /*!\brief Tag for destination.
       * \ingroup transfer
       *
       * The to_t is a tag struct that marks the destination of message in a
       * transfer between a \system of an \elastica Simulator and a
       * \primitive.  For example in a transfer from a SpherePrimitive to a
       * Sphere, one can declare the transfer function like so:
       * \code
       *    void transfer(from_t, SpherePrimitive, from_t, Sphere);
       * \endcode
       * to indicate that the direction of transfer is from SpherePrimitive to
       * SpherePrimitive
       *
       * \see from_t, elastica::collision::to
       */
      struct to_t {};
      //************************************************************************

    }  // namespace tags

    //==========================================================================
    //
    //  GLOBAL TRANSFER VALUES
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Global \c from instance.
     * \ingroup transfer
     *
     * The from instance can be used wherever a from_t is expected, as in a \c
     * transfer message. It is a convenient shorthand for sending messages and
     * leads to readable and literate code. For example, for the following
     * definition of a transfer function
     * \code
     *   void transfer(from_t, Sphere, to_t, SpherePrimitive);
     * \endcode
     * the call site can invoke it as
     * \code
     *   using namespace elastica;
     *   Sphere sphere;
     *   SpherePrimitive sphere_primitive;
     *   transfer(from, sphere, to, sphere_primitive);
     * \endcode
     */
    constexpr tags::from_t from{};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Global \c to instance.
     * \ingroup transfer
     *
     * The to instance can be used wherever a to_t is expected, as in a \c
     * transfer message. It is a convenient shorthand for sending messages and
     * leads to readable and literate code. For example, for the following
     * definition of a transfer function
     * \code
     *   void transfer(to_t, Sphere, from_t, SpherePrimitive);
     * \endcode
     * the call site can invoke it as
     * \code
     *   using namespace elastica;
     *   Sphere sphere;
     *   SpherePrimitive sphere_primitive;
     *   transfer(to, sphere, from, sphere_primitive);
     * \endcode
     */
    constexpr tags::to_t to{};
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
