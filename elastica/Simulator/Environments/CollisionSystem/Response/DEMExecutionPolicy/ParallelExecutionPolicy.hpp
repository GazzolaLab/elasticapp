//==============================================================================
/*!
//  \file
//  \brief Parallely resolve contacts using DEM collision response
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

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/ParallelContactResolver.hpp"
//
#include "Parallel/blocked_range.hpp"
#include "Parallel/detail.hpp"  // for warning
#include "Parallel/reduce.hpp"
//
#include "Utilities/AsConst.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Overloader.hpp"
//

namespace elastica {

  namespace collision {

    namespace response {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Operator to apply forces on the detected contacts
       * \ingroup collision_response
       *
       * \tparam CollisionConfig The configuration for collision detection
       *
       * \note
       * Requirements : Force model is copy-constructible
       */
      template <typename CollisionConfig_, typename ExecutionPolicy_>
      class DEMSolverOp {
       protected:
        //**Type definitions****************************************************
        //! Type of the configuration.
        using CollisionConfig = CollisionConfig_;
        //! This type
        using This = DEMSolverOp<CollisionConfig, ExecutionPolicy_>;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of the policy
        using ExecutionPolicy = ExecutionPolicy_;
        //! The DEM response force model
        using ForceModel = typename CollisionConfig::ForceModel;
        //! Resolver type
        using Resolver = ContactResolver<ForceModel, ExecutionPolicy>;
        //**********************************************************************

       public:
        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{

        //**********************************************************************
        /*!\brief Constructor of the ContactResolver.
         */
        explicit DEMSolverOp(ForceModel force_model = {})
            : resolve_(std::move(force_model)) {}
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~DEMSolverOp() = default;
        //@}
        //**********************************************************************

        //**Access operators****************************************************
        /*!\name Access operators */
        //@{

        //**********************************************************************
        /*!\brief Access to the maximum overlap over all contacts.
         */
        inline constexpr auto max_overlap() & noexcept -> real_t& {
          return resolve_.max_overlap();
        }
        inline constexpr auto max_overlap() const& noexcept -> real_t {
          return resolve_.max_overlap();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Access to the underlying DEM force model
         */
        inline constexpr auto force_model() & noexcept -> ForceModel& {
          return resolve_.force_model();
        }
        inline constexpr auto force_model() const& noexcept
            -> ForceModel const& {
          return resolve_.force_model();
        }
        //**********************************************************************

        //**Function operator***************************************************
        /*!\name Function operator */
        //@{

        //**********************************************************************
        /*!\brief Resolves contacts
         *
         * \details
         * Resolves forces and torques based on the given contacts to
         * computes the maximal overlap over all contacts. Contacts are expected
         * to be batched into a granularity that can be parallely invoked
         *
         * \param contacts The number of contacts
         * \return
         */
        template <typename BatchedContactContainer>
        auto operator()(BatchedContactContainer const& contact_batches)
            const noexcept -> real_t {
          using std::cbegin;
          using std::cend;
          // looks similar to the parallel-op
          resolve_.max_overlap() = static_cast<real_t>(0.0);

          make_overloader(
              [&](elastica::configuration::sequential_policy) {
                resolve_(elastica::parallel::make_blocked_range(
                    cbegin(contact_batches), cend(contact_batches)));
              },
              [&](elastica::configuration::parallel_policy) {
                elastica::parallel::reduce(
                    elastica::parallel::make_blocked_range(
                        cbegin(contact_batches), cend(contact_batches)),
                    resolve_);
              })(ExecutionPolicy{});

          return resolve_.max_overlap();
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        /*!\name Member variables */
        //@{
        //! Model for applying DEM forces
        mutable Resolver resolve_;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
