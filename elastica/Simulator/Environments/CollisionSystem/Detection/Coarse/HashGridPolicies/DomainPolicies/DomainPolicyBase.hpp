//==============================================================================
/*!
//  \file
//  \brief Common domain utilities for the HashGrid implementation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cstddef>

#include "ModuleSettings/HashGrids.hpp"
//
#include "Simulator/Domain/Types.hpp"
#include "Simulator/Frames/EulerianFrame.hpp"
//
#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Customization of the HashGrid for unbounded domains
           * \ingroup collision_detection
           *
           * \tparam SimulatorConfig The configuration of collision detection
           */
          class DomainPolicyBase {
           protected:
            //**Type definitions************************************************
            //! Type of the simulation domain
            using DirectionType =
                typename ::elastica::detail::EulerianFrame::DirectionType;
            //******************************************************************

            template <DirectionType Dir>
            static inline constexpr auto initial_cell_count() noexcept
                -> std::size_t;
          };
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Gets the initial cell count in the x direction of the
           * hashgrid.
           *
           * \tparam Dir Direction type Dir
           */
          template <>
          inline constexpr auto DomainPolicyBase::initial_cell_count<
              DomainPolicyBase::DirectionType::x>() noexcept
              -> std::size_t {
            using namespace elastica::collision::detection::coarse::hhg;
            return initial_x_cell_count();
          }
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Gets the initial cell count in the y direction of the
           * hashgrid.
           *
           * \tparam Dir Direction type Dir
           */
          template <>
          inline constexpr auto DomainPolicyBase::initial_cell_count<
              DomainPolicyBase::DirectionType::y>() noexcept
              -> std::size_t {
            using namespace elastica::collision::detection::coarse::hhg;
            return initial_y_cell_count();
          }
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Gets the initial cell count in the z direction of the
           * hashgrid.
           *
           * \tparam Dir Direction type Dir
           */
          template <>
          inline constexpr auto DomainPolicyBase::initial_cell_count<
              DomainPolicyBase::DirectionType::z>() noexcept
              -> std::size_t {
            using namespace elastica::collision::detection::coarse::hhg;
            return initial_z_cell_count();
          }
          /*! \endcond */
          //********************************************************************

        }  // namespace detail

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
