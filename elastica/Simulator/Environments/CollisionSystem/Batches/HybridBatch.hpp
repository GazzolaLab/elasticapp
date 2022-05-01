//==============================================================================
/*!
//  \file
//  \brief Implementation of an algorithm that is a hybrid between single batch
//  and union find algorithm
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

//
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
//
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Batches/SingleBatch.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/UnionFind.hpp"

namespace elastica {

  namespace collision {

    namespace batches {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Implementation of a hybrid batching algorithm between
       * 'Union Find' and 'Single Batch'  generation algorithms.
       * \ingroup batch_generation
       *
       * \details
       * The hybrid algorithm uses the Single Batch in case the execution needs
       * to be sequential, else it uses the UnionFind algorithm
       *
       * \tparam C Type of the configuration
       */
      template <typename C>
      class HybridBatch {
       protected:
        //**Type definitions****************************************************
        //! Single batcher type
        using SingleBatcher = SingleBatch<C>;
        //! Type of union finder
        using UnionFinder = UnionFind<C>;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of the configuration.
        using CollisionConfig = C;
        //! Type of this instance.
        using This = HybridBatch<CollisionConfig>;
        //! Type of the batching algorithm
        using BatchingAlgorithm = std::conditional_t<
            cpp17::is_same_v<typename C::KernelExecutionPolicy::ExecutionPolicy,
                             ::elastica::configuration::sequential_policy>,
            SingleBatcher, UnionFinder>;
        //**********************************************************************

       public:
        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        HybridBatch() = default;
        HybridBatch(HybridBatch&&) noexcept = default;
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~HybridBatch() = default;
        //@}
        //**********************************************************************

        //**Utility functions***************************************************
        /*!\name Utility functions */
        //@{

        //**********************************************************************
        /*!\brief Generate batches of contacts.
         *
         * \param contacts Contact container holding all existing contacts.
         * \param batches Batch container to be filled with the contacts.
         * \return void
         *
         * This function generates batches of contacts from the
         * contacts given in \a contacts.
         */
        template <typename Contacts, typename Batches>
        inline void operator()(Contacts& contacts, Batches& batches) const {
          batching_algorithm_(contacts, batches);
        }
        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        /*!\name Member variables */
        //@{
        //! The union find algorithm
        BatchingAlgorithm batching_algorithm_;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica