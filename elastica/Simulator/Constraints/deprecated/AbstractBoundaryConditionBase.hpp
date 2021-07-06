//==============================================================================
/*!
//  \file
//  \brief Abstract base class for all constraints
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

#include "Utilities/NonCopyable.hpp"

namespace elastica {

  namespace constraints {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Abstract class for storing boundary conditions within \elastica
    // \ingroup constraints
    //
    // DefaultInterface is an implementation defined struct to enforce a common
    // interface to the simulator. It is used if some Simulation add-ons (such
    // as Connections or Constraint) are not present, yet due to incomplete
    // metaprogramming, the Timestepper class expects a certain public interface
    // such as a \c constrain_primitive_rates function. As a temporary remedy,
    // we define the public interface methods here with the correct signature,
    // but leave their implementation empty. This class can be deprecated
    // anytime and its best not to rely on it too much.
    //
    // \tparam SimTraits Simulator Configuration parameter
    */
    class AbstractBoundaryConditionBase : public NonCopyable {
     public:
      virtual ~AbstractBoundaryConditionBase() = default;
      virtual void constrain_values() const = 0;
      virtual void constrain_rates() const = 0;

      // we don't know the pointer type here, so we erase the type and put a
      // void we then convert it to the system pointer type immediately in the
      // concrete implementation. This is a downside of virtual hierarchies, but
      // its okay in this context
      virtual void set(void* sp, std::size_t first_idx,
                       std::size_t second_idx) noexcept = 0;
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace constraints

}  // namespace elastica