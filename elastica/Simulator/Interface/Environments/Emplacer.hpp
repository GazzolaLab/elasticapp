//==============================================================================
/*!
//  \file
//  \brief Emplaces primitives into a simulator
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
#include <mutex>
//
#include "Simulator/Environments/Primitives/PrimitivesCollection.hpp"
#include "Simulator/Environments/Primitives/TypeTraits.hpp"
//
#include "Simulator/Utilities/Offsets.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    class Emplacer {
     protected:
      //************************************************************************
      /*!\brief Metafunction to get type of Block from the slice
       */
      template <typename SliceType>
      inline static constexpr auto block_t(SliceType volatile const&) noexcept
          -> typename SliceType::ParentBlock;
      //************************************************************************

      //************************************************************************
      /*!\brief Reset primitive dispatch : generic primitive version
       */
      template <typename PrimitiveType, typename System, typename Buffer>
      static void reserve_primitives_dispatch(
          tmpl::type_<PrimitiveType> /*dispatch*/, System const& system,
          Buffer& reserve_buffer) {
        using ::elastica::get_offset;
        get_offset<PrimitiveType>(reserve_buffer) += ::elastica::size(system);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Reset primitive dispatch : no primitive version
       */
      template <typename System, typename Buffer>
      static void reserve_primitives_dispatch(
          tmpl::type_<NoSuchType> /*dispatch*/, System const&,
          Buffer&) noexcept {}
      //************************************************************************

      //************************************************************************
      /*!\brief Reserves appropriate capacity for the primitives after checking
       * all systems within the simulator
       *
       * \note
       * Requires simulator to be constructed with the add on
       * elastica::interface::environments::WithPrimitives
       * else, a compile-time error is thrown
       * can take simulator as argument
       */
      template <typename Simulator_>
      static void reserve_primitives(Simulator_& simulator) {
        // use public API
        using ::elastica::get_offset;
        using ::elastica::collision::tt::PrimitiveTrait_t;

        using PrimitivesBuffer = typename Simulator_::Primitives_;
        using Primitives = typename PrimitivesBuffer::PrimitiveList;
        using ReserveBuffer = Offsets<Primitives>;
        ReserveBuffer reserve_buffer;

        // 1. count size to reserved, use a mutex in case for_each is parallel
        std::mutex reserve_buffer_mutex;

        // timing is not necessary since it is a part of the environments module
        simulator.for_each_system_untimed([&](auto const& system) noexcept {
          std::lock_guard<std::mutex> guard(reserve_buffer_mutex);
          reserve_primitives_dispatch(
              tmpl::type_<PrimitiveTrait_t<decltype(block_t(system))>>{},
              system, reserve_buffer);
        });

        // 2. actually reserve size
        auto reserve = [reserve_size = std::move(reserve_buffer)](
                           auto& primitive_container) {
          using P = typename std::remove_cvref_t<
              decltype(primitive_container)>::value_type;
          primitive_container.reserve(get_offset<P>(reserve_size));
        };

        std::apply(
            [reserve](auto&... primitive_container) {
              EXPAND_PACK_LEFT_TO_RIGHT(reserve(primitive_container));
            },
            simulator.primitives().data());
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Adds primitive dispatch generic primitive version
       */
      template <typename PrimitiveType, typename BlockType, typename P,
                typename O>
      static void add_primitives_dispatch(
          tmpl::type_<PrimitiveType> /*dispatch*/, BlockType& block,
          P& primitives, O& offsets) {
        using ::elastica::get_offset;
        using ::elastica::collision::get_collection;

        // 0. Get reference to a vector of primitives
        auto& primitive_collection = get_collection<PrimitiveType>(primitives);

        // 2. Store the number of currently referenced primitives to the
        // offset indices for indexing next
        get_offset<BlockType>(offsets)[&block] =
            ::elastica::size(primitive_collection);

        std::size_t sid_base = get_offset<BlockType>(offsets)[&block];

        // slice can be copied for now
        for (auto slice : block) {
          std::size_t const end_idx = ::elastica::size(slice);

          for (std::size_t idx = 0UL; idx < end_idx; ++idx) {
            id_t sid = sid_base + idx;
            id_t uid = idx;  // user id, not important, we allocate it to
            // subslice for now
            const bool finite(true);

            // 2. Create primitives, no need to transfer yet (achieved in
            // first simulation step) needs to be pliable to future change
            primitive_collection.emplace_back(sid, uid, finite,
                                              slice.get_material(idx));
          }
          sid_base += end_idx;
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Adds primitives dispatch no primitive version
       */
      template <typename BlockType, typename P, typename O>
      static void add_primitives_dispatch(tmpl::type_<NoSuchType> /*dispatch*/,
                                          BlockType&, P&, O&) {
        // Don't do any work if the primitives required are no such primitive.
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Adds primitives from one collection (i.e. any iterable) of
       * blocks of a physical system
       */
      template <typename Simulator_>
      static void add_primitives(Simulator_& simulator) {
        using ::elastica::get_offset;
        using ::elastica::collision::get_collection;
        using ::elastica::collision::tt::PrimitiveTrait_t;

        std::mutex primitive_buffer_mutex;

        /* Devnote : the actual pattern is a std::transform but we need to
         * chunk the slices itself, so for_each is more readable */
        // timing is not necessary since it is a part of the environments module
        simulator.for_each_block_untimed(
            [&primitives = simulator.primitives(),
             &offsets = simulator.offsets(),
             &primitive_buffer_mutex](auto& block) {
              std::lock_guard<std::mutex> guard(primitive_buffer_mutex);

              using BlockType = std::remove_cvref_t<decltype(block)>;
              using PrimitiveType = PrimitiveTrait_t<BlockType>;
              add_primitives_dispatch(tmpl::type_<PrimitiveType>{}, block,
                                      primitives, offsets);
            });
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Creates primitives based on the simulators collection of
       * physical system blocks
       */
      template <typename Simulator_>
      static void create_primitives(Simulator_& simulator) {
        reserve_primitives(simulator);
        add_primitives(simulator);
      }
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica