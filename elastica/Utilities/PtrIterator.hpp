#pragma once

namespace elastica {

  template <typename T>
  class PtrIterator {
   public:
    using IteratorCategory = std::random_access_iterator_tag;
    using DifferenceType = std::ptrdiff_t;
    using ValueType = T*;
    using PointerType = T**;
    using ReferenceType = T*&;
    using IteratorType = T* const*;

    using iterator_category = IteratorCategory;
    using value_type = ValueType;
    using pointer = PointerType;
    using difference_type = DifferenceType;
    using reference = ReferenceType;

    inline PtrIterator();
    explicit inline PtrIterator(const IteratorType& it);
    template <typename OT>
    explicit inline PtrIterator(const PtrIterator<OT>& it);

    inline PtrIterator& operator++();
    inline PtrIterator operator++(int);
    inline PtrIterator& operator--();
    inline PtrIterator operator--(int);
    inline PtrIterator& operator+=(DifferenceType n);
    inline PtrIterator operator+(DifferenceType n) const;
    inline PtrIterator& operator-=(DifferenceType n);
    inline PtrIterator operator-(DifferenceType n) const;

    inline DifferenceType operator-(const PtrIterator& it) const;

    inline ValueType operator[](DifferenceType index) const;
    inline ValueType operator*() const;
    inline ValueType operator->() const;

    inline const IteratorType& base() const;

   private:
    IteratorType _it;
  };

  template <typename T>
  inline PtrIterator<T>::PtrIterator()
      : _it(nullptr)  // Pointer to the current memory location
  {}

  template <typename T>
  inline PtrIterator<T>::PtrIterator(const IteratorType& it)
      : _it(it)  // Pointer to the current memory location
  {}

  template <typename T>
  template <typename OT>
  inline PtrIterator<T>::PtrIterator(const PtrIterator<OT>& it)
      : _it(it.base())  // Pointer to the current memory location
  {}

  template <typename T>
  inline PtrIterator<T>& PtrIterator<T>::operator++() {
    ++_it;
    return *this;
  }

  template <typename T>
  inline PtrIterator<T> PtrIterator<T>::operator++(int) {
    // Why *this and not tmp(_it)?
    PtrIterator tmp(*this);
    ++_it;
    return tmp;
  }

  template <typename T>
  inline PtrIterator<T>& PtrIterator<T>::operator--() {
    --_it;
    return *this;
  }

  template <typename T>
  inline PtrIterator<T> PtrIterator<T>::operator--(int) {
    PtrIterator tmp(*this);
    --_it;
    return tmp;
  }

  template <typename Type>
  inline PtrIterator<Type>& PtrIterator<Type>::operator+=(DifferenceType n) {
    _it += n;
    return *this;
  }

  template <typename Type>
  inline PtrIterator<Type> PtrIterator<Type>::operator+(
      DifferenceType n) const {
    return PtrIterator(_it + n);
  }

  template <typename Type>
  inline PtrIterator<Type>& PtrIterator<Type>::operator-=(DifferenceType n) {
    _it -= n;
    return *this;
  }

  template <typename Type>
  inline PtrIterator<Type> PtrIterator<Type>::operator-(
      DifferenceType n) const {
    return PtrIterator(_it - n);
  }

  template <typename Type>
  inline typename PtrIterator<Type>::DifferenceType
  PtrIterator<Type>::operator-(const PtrIterator& it) const {
    return _it - it._it;
  }

  template <typename Type>
  inline typename PtrIterator<Type>::ValueType PtrIterator<Type>::operator[](
      DifferenceType index) const {
    return _it[index];
  }

  template <typename Type>
  inline typename PtrIterator<Type>::ValueType PtrIterator<Type>::operator*()
      const {
    return *_it;
  }
  template <typename Type>
  inline typename PtrIterator<Type>::ValueType PtrIterator<Type>::operator->()
      const {
    return *_it;
  }

  template <typename Type>
  inline const typename PtrIterator<Type>::IteratorType&
  PtrIterator<Type>::base() const {
    return _it;
  }

  //============================================================================
  //
  //  GLOBAL OPERATORS
  //
  //============================================================================
  template <typename TypeL, typename TypeR>
  inline bool operator==(const PtrIterator<TypeL>& lhs,
                         const PtrIterator<TypeR>& rhs) {
    return lhs.base() == rhs.base();
  }

  template <typename TypeL, typename TypeR>
  inline bool operator!=(const PtrIterator<TypeL>& lhs,
                         const PtrIterator<TypeR>& rhs) {
    return lhs.base() != rhs.base();
  }

  template <typename TypeL, typename TypeR>
  inline bool operator<(const PtrIterator<TypeL>& lhs,
                        const PtrIterator<TypeR>& rhs) {
    return lhs.base() < rhs.base();
  }

  template <typename TypeL, typename TypeR>
  inline bool operator>(const PtrIterator<TypeL>& lhs,
                        const PtrIterator<TypeR>& rhs) {
    return lhs.base() > rhs.base();
  }
  template <typename TypeL, typename TypeR>
  inline bool operator<=(const PtrIterator<TypeL>& lhs,
                         const PtrIterator<TypeR>& rhs) {
    return lhs.base() <= rhs.base();
  }
  template <typename TypeL, typename TypeR>
  inline bool operator>=(const PtrIterator<TypeL>& lhs,
                         const PtrIterator<TypeR>& rhs) {
    return lhs.base() >= rhs.base();
  }

}  // namespace elastica
