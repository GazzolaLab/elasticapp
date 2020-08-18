#pragma once

#include <iterator>
#include <type_traits>

//// Declare all types
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/Types.hpp"
////
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/Transfer.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/TransferTraits.hpp"

namespace elastica {

  namespace collision {

    // This is the interface class from the CollisionSystem
    // as it does a bunch of type checking first
    template <typename SystemTag>
    struct TransferHelper : public TransferTraits<SystemTag> {
      using Parent = TransferTraits<SystemTag>;
      using typename Parent::from_t;
      using typename Parent::PrimitiveType;
      using typename Parent::SystemCollectionType;
      using typename Parent::to_t;

      // //// https://stackoverflow.com/a/10546077
      template <typename Iterator>
      struct IsConstIterator {
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using value_type = typename std::remove_pointer<pointer>::type;
        static constexpr bool value = std::is_const<value_type>::value;
      };

      template <typename PrimitiveIterator>
      static void reset_forces_and_torques(PrimitiveIterator start,
                                           PrimitiveIterator end) {
        std::for_each(start, end, [](auto& primitive) -> void {
          primitive.reset_forces_and_torques();
        });
      }

      template <typename PrimitiveIterator>
      static void transfer(from_t /* meta */,
                           SystemCollectionType const& collection,
                           to_t /* meta */, PrimitiveIterator start,
                           PrimitiveIterator end) {
        using namespace tags;
        static_assert(std::is_same<PrimitiveType,
                                   typename std::iterator_traits<
                                       PrimitiveIterator>::value_type>::value,
                      "Wrong transfer detected!");

        // Reset previous time-steps effects
        reset_forces_and_torques(start, end);

        // Generic implementation to fall back to, can use public members
        Transfer<SystemTag>::template do_transfer<PrimitiveIterator>(
            from, collection, to, start, end);
      }

      template <typename ConstPrimitiveIterator>
      static void transfer(to_t /* meta */, SystemCollectionType& collection,
                           from_t /* meta */, ConstPrimitiveIterator start,
                           ConstPrimitiveIterator end) {
        using namespace tags;

        using IsPrimitiveIteratorConst =
            IsConstIterator<ConstPrimitiveIterator>;
        // gives a const P
        using value_type = typename IsPrimitiveIteratorConst::value_type;

        // Check first
        static_assert(IsPrimitiveIteratorConst::value,
                      "Const correctness failed!");
        static_assert(
            std::is_same<PrimitiveType,
                         typename std::remove_const<value_type>::type>::value,
            "Oops, wrong primitive!");

        Transfer<SystemTag>::template do_transfer<ConstPrimitiveIterator>(
            to, collection, from, start, end);
      }
    };

  }  // namespace collision

}  // namespace elastica
