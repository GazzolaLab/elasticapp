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
        struct UpdatePrimitivesOp;
        //! Definition of operator for constraining rates
        struct UpdateRatesOp;
        //**********************************************************************

        //**Primitive classes***************************************************
        /*!\name Primitive constraint functors */
        //@{

        //**********************************************************************
        /*!\brief Sequentially constrain primitives (positions, directors) of
         * systems.
         */
        struct UpdatePrimitivesOp {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          UpdatePrimitivesOp(ImposedPrimitivesModule* sim,
                             ImposedPrimitivesTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit UpdatePrimitivesOp(std::tuple<ImposedPrimitivesModule*,
                                                 ImposedPrimitivesTraits const&>
                                          args)
              : UpdatePrimitivesOp(cpp17::make_from_tuple<UpdatePrimitivesOp>(
                    std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~UpdatePrimitivesOp() = default;
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
        struct UpdateRatesOp {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          UpdateRatesOp(ImposedPrimitivesModule* sim,
                        ImposedPrimitivesTraits const& /* meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit UpdateRatesOp(std::tuple<ImposedPrimitivesModule*,
                                            ImposedPrimitivesTraits const&>
                                     args)
              : UpdateRatesOp(
                    cpp17::make_from_tuple<UpdateRatesOp>(std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~UpdateRatesOp() = default;
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
        struct UpdatePrimitives {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = UpdatePrimitivesOp;
          //********************************************************************

          //**Info method*******************************************************
          static std::string info(
              ImposedPrimitivesModule const& module_handle) noexcept {
            return std::string(
                MakeString{}
                << "--Update " << module_handle.n_imposed_primitives()
                << " imposed primitives' positions and rotations\n");
          }
          //********************************************************************
        };

        struct UpdateRates {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = UpdateRatesOp;
          //********************************************************************

          //**Info method*******************************************************
          static std::string info(
              ImposedPrimitivesModule const& module_handle) noexcept {
            return std::string(MakeString{}
                               << "--Update "
                               << module_handle.n_imposed_primitives()
                               << " imposed primitives' velocities and "
                                  "angular velocities\n");
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
        using PostKinematicUpdateSteps = tmpl::list<UpdatePrimitives>;
        //! Algorithms to be carried out after the dynamic update step
        using PostDynamicUpdateSteps = tmpl::list<UpdateRates>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace environments

  }  // namespace interface

}  // namespace elastica
