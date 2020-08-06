#pragma once
#include <algorithm>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/TransferTraits.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/TypeTraits.hpp"
#include "elastica/Utilities/Requires.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {


    // Provide generic implementation here
    template <typename SystemTag>
    struct Transfer : public TransferTraits<SystemTag> {
      using Parent = TransferTraits<SystemTag>;
      using typename Parent::SystemCollectionType;
      using typename Parent::from_t;
      using typename Parent::PrimitiveType;
      using typename Parent::to_t;

      static_assert(
          tt::IsCollectionDefaultTransferable<SystemCollectionType>::value,
          "(Default) Transfer failed because the Collection fails to provide "
          "suitable"
          "members. Either provide all members necessary: -get_dimensions -"
          "get_position -get_velocity ... or "
          "provide a a specialization to the Transfer class");

      static_assert(
          tt::IsPrimitiveDefaultTransferable<PrimitiveType>::value,
          "(Default) Transfer failed because the primitive fails to provide "
          "suitable"
          "members. Either derive from RigidBodyPrimitive and provide a "
          "`transfer` function"
          "or provide a a specialization to the Transfer class");

      // TODO Requires
      template <typename PrimIter>
      static void do_transfer(from_t /* meta */,
                              SystemCollectionType const& collection,
                              to_t /* meta */, PrimIter start, PrimIter end) {
        // Generic implementation to fall back to, can use public members
        ELASTICA_ASSERT(end > start, "Oops");
        ELASTICA_ASSERT(
            collection.size() == static_cast<std::size_t>(end - start), "Oops");

        // Uses primitives transfer member
        std::for_each(
            start, end, [&collection, index = 0UL](auto& primitive) mutable {
              primitive.transfer(collection.get_dimensions(index),
                                 collection.get_position(index),
                                 collection.get_velocity(index),
                                 collection.get_angular_velocity(index));
              ++index;
            });
      };

      template <typename ConstPrimIter>
      static void do_transfer(to_t /* meta */, SystemCollectionType& collection,
                              from_t /* meta */, ConstPrimIter start,
                              ConstPrimIter end) {
        // Generic implementation to fall back to, can use public members
        ELASTICA_ASSERT(end > start, "Oops");
        ELASTICA_ASSERT(
            collection.size() == static_cast<std::size_t>(end - start), "Oops");

        // Friended
        std::for_each(start, end,
                      [&collection, index = 0UL](auto& primitive) mutable {
                        collection.get_force(index) += primitive.get_force();
                        collection.get_torque(index) += primitive.get_torque();
                        ++index;
                      });
      };
    };

  }  // namespace collision

}  // namespace elastica