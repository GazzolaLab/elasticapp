//==============================================================================
/*!
//  \file
//  \brief Position-Verlet time-stepping algorithm
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

#include <cstddef>  // size_t

#include "Time/SymplecticSteppers/Algorithms/Types.hpp"
///
#include "Time/Helpers.hpp"
#include "Time/SimulationTime.hpp"
///
#include "Utilities/DefineTypes.h"
#include "Utilities/NonCreatable.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace timestepper {

    namespace symplectic {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Position verlet algorithm
       * \ingroup symplectic
       *
       * \details
       * Implements the position Verlet time-stepping algorithm
       */
      struct PositionVerlet : private NonCreatable {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        struct TimeUpdateSubStep
            : public detail::TimeUpdate<TimeUpdateSubStep> {
          static inline constexpr auto temporal_coefficient() noexcept
              -> real_t {
            return static_cast<real_t>(0.5);
          }
        };

        struct KinematicSubStep
            : public detail::KinematicUpdate<KinematicSubStep> {
          template <typename System>
          inline auto operator()(System& sys, Time const time_v,
                                 TimeDelta const dt) const noexcept -> void {
            sys.kinematic_states() +=
                TimeUpdateSubStep::temporal_coefficient() * dt *
                sys.kinematic_rates(time_v);
          }
        };

        struct DynamicSubStep : public detail::DynamicUpdate<DynamicSubStep> {
          template <typename System>
          inline auto operator()(System& sys, Time const time_v,
                                 TimeDelta const dt) const noexcept -> void {
            sys.dynamic_states() += dt * sys.dynamic_rates(time_v);
          }
        };
        /*! \endcond */
        //**********************************************************************

        // Can wrap over vector if you need here, but not necessary
        //**Type definitions****************************************************
        //! The list of algorithmic substeps
        using SubSteps =
            tmpl::list<KinematicSubStep, TimeUpdateSubStep, DynamicSubStep,
                       TimeUpdateSubStep, KinematicSubStep>;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace symplectic

  }  // namespace timestepper

}  // namespace elastica
