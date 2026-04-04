//==============================================================================
/*!
//  \file
//  \brief Helpers for implementing time-step updates
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

///
#include "Time/Helpers/Types.hpp"
#include "Time/SimulationTime.hpp"
///
#include "Utilities/CRTP.hpp"

namespace elastica {

  namespace timestepper {

    namespace detail {

      //************************************************************************
      /*!\brief Marks an update sub-step as a state update sub-step
       * \ingroup time
       *
       * \tparam SubStep substep derived from the current CRTP class
       */
      template <typename SubStep>
      struct StateUpdate : public CRTPHelper<SubStep, StateUpdate> {
        using CRTP = CRTPHelper<SubStep, StateUpdate>;
        using CRTP::self;
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Marks an update sub-step as a kinematics-only update sub-step
       * \ingroup time
       *
       * \tparam SubStep substep derived from the current CRTP class
       */
      template <typename D>
      struct KinematicUpdate : public StateUpdate<D> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Marks an update sub-step as a dynamics-only update sub-step
       * \ingroup time
       *
       * \tparam SubStep substep derived from the current CRTP class
       */
      template <typename D>
      struct DynamicUpdate : public StateUpdate<D> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Marks an update sub-step as a time-only update sub-step
       * \ingroup time
       *
       * \tparam SubStep substep derived from the current CRTP class
       */
      template <typename D>
      struct TimeUpdate : public CRTPHelper<D, TimeUpdate> {
        using CRTP = CRTPHelper<D, TimeUpdate>;
        using CRTP::self;

        inline auto operator()(TimeDelta const dt) const noexcept -> TimeDelta {
          return self().temporal_coefficient() * dt;
        }
      };
      //************************************************************************

    }  // namespace detail

    //**Dispatch methods********************************************************
    /*!\name Dispatch methods*/
    // \brief Dispatch to functions based on the type of the update step
    // \ingroup time
    //@{

    // two argument versions
    template <typename Step, typename StateUpdateOp, typename TimeUpdateOp>
    inline auto dispatch_based_on(
        detail::TimeUpdate<Step> const& update_step, StateUpdateOp&&,
        TimeUpdateOp&&
            time_update_op) noexcept(noexcept(time_update_op(update_step)))
        -> TimeUpdateOp&& {
      time_update_op(update_step);
      return std::forward<TimeUpdateOp>(time_update_op);
    }

    template <typename Step, typename StateUpdateOp, typename TimeUpdateOp>
    inline auto dispatch_based_on(
        detail::StateUpdate<Step> const& update_step,
        StateUpdateOp&& system_update_op,
        TimeUpdateOp&&) noexcept(noexcept(system_update_op(update_step.self())))
        -> StateUpdateOp&& {
      system_update_op(update_step.self());
      return std::forward<StateUpdateOp>(system_update_op);
    }

    // three argument versions
    template <typename Step, typename KinematicUpdateOp,
              typename DynamicUpdateOp, typename TimeUpdateOp>
    inline auto dispatch_based_on(
        detail::TimeUpdate<Step> const& update_step, KinematicUpdateOp&&,
        DynamicUpdateOp&&,
        TimeUpdateOp&&
            time_update_op) noexcept(noexcept(time_update_op(update_step)))
        -> TimeUpdateOp&& {
      time_update_op(update_step);
      return std::forward<TimeUpdateOp>(time_update_op);
    }

    template <typename Step, typename KinematicUpdateOp,
              typename DynamicUpdateOp, typename TimeUpdateOp>
    inline auto dispatch_based_on(
        detail::KinematicUpdate<Step> const& update_step,
        KinematicUpdateOp&& kinematic_update_op, DynamicUpdateOp&&,
        TimeUpdateOp&&) noexcept(noexcept(kinematic_update_op(update_step
                                                                  .self())))
        -> KinematicUpdateOp&& {
      kinematic_update_op(update_step.self());
      return std::forward<KinematicUpdateOp>(kinematic_update_op);
    }

    template <typename Step, typename KinematicUpdateOp,
              typename DynamicUpdateOp, typename TimeUpdateOp>
    inline auto dispatch_based_on(
        detail::DynamicUpdate<Step> const& update_step, KinematicUpdateOp&&,
        DynamicUpdateOp&& dynamic_update_op,
        TimeUpdateOp&&) noexcept(noexcept(dynamic_update_op(update_step
                                                                .self())))
        -> DynamicUpdateOp&& {
      dynamic_update_op(update_step.self());
      return std::forward<DynamicUpdateOp>(dynamic_update_op);
    }
    //@}
    //**************************************************************************

    /*
     * tmpl::for_each<Steps>(
     * [this, &time, dt](auto v){
     * using UpdateStep = tmpl::type_from<decltype(v)>;
     * auto const& update_step = std::get<UpdateStep>(updates);
     *
     * dispatch(update_step,
     * [&time = std::as_const<>, dt](auto&& update){
     * tmpl::for_each<Systems>([this, time, dt](auto s){
     *  using System = tmpl::type_from<decltype(s)>;
     *  update_step(this->get_systems<System>(), time, dt);
     * }
     * }
     * )
     * },
     * [&time](auto v){
     * using UpdateTimeStep = tmpl::type_from<decltype(v)>;
     * auto&& step = std::get<UpdateTimeStep>(updates);
     * time_v += step(dt);
     * })
     * );
     *
     * dispatch([](){}, [&time, dt](auto&& step){
     * time_v += step(dt);
     * })
     */

  }  // namespace timestepper

}  // namespace elastica
