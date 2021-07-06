//==============================================================================
/*!
//  \file
//  \brief Base class for all constraints within \elastica
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

#include <cstdint>

/// Types first
#include "Simulator/Constraints/Types.hpp"
///
#include "Simulator/Constraints/AbstractBoundaryConditionBase.hpp"

namespace elastica {

  namespace constraints {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Helper struct for enforcing a default interface for the simulator.
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
    template <typename BC,  // The implemented type of the boundary
              typename S>   // The system
    class BoundaryConditionBase : public AbstractBoundaryConditionBase {
     protected:
      BoundaryConditionBase() = default;
      ~BoundaryConditionBase() override = default;

      // Must come from the configuration files, leave for now
      using ValueType = double;
      using TimeType = ValueType;

      //**Type definitions******************************************************
      //! Type of the boundary condition
      using BoundaryCondition = BC;
      //! Reference type of the boundary condition
      using Reference = BoundaryCondition&;
      //! const reference type of the boundary condition
      using ConstReference = BoundaryCondition const&;
      //************************************************************************

      //**Self method***********************************************************
      /*!\brief Access to the underlying boundary condition
      //
      // \return Mutable reference to the underlying boundary condition
      //
      // Safely down-casts this module to the underlying boundary condition
      // type, using the Curiously Recurring Template Pattern (CRTP).
      */
      inline constexpr auto self() noexcept -> Reference {
        return static_cast<Reference>(*this);
      }
      //************************************************************************

      //**Self method***********************************************************
      /*!\brief Access to the underlying boundary condition
      //
      // \return Const reference to the underlying boundary condition
      //
      // Safely down-casts this module to the underlying boundary condition
      // type, using the Curiously Recurring Template Pattern (CRTP).
      */
      inline constexpr auto self() const noexcept -> ConstReference {
        return static_cast<ConstReference>(*this);
      }
      //************************************************************************

     public:
      void constrain_values(/*ValueType& s, TimeType t*/) const final {
        self().constrain_values(*sys_ptr, idx1, idx2);
      };

      void constrain_rates(/*ValueType& s, TimeType t*/) const final {
        self().constrain_rates(*sys_ptr, idx1, idx2);
      };

      //      virtual void set(std::vector<double> initial_positions,
      //                       std::vector<double> initial_velocities,
      //                       std::vector<double> initial_directors,
      //                       std::vector<double> initial_omegas) = 0;
      //

      void set(void* sp, std::size_t first_idx,
               std::size_t second_idx) noexcept override {
        sys_ptr = static_cast<S*>(sp);
        idx1 = first_idx;
        idx2 = second_idx;
      }

     private:
      // non-owning
      S* sys_ptr = nullptr;
      // For arbitrary number of indices
      // static constexpr std::size_t n_indices_in_bc = BC::n_indices_in_bc;
      // While we can have arbitrary number of indices here, rarely are more
      // than two used
      std::size_t idx1 = 0UL, idx2 = 0UL;
    };
    /*! \endcond */

  }  // namespace constraints

}  // namespace elastica