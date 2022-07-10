//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica ImposedPrimitives
//  module
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
#include "Simulator/Interface/Environments/Types.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace environments {

      //************************************************************************
      inline std::string kinematics_algorithm_desc(
          std::size_t n_imposed_primitives) {
        return std::string(MakeString{}
                           << "Update kinematic (position, director) states of "
                           << n_imposed_primitives << " imposed primitives");
      }
      //************************************************************************

      //************************************************************************
      inline std::string dynamics_algorithm_desc(
          std::size_t n_imposed_primitives) noexcept {
        return std::string(
            MakeString{}
            << "Update dynamic (velocity, angular velocity) states of "
            << n_imposed_primitives << " imposed primitives");
      }
      //************************************************************************

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Execution policy for ImposedPrimitives module
       * \ingroup interface
       *
       * This class represents the policy of execution for applying kinematics
       * to imposed primitives within the ambit of the environments
       * primitives module.
       *
       * \tparam ImposedPrimitivesTraits Options for configuration of the
       * ImposedPrimitives Module
       * \tparam ImposedPrimitivesModule The
       * imposed primitives module of the Simulator
       *
       * \see elastica::interface::ImposedPrimitives
       */
      template <class ImposedPrimitivesTraits, class ImposedPrimitivesModule>
      class WithImposedPrimitivesExecutionPolicy
          : public CRTPHelper<ImposedPrimitivesModule, ExecutionPolicy>,
            public ::elastica::modules::detail::EmptyUpdateSteps<
                ImposedPrimitivesTraits,
                WithImposedPrimitivesExecutionPolicy<ImposedPrimitivesTraits,
                                                     ImposedPrimitivesModule>> {
       private:
        //**Type definitions****************************************************
        //! CRTP Type
        using CRTP = CRTPHelper<ImposedPrimitivesModule, ExecutionPolicy>;
        //! CRTP methods
        using CRTP::self;
        //! Type of simulation time
        using TimeType = typename ImposedPrimitivesTraits::TimeType;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Definition of operator for constraining primitives
        struct UpdateImposedPrimitiveKinematicsOp;
        //! Definition of operator for constraining rates
        struct UpdateImposedPrimitiveDynamicsOp;
        //**********************************************************************

        //**Primitive classes***************************************************
        /*!\name Primitive constraint functors */
        //@{

        //**********************************************************************
        /*!\brief Sequentially constrain primitives (positions, directors) of
         * systems.
         */
        struct UpdateImposedPrimitiveKinematicsOp {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          UpdateImposedPrimitiveKinematicsOp(
              ImposedPrimitivesModule* sim,
              ImposedPrimitivesTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit UpdateImposedPrimitiveKinematicsOp(
              std::tuple<ImposedPrimitivesModule*,
                         ImposedPrimitivesTraits const&>
                  args)
              : UpdateImposedPrimitiveKinematicsOp(
                    cpp17::make_from_tuple<UpdateImposedPrimitiveKinematicsOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~UpdateImposedPrimitiveKinematicsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            module_handle.for_each_imposed_primitive(
                [=](auto& imposed_primitive) {
                  imposed_primitive.update_primitives(time_v);
                });
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(
              std::ostream& os, UpdateImposedPrimitiveKinematicsOp const& op) {
            return os << kinematics_algorithm_desc(
                       op.module_handle.n_imposed_primitives());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ImposedPrimitivesModule
          ImposedPrimitivesModule /*const*/& module_handle;
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
        struct UpdateImposedPrimitiveDynamicsOp {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          UpdateImposedPrimitiveDynamicsOp(
              ImposedPrimitivesModule* sim,
              ImposedPrimitivesTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit UpdateImposedPrimitiveDynamicsOp(
              std::tuple<ImposedPrimitivesModule*,
                         ImposedPrimitivesTraits const&>
                  args)
              : UpdateImposedPrimitiveDynamicsOp(
                    cpp17::make_from_tuple<UpdateImposedPrimitiveDynamicsOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~UpdateImposedPrimitiveDynamicsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute constraint of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            module_handle.for_each_imposed_primitive(
                [=](auto& imposed_primitive) {
                  imposed_primitive.update_rates(time_v);
                });
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(
              std::ostream& os, UpdateImposedPrimitiveDynamicsOp const& op) {
            return os << dynamics_algorithm_desc(
                       op.module_handle.n_imposed_primitives());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ImposedPrimitivesModule
          ImposedPrimitivesModule /*const*/& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for imposed primitives*/
        //@{
        struct UpdateImposedPrimitiveKinematics {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = UpdateImposedPrimitiveKinematicsOp;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              ImposedPrimitivesModule const& module_handle) noexcept {
            return kinematics_algorithm_desc(
                module_handle.n_imposed_primitives());
          }
          //********************************************************************
        };

        struct UpdateImposedPrimitiveDynamics {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = UpdateImposedPrimitiveDynamicsOp;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              ImposedPrimitivesModule const& module_handle) noexcept {
            return dynamics_algorithm_desc(
                module_handle.n_imposed_primitives());
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
        using PostKinematicUpdateSteps =
            tmpl::list<UpdateImposedPrimitiveKinematics>;
        //! Algorithms to be carried out after the dynamic update step
        using PostDynamicUpdateSteps =
            tmpl::list<UpdateImposedPrimitiveDynamics>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace environments

  }  // namespace interface

}  // namespace elastica
