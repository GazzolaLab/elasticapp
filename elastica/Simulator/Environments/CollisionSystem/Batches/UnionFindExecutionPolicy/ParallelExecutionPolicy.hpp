//==============================================================================
/*!
//  \file
//  \brief Implementation of the parallel 'Union Find' batch generation algo.
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>

#include "ErrorHandling/Assert.hpp"
//
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
//
#include "Simulator/Environments/CollisionSystem/Batches/UnionFindExecutionPolicy/SequentialExecutionPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/UnionFindExecutionPolicy/Types.hpp"
//
#include "ModuleSettings/Parallel.hpp"
#include "Parallel/for_each.hpp"
#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/concurrent_unordered_set.h>
#endif
//
#include "Utilities/NonCopyable.hpp"
#include "Utilities/Size.hpp"

namespace elastica {

  namespace collision {

    namespace batches {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Implementation of the 'Union Find' batch generation algorithm.
       * \ingroup batch_generation
       *
       * \details
       * The UnionFind class represents the implementation of the 'Union Find'
       * batch generation algorithm. This algorithm is used to create
       * independent batches of contacts that can be solved independent/in
       * parallel to each other.
       *
       * This specialization implements a sequential version of the algorithm.
       *
       * \tparam C Type of the configuration
       */
#if ELASTICA_TBB_PARALLEL_MODE
      template <typename C>
      class UnionFindOp<C, ::elastica::configuration::parallel_policy>
          : private NonCopyable {
       public:
        //**Type definitions****************************************************
        //! Type of the configuration.
        using CollisionConfig = C;
        //! Type of the parallel policy
        using ParallelPolicy = ::elastica::configuration::parallel_policy;
        //! Type of this UnionFind instance.
        using This = UnionFindOp<CollisionConfig, ParallelPolicy>;
        //! Type of the rigid bodies
        using BodyType = typename CollisionConfig::BodyType;
        //! Handle for a rigid body.
        using BodyID = typename CollisionConfig::BodyID;
        //! Handle for a constant rigid  body.
        using ConstBodyID = typename CollisionConfig::ConstBodyID;
        //!< Type of a constant node handle.
        using NodeID = typename BodyType::NodeID;
        //!< Type of a constant node handle.
        using ConstNodeID = typename BodyType::ConstNodeID;
        //! Type of lookup table
        using LUT = tbb::concurrent_unordered_set<ConstNodeID>;
        //**********************************************************************

       protected:
        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        UnionFindOp() = default;
        UnionFindOp(UnionFindOp&&) noexcept = default;
        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~UnionFindOp() = default;
        //@}
        //**********************************************************************

        //**Utility functions***************************************************
        /*!\name Utility functions */
        //@{

        //**********************************************************************
        /*!\brief Generating independent batches of contacts.
         *
         * \param contacts Contact container holding all existing contacts.
         * \param batches Batch container to be filled with the contacts.
         * \return void
         *
         * This function generates independent batches of contacts from the
         * contacts given in \a contacts.
         *
         * \note
         * The routine does clear the batch after its passed in. The caller
         * takes responsibility for the batches.
         */
        template <typename Contacts, typename Batches>
        void generate_batches(Contacts& contacts, Batches& batches) const {
          // using Batch = typename Batches::Batch;

          // Initialization, clear() is unsafe in parallel
          roots_.clear();

          using std::cbegin;
          using std::cend;
          elastica::parallel::for_each(
              cbegin(contacts), cend(contacts), [this](auto const& contact) {
                const BodyID b1 = contact.get_first_body();
                const BodyID b2 = contact.get_second_body();

                ELASTICA_ASSERT(
                    b1->is_fixed() or b2->is_fixed() or
                        b1->get_root() == b2->get_root(),
                    "Conflicting root nodes in contacting bodies detected");

                ConstNodeID root(b1->is_fixed() ? b2->get_root()
                                                : b1->get_root());

                if (roots_.find(root) == roots_.end()) {
                  // Having an atomic variable just for the size does not seem
                  // like a bright idea overall
                  // root->batch_ = batch_count++;
                  roots_.insert(root);
                }
              });

          // Counting the number of independent contact graphs
          // elastica::size may be thread unsafe?
          const std::size_t batch_count(roots_.size());

          // TODO : refactor threshold
          if (batch_count < 1000UL) {
            // Serial loop looks something like this
            std::size_t index(0);
            for (ConstNodeID root : roots_) {
              const_cast<NodeID>(root)->set_batch(index++);
            }
          } else {
            // do parallel
            // TODO: parallelize, seems too much work for too little benefit IMO
            std::size_t index(0);
            for (ConstNodeID root : roots_) {
              const_cast<NodeID>(root)->set_batch(index++);
            }
          }

          // Sorting the contacts into batches
          batches.resize(batch_count);

          using std::begin;
          using std::end;
          elastica::parallel::for_each(
              begin(contacts), end(contacts), [&batches, this](auto& contact) {
                const BodyID b1 = contact.get_first_body();
                const BodyID b2 = contact.get_second_body();

                ConstNodeID root(b1->is_fixed() ? b2->get_root()
                                                : b1->get_root());
                auto& batch(batches[root->batch()]);

                // concurrent vector can safely call size without thread safety
                // issues
                // each contact is only accessed by a single thread, so no
                // issues in setting index as well
                contact.set_index(batch.size());
                // Adding the contact to the batch
                batch.push_back(contact);
              });
        }

       private:
        //**Member variables****************************************************
        /*!\name Member variables */
        //@{
        //! Set tracking all added roots
        mutable LUT roots_;
        //@}
        //**********************************************************************
        //@}
        //**********************************************************************
      };
      //************************************************************************
#else
      template <typename C>
      class UnionFindOp<C, ::elastica::configuration::parallel_policy>
          : public UnionFindOp<C,
                               ::elastica::configuration::sequential_policy> {
       private:
        //**Type definitions****************************************************
        //! Parent type
        using P = UnionFindOp<C, ::elastica::configuration::sequential_policy>;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of the parallel policy
        using ParallelPolicy = ::elastica::configuration::parallel_policy;
        //**********************************************************************

        using P::P;
      };
      //************************************************************************
#endif

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
