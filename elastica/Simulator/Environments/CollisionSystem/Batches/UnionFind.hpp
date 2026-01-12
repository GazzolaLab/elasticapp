//==============================================================================
/*!
//  \file
//  \brief Implementation of the 'Union Find' batch generation algorithm
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
**
**  Project home: https://www.cs10.tf.fau.de/research/software/pe/
**
**  Copyright (C) 2009 Klaus Iglberger
**
**  This file is part of pe.
**
**  pe is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License as published by the Free Software
** Foundation, either version 3 of the License, or (at your option) any later
** version.
**
**  pe is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License along with
** pe. If not, see <http://www.gnu.org/licenses/>.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>
//
//
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Batches/UnionFindExecutionPolicy/ParallelExecutionPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/UnionFindExecutionPolicy/SequentialExecutionPolicy.hpp"
#include "Utilities/Logging.hpp"

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
       * The UnionFind class represents the implementation of the 'Union Find'
       * batch generation algorithm. This algorithm is used to create
       * independent batches of contacts that can be solved independent/in
       * parallel to each other.
       *
       * \tparam C Type of the configuration
       */
      template <typename C>
      class UnionFind
          : protected UnionFindOp<
                C, typename C::KernelExecutionPolicy::ExecutionPolicy> {
       protected:
        //**Type definitions****************************************************
        //! Execution type
        using Op =
            UnionFindOp<C, typename C::KernelExecutionPolicy::ExecutionPolicy>;
        using Op::generate_batches;
        //**********************************************************************

       public:
        //**Type definitions****************************************************
        //! Type of the configuration.
        using CollisionConfig = C;
        //! Type of this UnionFind instance.
        using This = UnionFind<CollisionConfig>;
        //! Type of the batching algorithm
        using BatchingAlgorithm = This;
        //**********************************************************************

       public:
        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        UnionFind() = default;
        UnionFind(UnionFind&&) noexcept = default;
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~UnionFind() = default;
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
         */
        template <typename Contacts, typename Batches>
        void operator()(Contacts& contacts, Batches& batches) const {
          using namespace ::elastica::logging;
          log<debug>([](auto& log) {
            log << "   Generating batches of contacts via the UnionFind "
                   "algorithm ...";
          });

          // something
          generate_batches(contacts, batches);

          log<debug>([](auto& log) {
            log << "   Finished generating batches of contacts via the "
                   "UnionFind algorithm ...";
          });

          // using namespace ::elastica::logging;
          // log<debug>([](auto& log) {
          //   if (not batches.empty()) {
          //     log << "   Batch generation...\n";
          //     for (std::size_t i = 0UL; i < batches.size(); ++i) {
          //       Batch& batch(batches[i]);
          //       log << "      Batch " << i + 1 << " = (";
          //       for (std::size_t j = 0UL; j < batch.size(); ++j)
          //         log << " " << batch[j]->getID();
          //       log << " )\n";
          //     }
          //   }
          // });
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Compile time checks*************************************************
        /*! \cond ELASTICA_INTERNAL */
        // static_assert(
        //     std::is_same_v<
        //         This, typename CollisionConfig::template BatchGenerator<C>>,
        //     R"error(Union-find meta-failure, contact the developers!)error");
        // /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
