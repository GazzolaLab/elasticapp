#pragma once

#include <cstddef>
#include <type_traits>

#include "Utilities/MemoryPool.hpp"

namespace elastica {

  template <typename T, std::size_t BlockSize = 8192UL>
  class MemoryPoolAllocator {
   public:
    /* Member types */
    // STL allocator requirements
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_pointer = T const*;
    using const_reference = T const&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    using Pool = MemoryPool<T, BlockSize>;

    //**rebind class definition*************************************************
    /*!\brief Implementation of the AlignedAllocator rebind mechanism.
     */
    template <typename U>
    struct rebind {
      using other = MemoryPoolAllocator<U>;  //!< Type of the other allocator.
    };
    //**************************************************************************

    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    MemoryPoolAllocator() noexcept : pool_(){};  // start from a fresh pool
    // MemoryPoolAllocator(Pool p) noexcept : pool_(std::move(p)){};
    MemoryPoolAllocator(MemoryPoolAllocator const &) noexcept
        : MemoryPoolAllocator(){};
    MemoryPoolAllocator(MemoryPoolAllocator&& other) noexcept
        : pool_(std::move(other.pool_)){};
    template <class U>
    MemoryPoolAllocator(MemoryPoolAllocator<U> const & memoryPool) noexcept
        : MemoryPoolAllocator(){};
    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~MemoryPoolAllocator() noexcept = default;
    //@}
    //**************************************************************************

    //**Assignment**************************************************************
    /*!\name Assignment operators */
    //@{
    MemoryPoolAllocator& operator=(MemoryPoolAllocator const & memoryPool) =
        delete;
    MemoryPoolAllocator& operator=(MemoryPoolAllocator&& other) noexcept {
      if (this != &other) {
        std::swap(pool_, other.pool_);
      }
      return *this;
    };
    //@}
    //**************************************************************************

    //**Storage and lifetime****************************************************
    /*!\name Storage and lifetime functions */
    //@{

    //**************************************************************************
    /*!\brief Allocates aligned memory for the specified number of objects.
    //
    // \param n The number of objects to be allocated.
    // \return Pointer to the newly allocated memory.
    //
    // This function allocates a chunk of memory for the specified number of
    // objects of type \a T. It allocates one block at a time, so
    */
    inline pointer allocate(size_type n = 1){
        // allocate one object at a time. n and hint are ignored
    };
    inline void deallocate(pointer p, size_type n = 1);
    size_type max_size() const noexcept;
    template <class U, class... Args>
    void construct(U* p, Args&&... args);
    template <class U>
    void destroy(U* p);
    //@}
    //**************************************************************************

    Pool pool_;
  };

}  // namespace elastica