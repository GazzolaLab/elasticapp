//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica Connections module
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
#include <tuple>  // for construction support

///
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Streams.hpp"  // string
#include "Simulator/Interface/Affordances/detail/EmptyUpdateSteps.hpp"
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace connections {

      //************************************************************************
      inline std::string connect_algorithm_desc(std::size_t n_connections) {
        return std::string(MakeString{} << "Apply " << n_connections
                                        << " connections between systems");
      }
      //************************************************************************

      //************************************************************************
      template <typename Policy>
      inline std::string connect_algorithm_desc(std::size_t n_connections) {
        return connect_algorithm_desc(n_connections) + " using " +
               configuration::to_string(Policy{});
      }
      //************************************************************************

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      // TODO : should be batched if parallelized

      //************************************************************************
      /*!\brief Execution policy for Connections module
       * \ingroup interface
       *
       * This class represents the policy of execution for applying connections
       * to systems within the ambit of the connections module. As such, the
       * policies span a spectrum for purely sequential execution to parallel,
       * asynchronous execution.
       *
       * \tparam ConnectionsTraits Options for configuration of the Connections
       * Module
       * \tparam ConnectionsModule The connections module of the Simulator
       *
       * \see elastica::interface::Connections
       */
      template <class ConnectionsTraits, class ConnectionsModule>
      class ExecutionPolicy
          : public CRTPHelper<ConnectionsModule, ExecutionPolicy>,
            public ::elastica::modules::detail::EmptyUpdateSteps<
                ConnectionsTraits,
                ExecutionPolicy<ConnectionsTraits, ConnectionsModule>> {
       private:
        //**Type definitions****************************************************
        //! CRTP Type
        using CRTP = CRTPHelper<ConnectionsModule, ExecutionPolicy>;
        //! CRTP methods
        using CRTP::self;
        //! Type of simulation time
        using TimeType = typename ConnectionsTraits::TimeType;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Definition of operator for connecting systems
        template <typename Policy, class Dummy = void>
        struct ConnectSystemsOp;
        //! Sequential connection op
        using ConnectSystemsInSequence =
            ConnectSystemsOp<::elastica::configuration::sequential_policy>;
        //! Parallel connection op
        using ConnectSystemsInParallel =
            ConnectSystemsOp<::elastica::configuration::parallel_policy>;
        //! Hybrid connection op
        using ConnectSystemsInHybrid =
            ConnectSystemsOp<::elastica::configuration::hybrid_policy>;
        //**********************************************************************

        //**Connection functor**************************************************
        /*!\name Connection functor */
        //@{

        //**********************************************************************
        /*!\brief Connect systems in sequence
         */
        template <class Dummy>
        struct ConnectSystemsOp<::elastica::configuration::sequential_policy,
                                Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ConnectSystemsOp(ConnectionsModule* sim,
                           ConnectionsTraits const& /*meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConnectSystemsOp(
              std::tuple<ConnectionsModule*, ConnectionsTraits const&> args)
              : ConnectSystemsOp(
                    std::make_from_tuple<ConnectSystemsOp>(std::move(args))) {
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConnectSystemsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute connection of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            for (auto const& connecter : module_handle.connecters()) {
              connecter->apply_forces(time_v);
              connecter->apply_torques(time_v);
            }
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ConnectSystemsOp const& op) {
            return os << connect_algorithm_desc<
                       ::elastica::configuration::sequential_policy>(
                       op.module_handle.n_connections());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConnectionsModule
          ConnectionsModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Connect systems in parallel
         */
        template <class Dummy>
        struct ConnectSystemsOp<::elastica::configuration::parallel_policy,
                                Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          explicit ConnectSystemsOp(ConnectionsModule* sim,
                                    ConnectionsTraits const& /*meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConnectSystemsOp(
              std::tuple<ConnectionsModule*, ConnectionsTraits const&> args)
              : ConnectSystemsOp(
                    std::make_from_tuple<ConnectSystemsOp>(std::move(args))) {
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConnectSystemsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute connection of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // TODO : paralellism
            for (auto const& connecter : module_handle.connecters()) {
              connecter->apply_forces(time_v);
              connecter->apply_torques(time_v);
            }
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ConnectSystemsOp const& op) {
            return os << connect_algorithm_desc<
                       ::elastica::configuration::parallel_policy>(
                       op.module_handle.n_connections());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConnectionsModule
          ConnectionsModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Hybridly execute algorithm to connect systems
         */
        template <class Dummy>
        struct ConnectSystemsOp<::elastica::configuration::hybrid_policy,
                                Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          explicit ConnectSystemsOp(ConnectionsModule* sim,
                                    ConnectionsTraits const& traits)
              : connect_systems_in_sequence_(sim, traits),
                connect_systems_in_parallel_(sim, traits),
                module_handle(*sim),
                parallelism_threshold_(
                    traits.connections_parallelism_threshold()) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ConnectSystemsOp(
              std::tuple<ConnectionsModule*, ConnectionsTraits const&> args)
              : ConnectSystemsOp(
                    std::make_from_tuple<ConnectSystemsOp>(std::move(args))) {
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ConnectSystemsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute forcing of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // Is this if() check viable when put in a flow graph?
            if (module_handle.n_connections() < parallelism_threshold_)
              connect_systems_in_sequence_(time_v);
            else
              connect_systems_in_parallel_(time_v);
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ConnectSystemsOp const& op) {
            return os << connect_algorithm_desc<
                       ::elastica::configuration::hybrid_policy>(
                       op.module_handle.n_connections());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Serial forcing functor
          ConnectSystemsInSequence connect_systems_in_sequence_;
          //! Parallel forcing functor
          ConnectSystemsInParallel connect_systems_in_parallel_;
          //! Handle to the ConnectionsModule
          ConnectionsModule const& module_handle;
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
        /*!\name Algorithms for connections*/
        //@{
        struct ConnectSystems {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = ConnectSystemsOp<
              typename ConnectionsTraits::ConnectionsExecutionPolicy>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              ConnectionsModule const& module_handle) noexcept {
            return connect_algorithm_desc<
                typename ConnectionsTraits::ConnectionsExecutionPolicy>(
                module_handle.n_connections());
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
        //! Algorithms to be carried out before the dynamic update step
        using PreDynamicUpdateSteps = tmpl::list<ConnectSystems>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace connections

  }  // namespace interface

}  // namespace elastica
