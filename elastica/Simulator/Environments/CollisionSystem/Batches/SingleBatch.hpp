//==============================================================================
/*!
//  \file
//  \brief Implementation of the 'Single Batch' batch generation algorithm
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
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"
//
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  namespace collision {

    namespace batches {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Implementation of the 'Single batch' batch generation algorithm.
       * \ingroup batch_generation
       *
       * The SingleBatch class represents the implementation of the 'Single
       * batch' batch generation algorithm. This algorithm generates a single
       * batch of contacts without sorting them into independent groups of
       * contacts.
       *
       * \tparam C Type of the configuration
       */
      template <typename C>
      class SingleBatch : private NonCopyable {
       public:
        //**Type definitions****************************************************
        //! Type of the configuration.
        using CollisionConfig = C;
        //! Type of this SingleBatch instance.
        using This = SingleBatch<CollisionConfig>;
        //! Type of the batching algorithm
        using BatchingAlgorithm = This;
        //**********************************************************************

        //**Utility functions***************************************************
        /*!\name Utility functions */
        //@{
        template <typename Contacts, typename Batches>
        void operator()(Contacts& contacts, Batches& batches) const;
        //@}
        //**********************************************************************

       private:
        // //**Compile time
        // checks*************************************************
        // /*! \cond ELASTICA_INTERNAL */
        // static_assert(
        //     cpp17::is_same_v<This, typename CollisionConfig::BatchGenerator>,
        //     R"error(Single-batch meta-failure, contact the
        //     developers!)error");
        // /*! \endcond */
        // //**********************************************************************
      };
      //************************************************************************

      //========================================================================
      //
      //  UTILITY FUNCTIONS
      //
      //========================================================================

      //************************************************************************
      /*!\brief Generating a single batch of contacts.
       *
       * \param contacts Contact container holding all existing contacts.
       * \param batches Batch container to be filled with the contacts.
       * \return void
       *
       * This function generates a single batch of contacts from the contacts
       * given in \a contacts.
       */
      template <typename C>         // Type of the configuration
      template <typename Contacts,  // Contact container type
                typename Batches>   // Batch container type
      void SingleBatch<C>::operator()(Contacts& contacts,
                                      Batches& batches) const {
        using namespace ::elastica::logging;
        log<debug>([](auto& log) {
          log << "   Generating batches of contacts via the SingleBatch "
                 "algorithm ...";
        });

        // Early exit in case the contact container is empty
        if (contacts.empty())
          return;

        // Setting the batch indices
        const std::size_t c_size(contacts.size());
        for (std::size_t i = 0; i < c_size; ++i) {
          contacts[i].set_index(i);
        }

        // Creating a single batch and adding all contacts to this batch
        batches.resize(1);
        using std::begin;
        using std::end;
        // soft copies all the contacts since batches is a ref wrapper/pointer
        batches[0UL].assign(begin(contacts), end(contacts));

        log<debug>([](auto& log) {
          log << "   Finished generating batches of contacts via the "
                 "SingleBatch algorithm ...";
        });

        // do logging at one-level
        // using namespace ::elastica::logging;
        // log<debug>([](auto& log) {
        //   if (not batches.empty()) {
        //     log << "   Batch generation...\n";
        //     for (std::size_t i = 0UL; i < batches.size(); ++i) {
        //       Batch& batch(batches[i]);
        //       log << "      Batch " << i + 1 << " = (";
        //       for (std::size_t j = 0UL; j < batch.size(); ++j)
        //         log << " " << batch[j]->get_ID();
        //       log << " )\n";
        //     }
        //   }
        // });
      }
      //************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
