#pragma once

#define ELASTICA_BLOCK_REGISTER_INITIALIZED_TYPE(TAG, STAGGER, RANK, INIT) \
  using TAG = typename Traits::template RegisterInitializedType<           \
      struct TAG, typename Traits::STAGGER, Traits::template RANK, INIT>

#define ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(TAG, STAGGER, RANK) \
  using TAG = typename Traits::template RegisterComputedType<     \
      struct TAG, typename Traits::STAGGER, Traits::template RANK>
