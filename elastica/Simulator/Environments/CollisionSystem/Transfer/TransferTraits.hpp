#pragma once

#include "elastica/Simulator/Environments/CollisionSystem/TransferTags.hpp"

namespace elastica {

  namespace collision {

    // Give the iterator etc. implementations
    template <typename SystemTag>
    struct TransferTraits {
      using from_t = tags::from_t;
      using to_t = tags::to_t;
      using SystemCollectionType = typename SystemTag::SystemCollectionType;
      using PrimitiveType = typename SystemTag::PrimitiveType;

      /* NOT POSSIBLE
       * Here we want to declare the types of PrimitiveIteratorType and
       * ConstPrimitiveIteratorType apriori like so
       *
       * using PrimitiveIteratorType =
       * std::iterator_traits<AddPrimtiveStorage<typename
       * BodyTag::PrimitiveType>::iterator>::value_type;
       *
       * , but this cannot be achieved
       * unless we know the type of the system storage (serial/parallel).
       */
    };

  }  // namespace collision

}  // namespace elastica
