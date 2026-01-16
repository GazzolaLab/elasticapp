//==============================================================================
/*!
//  \file
//  \brief Transfers information from primitives to systems and vice-versa
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
#include <iterator>

#include "ErrorHandling/Assert.hpp"
//
#include "Simulator/Environments/Primitives/PrimitivesCollection.hpp"
#include "Simulator/Environments/Primitives/TypeTraits.hpp"
//
#include "TransferTags.hpp"
#include "TransferWizard.hpp"
#include "Types.hpp"
//
#include "Simulator/Utilities/Offsets.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/Size.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    class Transferer {
     protected:
      //************************************************************************
      template <typename PrimitiveType, typename BlockType, typename P,
                typename O>
      void from_dispatch(tmpl::type_<PrimitiveType> /*dispatch*/,
                         BlockType const& block, P& primitives,
                         O const& offsets) const {
        // 0. Get reference to a vector of primitives
        auto& primitive_collection = get_collection<PrimitiveType>(primitives);

        // IteratorType models random access iterator
        using DifferenceType = typename std::iterator_traits<decltype(begin(
            primitive_collection))>::difference_type;

        // 2. Get offset into the primitive array, makes it easily
        // parallelizable and thread safe instead of having an incremental
        // counter here

        // The assumption here is that the offset index is not too big for
        // the iterator of the stored container of primitives, hence we
        // static cast it

        // follows a C++ STL map syntax
        auto offset_into_primitives =
            get_offset<BlockType>(offsets).find(&block);
        ELASTICA_ASSERT(
            offset_into_primitives != end(get_offset<BlockType>(offsets)),
            "Invalid iterator beyond end of map, the current "
            "block was not registered during emplacement!");

        auto primitive_begin =
            (begin(primitive_collection) +
             static_cast<DifferenceType>(offset_into_primitives->second));

        // 3. loop over all slices ...
        // The slices are not light weight :/
        for (auto /*const*/ slice : block) {
          auto primitive_end = primitive_begin + static_cast<DifferenceType>(
                                                     ::elastica::size(slice));

          ELASTICA_ASSERT(primitive_end <= end(primitive_collection),
                          "Invalid iterator beyond end of collection");

          // 4. ... and transfer
          TransferWizard<std::remove_cvref_t<decltype(slice)>>::transfer(
              from, slice, to, primitive_begin, primitive_end);

          std::exchange(primitive_begin, primitive_end);
        }
      }
      //************************************************************************

      //************************************************************************
      template <typename BlockType, typename P, typename O>
      void from_dispatch(tmpl::type_<NoSuchType> /*dispatch*/, BlockType const&,
                         P&, O const&) const {
        // Don't do any work if the primitives required are no such primitive.
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Transfers information from the systems to the primitives
       * buffer
       *
       * \param simulator Simulator instance
       * \return void
       *
       * \note
       * Not marked noexcept because unordered_map operator[] can throw
       */
      template <typename Simulator_>
      void operator()(tags::from_t /* meta */, Simulator_& simulator) const
      /*noexcept*/ {
        using ::elastica::get_offset;
        using ::elastica::collision::get_collection;
        using ::elastica::collision::tt::PrimitiveTrait_t;
        using std::begin;
        using std::end;

        // this will be in parallel over blocks
        // This kernel gets profiled as part of environments, so use the untimed
        // version of block
        simulator.for_each_block_untimed(
            [this, &primitives = simulator.primitives(),
             &offsets =
                 std::as_const(simulator).offsets()](auto const& block) {
              using BlockType = std::remove_cvref_t<decltype(block)>;
              using PrimitiveType = PrimitiveTrait_t<BlockType>;
              this->from_dispatch(tmpl::type_<PrimitiveType>{}, block,
                                  primitives, offsets);
            });
      }
      //************************************************************************
     protected:
      //************************************************************************
      template <typename PrimitiveType, typename BlockType, typename P,
                typename O>
      void to_dispatch(tmpl::type_<PrimitiveType> /*dispatch*/,
                       BlockType& block, P const& primitives,
                       O const& offsets) const {
        // 0. Get reference to a vector of primitives
        auto const& primitive_collection =
            get_collection<PrimitiveType>(primitives);

        // IteratorType models random access iterator
        using DifferenceType = typename std::iterator_traits<decltype(cbegin(
            primitive_collection))>::difference_type;

        // 2. Get offset into the primitive array, makes it easily
        // parallelizable and thread safe instead of having an incremental
        // counter here

        // The assumption here is that the offset index is not too big for
        // the iterator of the stored container of primitives, hence we
        // static cast it
        auto offset_into_primitives =
            get_offset<BlockType>(offsets).find(&block);
        ELASTICA_ASSERT(
            offset_into_primitives != end(get_offset<BlockType>(offsets)),
            "Invalid iterator beyond end of map, the current "
            "block was not registered during emplacement!");

        auto primitive_begin =
            (cbegin(primitive_collection) +
             static_cast<DifferenceType>(offset_into_primitives->second));

        // 3. loop over all slices ...
        for (auto slice : block) {
          auto primitive_end = primitive_begin + static_cast<DifferenceType>(
                                                     ::elastica::size(slice));

          ELASTICA_ASSERT(primitive_end <= cend(primitive_collection),
                          "Invalid iterator beyond end of collection");

          // 4. ... and transfer
          TransferWizard<decltype(slice)>::transfer(
              to, slice, from, primitive_begin, primitive_end);

          std::exchange(primitive_begin, primitive_end);
        }
      }
      //************************************************************************

      //************************************************************************
      template <typename BlockType, typename P, typename O>
      void to_dispatch(tmpl::type_<NoSuchType> /*dispatch*/, BlockType&,
                       P const&, O const&) const {
        // Don't do any work if the primitives required are no such primitive.
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Transfers information to the systems from the primitives
       * buffer
       *
       * \param simulator Simulator instance
       * \return void
       *
       * \note
       * Not marked noexcept because unordered_map operator[] can throw
       */
      template <typename Simulator_>
      void operator()(tags::to_t /* meta */, Simulator_& simulator) const
      /*noexcept*/ {
        using ::elastica::get_offset;
        using ::elastica::collision::get_collection;
        using ::elastica::collision::tt::PrimitiveTrait_t;
        using std::cbegin;
        using std::cend;

        // this will be in parallel over blocks
        // This kernel gets profiled as part of environments, so use the untimed
        // version of block
        simulator.for_each_block_untimed(
            [this, &primitives = std::as_const(simulator).primitives(),
             &offsets = std::as_const(simulator).offsets()](auto& block) {
              using BlockType = std::remove_cvref_t<decltype(block)>;
              using PrimitiveType = PrimitiveTrait_t<BlockType>;
              this->to_dispatch(tmpl::type_<PrimitiveType>{}, block, primitives,
                                offsets);
            });
      }
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica