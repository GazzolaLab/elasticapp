//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica Constraints module
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
#include <tuple>  // for construction support

///
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Interface/Affordances/detail/EmptyUpdateSteps.hpp"
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Parallel/for_each.hpp"  // uses elastica's, not GLIBCXX
///
#include "Utilities/CRTP.hpp"
#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace constraints {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Execution policy for Constraints module
       * \ingroup interface
       *
       * This class represents the policy of execution for applying constraints
       * to systems within the ambit of the constraints module. As such, the
       * policies span a spectrum for purely sequential execution to parallel,
       * asynchronous execution.
       *
       * \tparam ConstraintsTraits Options for configuration of the Constraints
       * Module
       * \tparam ConstraintsModule The constraints module of the Simulator
       *
       * \see elastica::interface::Constraints
       */
      template <class ConstraintsTraits, class ConstraintsModule>
      class ExecutionPolicy
          : public CRTPHelper<ConstraintsModule, ExecutionPolicy>,
            public ::elastica::modules::detail::EmptyUpdateSteps<
                ConstraintsTraits,
                ExecutionPolicy<ConstraintsTraits, ConstraintsModule>> {
       private:
        //**Type definitions****************************************************
        //! CRTP Type
        using CRTP = CRTPHelper<ConstraintsModule, ExecutionPolicy>;
        //! CRTP methods
        using CRTP::self;
        //! Type of simulation time
        using TimeType = typename ConstraintsTraits::TimeType;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Definition of operator for constraining primitives
        template <typename Policy, class Dummy = void>
        struct ConstrainPrimitivesOp;
        //! Definition of operator for constraining rates
        template <typename Policy, class Dummy = void>
        struct ConstrainRatesOp;
        //! Sequential primitive constraint op
        using ConstrainPrimitivesInSequence =
            ConstrainPrimitivesOp<::elastica::configuration::sequential_policy>;
        //! Parallel primitive constraint op
        using ConstrainPrimitivesInParallel =
            ConstrainPrimitivesOp<::elastica::configuration::parallel_policy>;
        //! Hybrid primitive constraint op
        using ConstrainPrimitivesInHybrid =
            ConstrainPrimitivesOp<::elastica::configuration::hybrid_policy>;
        //! Sequential rate constraint op
        using ConstrainRatesInSequence =
            ConstrainRatesOp<::elastica::configuration::sequential_policy>;
        //! Parallel rate constraint op
        using ConstrainRatesInParallel =
            ConstrainRatesOp<::elastica::configuration::parallel_policy>;
        //! Hybrid rate constraint op
        using ConstrainRatesInHybrid =
            ConstrainRatesOp<::elastica::configuration::hybrid_policy>;
        //**********************************************************************

        //**Primitive classes***************************************************
        /*!\name Primitive constraint functors */
        //@{

        //**********************************************************************
        /*!\brief Sequentially constrain primitives (positions, directors) of
         * systems.
         */
        template <class Dummy>
        struct ConstrainPrimitivesOp<
            ::elastica::configuration::sequential_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ConstrainPrimitivesOp(ConstraintsModule* sim,
                                ConstraintsTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConstrainPrimitivesOp(
              std::tuple<ConstraintsModule*, ConstraintsTraits const&> args)
              : ConstrainPrimitivesOp(
                    cpp17::make_from_tuple<ConstrainPrimitivesOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConstrainPrimitivesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            for (auto const& batch : module_handle.constrainer_batches())
              for (auto const& constrainer : batch)
                constrainer->constrain_primitives(time_v);
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConstraintsModule
          ConstraintsModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Parallely constrain primitives (positions, directors) of
         * systems.
         */
        template <class Dummy>
        struct ConstrainPrimitivesOp<::elastica::configuration::parallel_policy,
                                     Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ConstrainPrimitivesOp(ConstraintsModule* sim,
                                ConstraintsTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConstrainPrimitivesOp(
              std::tuple<ConstraintsModule*, ConstraintsTraits const&> args)
              : ConstrainPrimitivesOp(
                    cpp17::make_from_tuple<ConstrainPrimitivesOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConstrainPrimitivesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // DONE : parallelization occurs across independent batches
            using ::elastica::size;
            ::elastica::parallel::for_each(
                module_handle.constrainer_batches(),
                [time_v](auto const& batch) {
                  for (auto const& constrainer : batch)
                    constrainer->constrain_primitives(time_v);
                });
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConstraintsModule
          ConstraintsModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Hybridly execute algorithm to constrain primitives
         */
        template <class Dummy>
        struct ConstrainPrimitivesOp<::elastica::configuration::hybrid_policy,
                                     Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ConstrainPrimitivesOp(ConstraintsModule* sim,
                                ConstraintsTraits const& traits)
              : constrain_primitives_in_sequence_(sim, traits),
                constrain_primitives_in_parallel_(sim, traits),
                module_handle(*sim),
                parallelism_threshold_(
                    traits.constraints_parallelism_threshold()) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConstrainPrimitivesOp(
              std::tuple<ConstraintsModule*, ConstraintsTraits const&> args)
              : ConstrainPrimitivesOp(
                    cpp17::make_from_tuple<ConstrainPrimitivesOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConstrainPrimitivesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // Is this if() check viable when put in a flow graph?
            if (module_handle.n_constraints() < parallelism_threshold_)
              constrain_primitives_in_sequence_(time_v);
            else
              constrain_primitives_in_parallel_(time_v);
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Serial forcing functor
          ConstrainPrimitivesInSequence constrain_primitives_in_sequence_;
          //! Parallel forcing functor
          ConstrainPrimitivesInParallel constrain_primitives_in_parallel_;
          //! Handle to the ConstraintsModule
          ConstraintsModule const& module_handle;
          //! Threshold to switch between serial and parallel execution
          const std::size_t parallelism_threshold_;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Rate classes********************************************************
        /*!\name Rate constraint methods */
        //@{

        //**********************************************************************
        /*!\brief Sequentially constrain rates (velocities, angular velocities)
         * of systems.
         */
        template <class Dummy>
        struct ConstrainRatesOp<::elastica::configuration::sequential_policy,
                                Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ConstrainRatesOp(ConstraintsModule* sim,
                           ConstraintsTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConstrainRatesOp(
              std::tuple<ConstraintsModule*, ConstraintsTraits const&> args)
              : ConstrainRatesOp(
                    cpp17::make_from_tuple<ConstrainRatesOp>(std::move(args))) {
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConstrainRatesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            for (auto const& batch : module_handle.constrainer_batches())
              for (auto const& constrainer : batch)
                constrainer->constrain_rates(time_v);
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConstraintsModule
          ConstraintsModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Parallely constrain rates (velocities, angular velocities)
         * of systems.
         */
        template <class Dummy>
        struct ConstrainRatesOp<::elastica::configuration::parallel_policy,
                                Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ConstrainRatesOp(ConstraintsModule* sim,
                           ConstraintsTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConstrainRatesOp(
              std::tuple<ConstraintsModule*, ConstraintsTraits const&> args)
              : ConstrainRatesOp(
                    cpp17::make_from_tuple<ConstrainRatesOp>(std::move(args))) {
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConstrainRatesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // DONE : parallelism in batches
            using ::elastica::size;
            ::elastica::parallel::for_each(
                module_handle.constrainer_batches(),
                [time_v](auto const& batch) {
                  for (auto const& constrainer : batch)
                    constrainer->constrain_rates(time_v);
                });
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConstraintsModule
          ConstraintsModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Hybridly execute algorithm to constrain rates
         */
        template <class Dummy>
        struct ConstrainRatesOp<::elastica::configuration::hybrid_policy,
                                Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ConstrainRatesOp(ConstraintsModule* sim,
                           ConstraintsTraits const& traits)
              : constrain_rates_in_sequence_(sim, traits),
                constrain_rates_in_parallel_(sim, traits),
                module_handle(*sim),
                parallelism_threshold_(
                    traits.constraints_parallelism_threshold()) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConstrainRatesOp(
              std::tuple<ConstraintsModule*, ConstraintsTraits const&> args)
              : ConstrainRatesOp(
                    cpp17::make_from_tuple<ConstrainRatesOp>(std::move(args))) {
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConstrainRatesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of rates
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // Is this if() check viable when put in a flow graph?
            if (module_handle.n_constraints() < parallelism_threshold_)
              constrain_rates_in_sequence_(time_v);
            else
              constrain_rates_in_parallel_(time_v);
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Serial forcing functor
          ConstrainRatesInSequence constrain_rates_in_sequence_;
          //! Parallel forcing functor
          ConstrainRatesInParallel constrain_rates_in_parallel_;
          //! Handle to the ConstraintsModule
          ConstraintsModule const& module_handle;
          //! Threshold to switch between serial and parallel execution
          const std::size_t parallelism_threshold_;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for constraints*/
        //@{
        struct ConstrainPrimitives {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = ConstrainPrimitivesOp<
              typename ConstraintsTraits::ConstraintsExecutionPolicy>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string info(
              ConstraintsModule const& module_handle) noexcept {
            return std::string(MakeString{}
                               << "--Apply " << module_handle.n_constraints()
                               << " constraints on positions and rotations\n");
          }
          //********************************************************************
        };

        struct ConstrainRates {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = ConstrainRatesOp<
              typename ConstraintsTraits::ConstraintsExecutionPolicy>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string info(
              ConstraintsModule const& module_handle) noexcept {
            return std::string(
                MakeString{}
                << "--Apply " << module_handle.n_constraints()
                << " constraints on velocities and angular velocities\n");
          }
          //********************************************************************
        };
        //@}
        /*! \endcond */
        //**********************************************************************

       public:
        //**Algorithmic order***************************************************
        /*!\name Algorithmic order of steps within a temporal loop*/
        //@{
        //! Algorithms to be carried out after the kinematic update step
        using PostKinematicUpdateSteps = tmpl::list<ConstrainPrimitives>;
        //! Algorithms to be carried out after the dynamic update step
        using PostDynamicUpdateSteps = tmpl::list<ConstrainRates>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica