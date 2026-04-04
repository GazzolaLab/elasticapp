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
    //! Copy assignment is deleted (pool is not copyable).
    MemoryPoolAllocator& operator=(MemoryPoolAllocator const & memoryPool) =
        delete;
    //! Move assignment swaps the underlying memory pool with \a other.
    //! \param other Allocator to swap pools with.
    MemoryPoolAllocator& operator=(MemoryPoolAllocator&& other) noexcept {
      if (this != &other) {
        std::swap(pool_, other.pool_);
      }
      return *this;
    };
    //@}
    //**************************************************************************

    //**Storage and lifetime****************************************************
    //! \brief Allocates aligned memory for the specified number of objects.
    //! \param n The number of objects to be allocated.
    //! \return Pointer to the newly allocated memory.
    //! Allocates one block at a time from the pool (see implementation).
    inline pointer allocate(size_type n = 1){
        // allocate one object at a time. n and hint are ignored
    };
    //! \brief Deallocates memory obtained from allocate().
    //! \param p Pointer previously returned by allocate.
    //! \param n Number of objects (ignored; STL interface compatibility).
    inline void deallocate(pointer p, size_type n = 1);
    //! \brief Maximum number of \c T instances the allocator can handle in one call.
    size_type max_size() const noexcept;
    //! \brief Constructs \c U at address \a p.
    //! \param p Storage address.
    //! \param args Arguments forwarded to \c U's constructor.
    template <class U, class... Args>
    void construct(U* p, Args&&... args);
    //! \brief Destroys the object at \a p without releasing storage.
    //! \param p Pointer to the object to destroy.
    template <class U>
    void destroy(U* p);
    //**************************************************************************

    Pool pool_;
  };

}  // namespace elastica