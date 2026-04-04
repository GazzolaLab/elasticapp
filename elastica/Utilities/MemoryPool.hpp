//=================================================================================================
/*!
 *  \file elastica/Utilities/MemoryPool.hpp
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

//******************************************************************************
// Includes
//******************************************************************************

#include <utility>  // swap and move
#include <vector>

#include "ErrorHandling/Assert.hpp"
#include "Utilities/Byte.hpp"
#include "Utilities/NonCopyable.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Memory pool for small objects.
   * \ingroup util
   *
   * The memory pool efficiently improves the performance of dynamic memory
   * allocations for small objects. By allocating a large block of memory that
   * can be dynamically assigned to small objects, the memory allocation is
   * reduced from a few hundred cycles to only a few cycles.
   *
   * The memory pool is build from memory blocks of type Block, which hold the
   * memory for a specified number of objects. The memory of these blocks is
   * managed as a single free list.
   */
  template <typename Type, std::size_t BlockSize>
  class MemoryPool : private NonCopyable {
   private:
    //**union FreeObject********************************************************
    /*!\brief A single element of the free list of the memory pool.
     */
    union FreeObject {
      //! Pointer to the next free object.
      FreeObject* next_;
      //! Dummy array to create an object of the appropriate size.
      byte dummy_[sizeof(Type)];
    };
    //**Struct Block************************************************************
    /*!\brief Memory block within the memory pool.
     *
     * One memory block holds the memory for exactly \a BlockSize objects of
     * type \a Type.
     */
    struct Block {
     public:
      //**Memory management functions*******************************************
      /*!\name Memory management functions */
      //@{
      void init();
      void free();
      //@}
      //************************************************************************

      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Allocated memory pool of the block.
      FreeObject* rawMemory_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //**Type definitions********************************************************
    using Blocks = std::vector<Block>;  //!< Vector of memory blocks.
    //**************************************************************************

   public:
    //**Constructor*************************************************************
    /*!\name Constructor */
    //@{
    inline MemoryPool();
    MemoryPool(MemoryPool&&) noexcept;
    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    inline ~MemoryPool();
    //@}
    //**************************************************************************

    friend void swap(MemoryPool& first, MemoryPool& second) {
      using std::swap;  // bring in swap for built-in types

      swap(first.blocks_, second.blocks_);
      swap(first.free_list_, second.free_list_);
    }

    //**Memory management
    // functions*****************************************************************
    /*!\name Memory management functions */
    //@{
    inline void* malloc();
    inline void free(void* rawMemory);
    //@}
    //**********************************************************************************************

   private:
    //**Memory management functions*********************************************
    /*!\name Memory management functions */
    //@{
    inline bool checkMemory(FreeObject* toRelease) const;
    //@}
    //**************************************************************************

    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    FreeObject* free_list_;  //!< Head of the free list.
    Blocks blocks_;          //!< Vector of available memory blocks.
    //@}
    //**************************************************************************
  };
  //****************************************************************************

  //============================================================================
  //
  //  CLASS MEMORYPOOL::BLOCK
  //
  //============================================================================

  //****************************************************************************
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
  //****************************************************************************

  //****************************************************************************
  /*!\brief Release of the entire memory block.
   *
   * \return void
   */
  template <typename Type, size_t BlockSize>
  inline void MemoryPool<Type, BlockSize>::Block::free() {
    delete[] rawMemory_;
  }
  //****************************************************************************

  //============================================================================
  //
  //  CLASS MEMORYPOOL
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Constructor of the memory pool.
   */
  template <typename Type, size_t BlockSize>
  inline MemoryPool<Type, BlockSize>::MemoryPool() {
    blocks_.push_back(Block());
    Block& block = blocks_.back();
    block.init();
    free_list_ = block.rawMemory_;
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Constructor of the memory pool.
   */
  template <typename Type, size_t BlockSize>
  MemoryPool<Type, BlockSize>::MemoryPool(MemoryPool&& other) noexcept
      : blocks_(std::move(other.blocks_)), free_list_(other.free_list_) {
    other.free_list_ = nullptr;
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Destructor of the memory pool.
   */
  template <typename Type, size_t BlockSize>
  inline MemoryPool<Type, BlockSize>::~MemoryPool() {
    for (auto& block_ : blocks_) {
      block_.free();
    }
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Allocation of raw memory for an object of type \a Type.
   *
   * \return Pointer to the raw memory.
   */
  template <typename Type, size_t BlockSize>
  inline void* MemoryPool<Type, BlockSize>::malloc() {
    if (!free_list_) {
      blocks_.push_back(Block());
      Block& block = blocks_.back();
      block.init();
      free_list_ = block.rawMemory_;
    }

    void* ptr = free_list_;
    free_list_ = free_list_->next_;
    return ptr;
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Deallocation of raw memory for an object of type \a Type.
   *
   * \param rawMemory Pointer to the raw memory.
   * \return void
   */
  template <typename Type, size_t BlockSize>
  inline void MemoryPool<Type, BlockSize>::free(void* rawMemory) {
    auto* ptr = reinterpret_cast<FreeObject*>(rawMemory);
    ELASTICA_ASSERT(checkMemory(ptr), "Memory pool check failed");
    ptr->next_ = free_list_;
    free_list_ = ptr;
  }
  //****************************************************************************

  //****************************************************************************
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
      if (toRelease >= it->rawMemory_ and
          toRelease < it->rawMemory_ + BlockSize) {
        // Alignment check
        byte const * const ptr1(reinterpret_cast<byte const *>(toRelease));
        byte const * const ptr2(reinterpret_cast<byte const *>(it->rawMemory_));

        if ((ptr1 - ptr2) % sizeof(FreeObject) != 0)
          return false;

        // Duplicate free check : if memory to be released is located in the
        // free pool of the blocks, then there's an issue
        FreeObject* ptr(free_list_);
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
  //****************************************************************************

}  // namespace elastica
