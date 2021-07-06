//==============================================================================
/*!
//  \file
//  \brief All other constraints
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

/// First
#include "Simulator/Constraints/Types.hpp"
///
#include "Simulator/Constraints/BoundaryConditionBase.hpp"

namespace elastica {

  namespace constraints {

    class FreeBoundaryCondition final : public BoundaryConditionBase {
      FreeBoundaryCondition(){};
      ~FreeBoundaryCondition(){};
      void constrain_values(ValueType& /*st*/,
                            TimeType /**/) const noexcept override{};
      void constrain_rates(ValueType& /*dt*/,
                           TimeType /**/) const noexcept override{};
      void set(std::vector<double> /*initial_positions*/,
               std::vector<double> /*initial_velocities*/,
               std::vector<double> /*initial_directors*/,
               std::vector<double> /*initial_omegas*/) noexcept override{};
    };

    class FixedBoundaryCondition final : public BoundaryConditionBase {
      FixedBoundaryCondition(ValueType init_Q, ValueType init_x)
          : initial_Q(init_Q), initial_x(init_x){};
      // template <typename ...Args>
      // FixedBoundaryCondition(Args... args)
      // 	: initial_Q(std::get<0UL>(args...)), initial_x(init_x){};
      ~FixedBoundaryCondition(){};

      void constrain_values(ValueType&,
                            TimeType /**/) const noexcept override{};
      void constrain_rates(ValueType&, TimeType /**/) const noexcept override{};

      void set(std::vector<double> /*initial_positions*/,
               std::vector<double> /*initial_velocities*/,
               std::vector<double> /*initial_directors*/,
               std::vector<double> /*initial_omegas*/) noexcept override{
          // initial_Q = initial_directors[0UL];
          // initial_x = initial_positions[0UL];
      };

      // Store starting Q and x?
      ValueType initial_Q, initial_x;
    };

    class HingedBoundaryCondition final : public BoundaryConditionBase {
      // using Parent = DirtyBoundaryCondition<HingedBoundaryCondition>;
      explicit HingedBoundaryCondition(ValueType init_x) : initial_x(init_x){};
      ~HingedBoundaryCondition(){};

      //    template <typename StateType, typename TimeType>
      void constrain_values(ValueType& /**/,
                            TimeType /**/) const noexcept override{};
      void constrain_rates(ValueType& /**/,
                           TimeType /**/) const noexcept override{};

      void set(std::vector<double> /*initial_positions*/,
               std::vector<double> /*initial_velocities*/,
               std::vector<double> /*initial_directors*/,
               std::vector<double> /*initial_omegas*/) noexcept override{
          // initial_x = initial_positions[0UL];
      };

      // Store starting Q and x?
      ValueType initial_x;
    };

  }  // namespace constraints

}  // namespace elastica