#pragma once

namespace elastica {

  namespace collision {

    ////////////////////////////////////////////////////////////////////////////
    // Forward declarations of transfer types
    ////////////////////////////////////////////////////////////////////////////

    // The main classes

    template <typename SystemTag>
    struct TransferTraits;
    template <typename SystemTag>
    struct Transfer;
    template <typename SystemTag>
    struct TransferHelper;

    // Details
    namespace tags{
      struct from_t;
      struct to_t;
    }

  }  // namespace collision

}  // namespace elastica
