//==============================================================================
/*!
//  \file
//  \brief Operator for resolving contacts (in parallel, with TBB) based on a
//  DEM force model
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

/*
 * dev note : we need a separate parallel class here to implement the split and
 * join constructors
 */

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cmath>
#include <numeric>

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/SequentialContactResolver.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/Types.hpp"
//
#include "Utilities/DefineTypes.h"
// parallelism details
#include "ModuleSettings/Parallel.hpp"
#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/parallel_reduce.h>
#endif

namespace elastica {

  namespace collision {

    namespace response {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Class for resolving forces from DEM contacts using a given
       * Forcing Model, in parallel
       * \ingroup collision_response
       *
       * The ContactResolver class template is used to adapt a particular
       * force model to the interface in the collision system. It reduces all
       * the displacements across all contacts (with forces/torques are
       * side effects) and returns it to the CollisionSystem.
       * This specialization reduces the forces/delta in parallel.
       *
       * \tparam FM The force model to resolve contact forces
       */
#if ELASTICA_TBB_PARALLEL_MODE
      template <typename FM>
      class ContactResolver<FM, ::elastica::configuration::parallel_policy> {
       protected:
        //**Type definitions****************************************************
        //! Serial resolver
        using Serial =
            ContactResolver<FM, ::elastica::configuration::sequential_policy>;
        //! This type
        using This =
            ContactResolver<FM, ::elastica::configuration::parallel_policy>;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of force model
        using ForceModel = typename Serial::ForceModel;
        //! Type of the parallel policy
        using ExecutionPolicy = ::elastica::configuration::parallel_policy;
        //**********************************************************************

        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{

        //**********************************************************************
        /*!\brief Constructor of the ContactResolver.
         */
        explicit ContactResolver(FM force_model, real_t initial_overlap = 0.0)
            : force_model_(std::move(force_model)),
              max_overlap_(initial_overlap){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Default copy-constructor of the ContactResolver.
         */
        ContactResolver(ContactResolver const& other) = default;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Default move-constructor of the ContactResolver.
         */
        ContactResolver(ContactResolver&& other) noexcept = default;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Split constructor for contact reduction
         *
         * Copies the force model, so its up to the user to implement the
         * correct API needed
         */
        ContactResolver(ContactResolver const& other, tbb::split) noexcept
            : ContactResolver(other.force_model(), other.max_overlap()){};
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~ContactResolver() = default;
        //@}
        //**********************************************************************

        //**Access operators****************************************************
        /*!\name Access operators */
        //@{

        //**********************************************************************
        /*!\brief Access to the maximum overlap over all contacts.
         */
        inline constexpr auto max_overlap() & noexcept -> real_t& {
          return max_overlap_;
        }
        inline constexpr auto max_overlap() const& noexcept -> real_t {
          return max_overlap_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Access to the underlying DEM force model
         */
        inline constexpr auto force_model() & noexcept -> ForceModel& {
          return force_model_.force_model();
        }
        inline constexpr auto force_model() const& noexcept
            -> ForceModel const& {
          return force_model_.force_model();
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Function operator***************************************************
        /*!\name Function operator */
        //@{

        //**********************************************************************
        /*!\brief Resolves contacts over a batch
         *
         * \details
         * Resolves forces and torques based on the given contacts and
         * computes the maximal overlap over all the given contacts.
         *
         * \param r Range of contact batches adhering to the tbb::BlockedRange
         * concept
         * \return
         */
        template <class BlockedContactBatchRange>
        void operator()(BlockedContactBatchRange const& r) const noexcept {
          // may not be needed since split() and join will take care of it
          // force_model_.max_overlap() = max_overlap_;
          resolve_contacts(r);
          max_overlap_ = force_model_.max_overlap();
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\name Processing logic */
        //@{

        //**********************************************************************
        /*!\brief Resolves contacts
         *
         * \details
         * Resolves forces and torques based on the given contacts and
         * computes the maximal overlap over all the given contacts.
         *
         * \param r Range of contact batches adhering to the tbb::BlockedRange
         * concept
         * \return
         */
        template <class BlockedContactRange>
        void resolve_contacts(const BlockedContactRange& r) const noexcept {
          force_model_(r);  // can be force_model.resolve(contacts)
        };
        //**********************************************************************
        //@}
        //**********************************************************************

       public:
        //**Join function*******************************************************
        /*!\name Join function*/
        //@{
        void join(This const& other) noexcept {
          max_overlap_ = std::max(max_overlap_, other.max_overlap_);
        }
        //**********************************************************************
        //@}
        //**********************************************************************

       private:
        //**Member
        // variables****************************************************
        /*!\name Member variables */
        //@{
        //! The force response model to apply forces and compute overlaps
        Serial force_model_;
        //! The maximum overlap computed over all the contacts serially
        mutable real_t max_overlap_;
        //@}
        //**********************************************************************
      };
      //************************************************************************
#else
      template <typename FM>
      class ContactResolver<FM, ::elastica::configuration::parallel_policy>
          : public ContactResolver<
                FM, ::elastica::configuration::sequential_policy> {
       private:
        //**Type definitions****************************************************
        //! Parent type
        using P =
            ContactResolver<FM, ::elastica::configuration::sequential_policy>;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of the parallel policy
        using ExecutionPolicy = ::elastica::configuration::parallel_policy;
        //**********************************************************************

        using P::P;
      };
#endif

      //************************************************************************
      /*!\brief ContactResolver with sequential execution
       * \ingroup collision_response
       *
       * \tparam FM The force model to resolve contact forces
       */
      template <typename FM>
      using ParallelContactResolver =
          ContactResolver<FM, ::elastica::configuration::parallel_policy>;
      //************************************************************************

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
