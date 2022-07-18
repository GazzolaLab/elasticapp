//==============================================================================
/*!
//  \file
//  \brief Operator for resolving contacts based on a DEM force model
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

#include <algorithm>
#include <numeric>
#include <utility>

#include "ErrorHandling/Assert.hpp"
//
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy/Types.hpp"
//
#include "Utilities/DefineTypes.h"

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
       * Forcing Model
       * \ingroup collision_response
       *
       * The ContactResolver class template is used to adapt a particular
       * force model to the interface in the collision system. It reduces all
       * the displacements across all contacts (with forces/torques are
       * side effects) and returns it to the CollisionSystem.
       *
       * \tparam FM The force model to resolve contact forces
       */
      template <typename FM>
      class ContactResolver<FM, ::elastica::configuration::sequential_policy> {
       public:
        //**Type definitions****************************************************
        //! Type of force model
        using ForceModel = FM;
        //! Type of the parallel policy
        using ParallelPolicy = ::elastica::configuration::sequential_policy;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! This type
        using This =
            ContactResolver<FM, ::elastica::configuration::sequential_policy>;
        //**********************************************************************

       public:
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
          return force_model_;
        }
        inline constexpr auto force_model() const& noexcept
            -> ForceModel const& {
          return force_model_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Function operator***************************************************
        /*!\name Function operator */
        //@{

        //**********************************************************************
        /*!\brief Resolves contacts
         *
         * \details
         * Resolves forces and torques based on the given contacts and
         * computes the maximal overlap over all the given contacts.
         *
         * Ideally, this function returns the overlap immediately. But to use
         * it like TBB's parallel-reduce, we need to maintain an internal
         * state. While usually using a func() with internal state is not a
         * good idea, here we are constrained by TBB's interface.
         *
         * \param r Range of contacts adhering to the tbb::BlockedRange
         * concept
         * \return
         */
        template <class BlockedContactRange>
        inline void operator()(const BlockedContactRange& r) const noexcept {
          // r.begin(), r.end() is iterator to a vector of contacts that can be
          // indpendently processed
          using std::cbegin;
          using std::cend;
          std::for_each(cbegin(r), cend(r),
                        [&](auto const& contacts_container) {
                          resolve_contacts(contacts_container);
                        });
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
          using std::cbegin;
          using std::cend;

          max_overlap_ = std::accumulate(
              cbegin(r), cend(r), max_overlap_,
              [this](real_t running_max, auto const& contact) {
                return std::max(running_max, force_model_(contact));
              });
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Join function*******************************************************
        /*!\name Join function*/
        //@{
        void join(This const&) noexcept {
          ELASTICA_ASSERT(false, "should not reach this point!");
        }
        //**********************************************************************
        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        /*!\name Member variables */
        //@{
        /* ForceModel is implemented with state for things like stiction*/
        //! The force response model to apply forces and compute overlaps
        ForceModel force_model_;
        //! The maximum overlap computed over all the contacts
        mutable real_t max_overlap_;
        //@}
        //**********************************************************************
      };
      //************************************************************************

      //************************************************************************
      /*!\brief ContactResolver with sequential execution
       * \ingroup collision_response
       *
       * \tparam FM The force model to resolve contact forces
       */
      template <typename FM>
      using SequentialContactResolver =
          ContactResolver<FM, ::elastica::configuration::sequential_policy>;
      //************************************************************************

    }  // namespace response

  }  // namespace collision

}  // namespace elastica