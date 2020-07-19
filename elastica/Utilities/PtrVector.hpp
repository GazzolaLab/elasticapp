#pragma once

#include <memory>
#include <type_traits>

#include "ErrorHandling/Assert.hpp"
#include "Utilities/Policies/NoDelete.hpp"
#include "Utilities/Policies/OptimalGrowth.hpp"
#include "Utilities/Policies/PtrDelete.hpp"
#include "Utilities/PtrIterator.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {
  /*
  Requires<cpp17::conjunction_v<not std::is_pointer<T>::value,
      not std::is_reference<T>::value>> =
  nullptr>
   */

  template <typename T,                  // Type of storage
            typename D = NoDelete,       // Deletion Policy
            typename G = OptimalGrowth>  // Growth Policy
  class PtrVector {
   private:
    // Friend
    template <typename OT, typename OD, typename OG>
    friend class PtrVector;

   public:
    // Elastica Types
    using SelfType = PtrVector<T, D, G>;
    using ValueType = T*;
    using PointerType = T**;
    using ConstPointerType = T** const;
    using ReferenceType = T*&;
    using ConstReferenceType = T* const&;
    using SizeType = size_t;
    using Iterator = PtrIterator<T>;
    using ConstIterator =
        PtrIterator<const T>;  // Important, access to underlying const object
    using GrowthPolicy = G;
    using DeletionPolicy = D;

    // STL Container

    using value_type = ValueType;
    using pointer = PointerType;
    using const_pointer = ConstPointerType;
    using reference = ReferenceType;
    using const_reference = ConstReferenceType;
    using iterator = Iterator;
    using size_type = SizeType;
    typedef ptrdiff_t difference_type;

    // Forward declare CastIterators
    template <typename C>
    class CastIterator;
    template <typename C>
    class ConstCastIterator;

    /*
     * Constructors and destructors
     */
    PtrVector();
    explicit inline PtrVector(SizeType init_capacity);
    inline PtrVector(const PtrVector& other_pv);
    // TODO Check for ptr-type compatibility
    template <typename OT, typename OD, typename OG>
    /*
    , Requires<std::is_convertible<
          ValueType, typename PtrVector<OT, OD, OG>::ValueType>::value> =
          nullptr >
              */
    explicit inline PtrVector(const PtrVector<OT, OD, OG>& other_pv);
    PtrVector(PtrVector&& other_pv) = delete;
    inline ~PtrVector();

    /*
     * Assignment operators;
     */
    inline PtrVector& operator=(PtrVector other_pv);
    template <typename OT, typename OD, typename OG>
    /*
    , Requires<std::is_convertible<
          ValueType, typename PtrVector<OT, OD, OG>::ValueType>::value> =
          nullptr >
              */
    inline PtrVector& operator=(const PtrVector<OT, OD, OG>& other);
    PtrVector& operator=(PtrVector&& other_pv) = delete;
    template <typename OT, typename OD, typename OG>
    inline PtrVector& operator=(PtrVector<OT, OD, OG>&& other) = delete;

    /*
     * Interface functions
     */
    // Capacity functions
    bool empty() const noexcept;
    SizeType size() const noexcept;
    SizeType capacity() const noexcept;
    SizeType max_size() const noexcept;
    /*
    void reserve(SizeType new_size);
     */

    // Element Access operations
    inline ReferenceType operator[](SizeType index);
    inline ConstReferenceType operator[](SizeType index) const;
    // Cannot be noexcept, see https://stackoverflow.com/a/53413523
    inline ReferenceType front();
    inline ConstReferenceType front() const;
    inline ReferenceType back();
    inline ConstReferenceType back() const;

    // Iterators
    inline Iterator begin() noexcept;
    inline ConstIterator begin() const noexcept;
    //    template <typename C>
    //    inline CastIterator<C> begin() noexcept;
    //    template <typename C>
    //    inline ConstCastIterator<C> begin() const noexcept;

    inline ConstIterator cbegin() const noexcept;
    //    template <typename C>
    //    inline ConstCastIterator<C> cbegin() const noexcept;

    inline Iterator end() noexcept;
    inline ConstIterator end() const noexcept;
    //    template <typename C>
    //    inline CastIterator<C> end() noexcept;
    //    template <typename C>
    //    inline ConstCastIterator<C> end() const noexcept;

    inline ConstIterator cend() const noexcept;
    //    template <typename C>
    //    inline ConstCastIterator<C> cend() const noexcept;

    // Modifying operations
    void push_back(ValueType ptr);
    void pop_back();
    void release_back();

    template <typename IteratorType>
    inline void assign(IteratorType first, IteratorType last);
    void swap(PtrVector& other) noexcept;

    inline Iterator insert(ConstIterator pos, ValueType p);
    template <class IteratorType>
    inline Iterator insert(ConstIterator pos, IteratorType first,
                           IteratorType last);

    inline Iterator erase(ConstIterator pos);
    inline Iterator erase(ConstIterator first, ConstIterator last);

    inline Iterator release(ConstIterator pos);
    inline Iterator release(ConstIterator first, ConstIterator last);

    void reserve(SizeType requested_capacity);
    void shrink_to_fit();
    inline void clear();

    static_assert(
        not cpp17::conjunction_v<std::is_pointer<T>, std::is_reference<T>>,
        "Incorrigible type");

   private:
    static PointerType allocate(std::size_t size);
    static void deallocate(pointer buffer);
    void delete_element(ValueType p_elem);
    void delete_all();
    inline SizeType calculate_capacity(SizeType req_capacity) const noexcept;

   private:
    SizeType _size;
    SizeType _capacity;
    PointerType _begin;
    PointerType _end;
  };

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  inline PtrVector<T, D, G>::PtrVector()
      : _size(0UL), _capacity(0UL), _begin(nullptr), _end(nullptr) {}

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  inline PtrVector<T, D, G>::PtrVector(PtrVector::SizeType init_capacity)
      : _size(0UL),
        _capacity(init_capacity),
        _begin(allocate(init_capacity)),
        _end(_begin) {
#ifndef NDEBUG
    // Initialize a fill only in debug mode, else just reserve capacity
    std::uninitialized_fill_n(_begin, init_capacity, nullptr);
#endif
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  inline PtrVector<T, D, G>::PtrVector(const PtrVector& other_pv)
      : _size(other_pv._size),
        _capacity(other_pv._size),
        _begin(allocate(_capacity)),
        _end(_begin + _size) {
    try {
      for (auto i = static_cast<SizeType>(0); i < _size; ++i)
        new (_begin + i) ValueType(other_pv._begin[i]);
      //      _begin[i] = other_pv._begin[i];
    } catch (...) {
      delete_all();
      deallocate(_begin);
      throw;
    }
  }

  template <typename T,   // Type
            typename D,   // Deletion Policy
            typename G>   // Growth Policy
  template <typename OT,  // Other PV Type
            typename OD,  // Other PV Deletion Policy
            typename OG   // Other PV Growth Policy
            >
  /*,
  Requires<std::is_convertible<
      typename PtrVector<T, D, G>::ValueType,
      typename PtrVector<OT, OD, OG>::ValueType>::value>>
     */
  inline PtrVector<T, D, G>::PtrVector(const PtrVector<OT, OD, OG>& other_pv)
      : _size(other_pv._size),
        _capacity(other_pv._size),
        _begin(allocate(_capacity)),
        _end(_begin + _size) {
    try {
      for (auto i = static_cast<SizeType>(0); i < _size; ++i)
        new (_begin + i) ValueType(other_pv._begin[i]);
      //      _begin[i] = other_pv._begin[i];
    } catch (...) {
      delete_all();
      deallocate(_begin);
      throw;
    }
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  inline PtrVector<T, D, G>::~PtrVector() {
    delete_all();
    deallocate(_begin);
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  inline PtrVector<T, D, G>& PtrVector<T, D, G>::operator=(PtrVector other) {
    // Note : Copy-and-swap idiom may not be a good idea here as
    // we may  already have enough capacity to hold the contents of the
    // other vector, in which case, an additional copy is no good

    // TODO Retain for now, think of another strategy later

    other.swap(*this);
    return *this;
  }

  template <typename T,   // Type
            typename D,   // Deletion Policy
            typename G>   // Growth Policy
  template <typename OT,  // Other PV Type
            typename OD,  // Other PV Deletion Policy
            typename OG   // Other PV Growth Policy
            >
  inline PtrVector<T, D, G>& PtrVector<T, D, G>::operator=(
      const PtrVector<OT, OD, OG>& other) {
    // 0. Self assignment not possible
    if (other._size > _capacity) {
      // 1. First increase the capacity of the current container
      // to fit size elements
      // For exception-free code, reserve a buffer and then copy
      // from it.
      PointerType new_begin(allocate(other._size));
      _end = std::uninitialized_copy(_begin, _end, new_begin);
      std::swap(_begin, new_begin);

      // Let's dellocate the new buffer. Don't destroy elements
      // as they are still in a valid state
      deallocate(new_begin);

      // Update sizes
      _size = other._size;
      _capacity = other._size;

    } else {
      // Potential loss of data here, as pointers to original data
      // are lost (_end - other._end pointers)
      // TODO Resolve
      _end = std::copy(other._begin, other._end, _begin);
      _size = other._size;
    }
    return *this;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  void PtrVector<T, D, G>::swap(PtrVector& other) noexcept {
    using std::swap;
    swap(_capacity, other._capacity);
    swap(_size, other._size);
    swap(_begin, other._begin);
    swap(_end, other._end);
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  void PtrVector<T, D, G>::delete_element(PtrVector::ValueType p_elem) {
    DeletionPolicy()(p_elem);
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  void PtrVector<T, D, G>::delete_all() {
    // Standard guarantees destruction of elements in reverse order
    // So the following loop is somewhat unnatural to read
    for (SizeType loop = 0; loop < _size; ++loop) {
      delete_element(*(_end - 1 - loop));
    }
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::PointerType PtrVector<T, D, G>::allocate(
      std::size_t size) {
    // Replace with whatever strategy (should derive from allocator)
    return static_cast<PtrVector::PointerType>(
        ::operator new(sizeof(PtrVector::ValueType) * size));
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  void PtrVector<T, D, G>::deallocate(PtrVector::PointerType buffer) {
    ::operator delete(buffer);
    //    delete static_cast<void*>(buffer);
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::SizeType PtrVector<T, D, G>::capacity() const
      noexcept {
    return _capacity;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::SizeType PtrVector<T, D, G>::size() const
      noexcept {
    return _size;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  bool PtrVector<T, D, G>::empty() const noexcept {
    return _size == 0;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  void PtrVector<T, D, G>::push_back(PtrVector::ValueType const ptr) {
    // 1. Check if it exceeds capacity
    if (_size != _capacity) {
      // Acquire resources and push_back
      *_end = ptr;
      ++_end;
      ++_size;
    } else {
      // 3. Apply growth strategy
      SizeType _new_capacity = GrowthPolicy()(_size, _size + 1);

      // Allocate and deallocate
      PointerType _buffer = allocate(_new_capacity);
      // move_n?, do not need to destruct elements as they are guaranteed
      // to be pointer types
      std::uninitialized_copy_n(_begin, _size, _buffer);

      // Release old memory, should I set to null?
      deallocate(_begin);

      _begin = _buffer;
      _end = _buffer + _size;
      *_end = ptr;
      ++_end;
      ++_size;
      _capacity = _new_capacity;
    }
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  void PtrVector<T, D, G>::pop_back() {
    delete_element(*(--_end));
    --_size;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  void PtrVector<T, D, G>::release_back() {
    --_end;
    --_size;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::ReferenceType PtrVector<T, D, G>::operator[](
      PtrVector::SizeType index) {
    ELASTICA_ASSERT(index < _size, "PtrVector index exceeds size bounds");
    return *(_begin + index);
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::ConstReferenceType
      PtrVector<T, D, G>::operator[](PtrVector::SizeType index) const {
    ELASTICA_ASSERT(index < _size, "PtrVector index exceeds size bounds");
    return *(_begin + index);
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::ReferenceType PtrVector<T, D, G>::front() {
    ELASTICA_ASSERT(_size > 0UL, "Invalid access to front using empty vector");
    return *_begin;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::ConstReferenceType PtrVector<T, D, G>::front()
      const {
    ELASTICA_ASSERT(_size > 0UL, "Invalid access to front using empty vector");
    return *_begin;
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::ReferenceType PtrVector<T, D, G>::back() {
    ELASTICA_ASSERT(_size > 0UL, "Invalid access to back using empty vector");
    return *(_end - 1);
  }

  template <typename T,  // Type
            typename D,  // Deletion Policy
            typename G>  // Growth Policy
  typename PtrVector<T, D, G>::ConstReferenceType PtrVector<T, D, G>::back()
      const {
    ELASTICA_ASSERT(_size > 0UL, "Invalid access to back using empty vector");
    return *(_end - 1);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::Iterator
  PtrVector<T, D, G>::begin() noexcept {
    return Iterator(_begin);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::ConstIterator PtrVector<T, D, G>::begin()
      const noexcept {
    return ConstIterator(_begin);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::ConstIterator PtrVector<T, D, G>::cbegin()
      const noexcept {
    return ConstIterator(_begin);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::Iterator
  PtrVector<T, D, G>::end() noexcept {
    return Iterator(_end);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::ConstIterator PtrVector<T, D, G>::end()
      const noexcept {
    return ConstIterator(_end);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::ConstIterator PtrVector<T, D, G>::cend()
      const noexcept {
    return ConstIterator(_end);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  typename PtrVector<T, D, G>::SizeType PtrVector<T, D, G>::max_size() const
      noexcept {
    return SizeType(-1) / sizeof(PointerType);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  template <typename IteratorType>
  void PtrVector<T, D, G>::assign(IteratorType first, IteratorType last) {
    clear();
    insert(cend(), first, last);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  typename PtrVector<T, D, G>::Iterator PtrVector<T, D, G>::insert(
      PtrVector::ConstIterator pos, PtrVector::ValueType p) {
    return -1;
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  template <typename IteratorType>
  inline typename PtrVector<T, D, G>::Iterator PtrVector<T, D, G>::insert(
      ConstIterator pos, IteratorType first, IteratorType last) {
    return insert(pos, first, last, typename IteratorType::iterator_category());
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::Iterator PtrVector<T, D, G>::erase(
      ConstIterator pos) {
    // ConstIterator pos.base() : T const * const* [&];
    // After casting : T ** : undefined behavior?
    T** const base = const_cast<T**>(pos.base());
    delete_element(*base);
    std::copy(base + 1, _end, base);

    --_size;
    --_end;

    /*
     * DONE : Iterator following the last removed element.
     * DONE : If pos refers to the last element, then the end() iterator is returned.
     * TODO : If last==end() prior to removal, then the updated end() iterator is
    returned.
    */
    return Iterator(base);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::Iterator PtrVector<T, D, G>::erase(
      ConstIterator first, ConstIterator last) {
    // TODO
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::Iterator PtrVector<T, D, G>::release(
      ConstIterator pos) {
    T** const base = const_cast<T**>(pos.base());
    std::copy(base + 1, _end, base);

    --_size;
    --_end;

    // TODO return an iterator, not const iterator
    return Iterator(pos);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::Iterator PtrVector<T, D, G>::release(
      ConstIterator first, ConstIterator last) {
    // TODO
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline void PtrVector<T, D, G>::clear() {
    delete_all();
    _end = _begin;
    _size = static_cast<SizeType>(0UL);
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  void PtrVector<T, D, G>::reserve(SizeType requested_capacity) {
    if (requested_capacity > _capacity) {
      SizeType new_capacity = calculate_capacity(requested_capacity);

      PointerType temp_buffer(allocate(new_capacity));
      std::uninitialized_copy(_begin, _end, temp_buffer);
      std::swap(_begin, temp_buffer);
      _capacity = new_capacity;
      _end = _begin + _size;
      deallocate(temp_buffer);
    }
  }

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  void PtrVector<T, D, G>::shrink_to_fit() {}

  template <typename T,  // Type
            typename D,  // Deletion policy
            typename G>  // Growth policy
  inline typename PtrVector<T, D, G>::SizeType
  PtrVector<T, D, G>::calculate_capacity(SizeType req_capacity) const noexcept {
    ELASTICA_ASSERT(req_capacity > _capacity, "Invalid new vector capacity");
    const size_t new_capacity(GrowthPolicy()(_capacity, req_capacity));
    ELASTICA_ASSERT(new_capacity > _capacity, "Invalid new vector capacity");
    return new_capacity;
  }

}  // namespace elastica
