//=================================================================================================
/*!
 *  \file pe/util/MemoryPool.h
 *  \brief Header file for the memory pool class
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <vector>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Utilities/Byte.hpp"
#include "elastica/Utilities/NonCopyable.hpp"

namespace elastica {

  //=================================================================================================
  //
  //  CLASS DEFINITION
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Memory pool for small objects.
   * \ingroup util
   *
   * The memory pool efficiently improves the performance of dynamic memory
   * allocations for small objects. By allocating a large block of memory that
   * can be dynamically assigned to small objects, the memory allocation is
   * reduced from a few hundred cycles to only a few cycles.\n The memory pool
   * is build from memory blocks of type Block, which hold the memory for a
   * specified number of objects. The memory of these blocks is managed as a
   * single free list.
   */
  template <typename Type, std::size_t BlockSize>
  class MemoryPool : private NonCopyable {
   private:
    //**union
    // FreeObject****************************************************************************
    /*!\brief A single element of the free list of the memory pool.
     */
    union FreeObject {
      FreeObject* next_;  //!< Pointer to the next free object.
      byte dummy_[sizeof(
          Type)];  //!< Dummy array to create an object of the appropriate size.
    };
    //**********************************************************************************************

    //**struct
    // Block********************************************************************************
    /*!\brief Memory block within the memory bool.
    //
    // One memory block holds the memory for exactly \a BlockSize objects of
    type \a Type.
    */
    struct Block {
     public:
      //**Memory management
      // functions**************************************************************
      /*!\name Memory management functions */
      //@{
      void init();
      void free();
      //@}
      //*******************************************************************************************

      //**Member
      // variables*************************************************************************
      /*!\name Member variables */
      //@{
      FreeObject* rawMemory_;  //!< Allocated memory pool of the block.
                               //@}
      //*******************************************************************************************
    };
    //**********************************************************************************************

    //**Type
    // definitions****************************************************************************
    using Blocks = std::vector<Block>;  //!< Vector of memory blocks.
    //**********************************************************************************************

   public:
    //**Constructor*********************************************************************************
    /*!\name Constructor */
    //@{
    inline MemoryPool();
    //@}
    //**********************************************************************************************

    //**Destructor**********************************************************************************
    /*!\name Destructor */
    //@{
    inline ~MemoryPool();
    //@}
    //**********************************************************************************************

    //**Memory management
    // functions*****************************************************************
    /*!\name Memory management functions */
    //@{
    inline void* malloc();
    inline void free(void* rawMemory);
    //@}
    //**********************************************************************************************

   private:
    //**Memory management
    // functions*****************************************************************
    /*!\name Memory management functions */
    //@{
    inline bool checkMemory(FreeObject* toRelease) const;
    //@}
    //**********************************************************************************************

    //**Member
    // variables****************************************************************************
    /*!\name Member variables */
    //@{
    FreeObject* freeList_;  //!< Head of the free list.
    Blocks blocks_;         //!< Vector of available memory blocks.
                            //@}
    //**********************************************************************************************
  };

  //=================================================================================================
  //
  //  CLASS MEMORYPOOL::BLOCK
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Initialization of a memory block.
   *
   * \return void
   *
   * The \a init function allocates a single memory block for \a BlockSize
   * objects of type \a Type. This memory is already prepared for the inclusion
   * in the free list of the memory pool.
   */
  template <typename Type, size_t BlockSize>
  inline void MemoryPool<Type, BlockSize>::Block::init() {
    rawMemory_ = new FreeObject[BlockSize];
    for (size_t i = 0; i < BlockSize - 1; ++i) {
      rawMemory_[i].next_ = &rawMemory_[i + 1];
    }
    rawMemory_[BlockSize - 1].next_ = nullptr;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Release of the entire memory block.
   *
   * \return void
   */
  template <typename Type, size_t BlockSize>
  inline void MemoryPool<Type, BlockSize>::Block::free() {
    delete[] rawMemory_;
  }
  //*************************************************************************************************

  //=================================================================================================
  //
  //  CLASS MEMORYPOOL
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Constructor of the memory pool.
   */
  template <typename Type, size_t BlockSize>
  inline MemoryPool<Type, BlockSize>::MemoryPool() {
    blocks_.push_back(Block());
    Block& block = blocks_.back();
    block.init();
    freeList_ = block.rawMemory_;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Destructor of the memory pool.
   */
  template <typename Type, size_t BlockSize>
  inline MemoryPool<Type, BlockSize>::~MemoryPool() {
    for (Block& block_ : blocks_) {
      block_.free();
    }
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Allocation of raw memory for an object of type \a Type.
   *
   * \return Pointer to the raw memory.
   */
  template <typename Type, size_t BlockSize>
  inline void* MemoryPool<Type, BlockSize>::malloc() {
    if (!freeList_) {
      blocks_.push_back(Block());
      Block& block = blocks_.back();
      block.init();
      freeList_ = block.rawMemory_;
    }

    void* ptr = freeList_;
    freeList_ = freeList_->next_;
    return ptr;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Deallocation of raw memory for an object of type \a Type.
   *
   * \param rawMemory Pointer to the raw memory.
   * \return void
   */
  template <typename Type, size_t BlockSize>
  inline void MemoryPool<Type, BlockSize>::free(void* rawMemory) {
    auto* ptr = reinterpret_cast<FreeObject*>(rawMemory);
    ELASTICA_ASSERT(checkMemory(ptr), "Memory pool check failed");
    ptr->next_ = freeList_;
    freeList_ = ptr;
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Performing a number of checks on the memory to be released.
   *
   * \param toRelease Pointer to the memory to be released.
   * \return \a true if the memory check succeeds, \a false if an error is
   * encountered.
   */
  template <typename Type, size_t BlockSize>
  inline bool MemoryPool<Type, BlockSize>::checkMemory(
      FreeObject* toRelease) const {
    for (typename Blocks::const_iterator it = blocks_.begin();
         it != blocks_.end(); ++it) {
      // Range check
      if (toRelease >= it->rawMemory_ &&
          toRelease < it->rawMemory_ + BlockSize) {
        // Alignment check
        const byte* const ptr1(reinterpret_cast<const byte*>(toRelease));
        const byte* const ptr2(reinterpret_cast<const byte*>(it->rawMemory_));

        if ((ptr1 - ptr2) % sizeof(FreeObject) != 0)
          return false;

        // Duplicate free check : if memory to be released is located in the free
        // pool of the blocks, then there's an issue
        FreeObject* ptr(freeList_);
        while (ptr) {
          if (ptr == toRelease)
            return false;
          ptr = ptr->next_;
        }

        return true;
      }
    }
    return false;
  }
  //*************************************************************************************************

}  // namespace elastica
