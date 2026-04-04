//==============================================================================
/*!
//  \file
//  \brief Veclocity Verlet time-stepping algorithm
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
      /*!\brief Velocity verlet algorithm
       * \ingroup symplectic
       *
       * \details
       * Implements the velocity Verlet time-stepping algorithm
       */
      struct VelocityVerlet : private NonCreatable {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */

        struct DynamicSubStep : public detail::DynamicUpdate<DynamicSubStep> {
          template <typename System>
          inline auto operator()(System& sys, Time const time_v,
                                 TimeDelta const dt) const noexcept -> void {
            sys.dynamic_states() += (0.5 * dt) * sys.dynamic_rates(time_v);
          }
        };

        struct TimeUpdateSubStep
            : public detail::TimeUpdate<TimeUpdateSubStep> {
          static inline constexpr auto temporal_coefficient() noexcept
              -> real_t {
            return static_cast<real_t>(1.0);
          }
        };

        struct KinematicSubStep
            : public detail::KinematicUpdate<KinematicSubStep> {
          template <typename System>
          inline auto operator()(System& sys, Time const time_v,
                                 TimeDelta const dt) const noexcept -> void {
            sys.kinematic_states() += dt * sys.kinematic_rates(time_v);
          }
        };

        /*! \endcond */
        //**********************************************************************

        // Can wrap over vector if you need here, but not necessary
        //**Type definitions****************************************************
        //! The list of algorithmic substeps
        using SubSteps = tmpl::list<DynamicSubStep, KinematicSubStep,
                                    TimeUpdateSubStep, DynamicSubStep>;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace symplectic

  }  // namespace timestepper

}  // namespace elastica
