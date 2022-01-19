//
// Created by Tejaswin Parthasarathy on 1/11/22.
//

#pragma once

#include <cstddef>  // for ptrdiff_t, size_t, nullptr_t
#include <type_traits>

#include "ErrorHandling/ExpectsAndEnsures.hpp"
#include "Utilities/Requires.hpp"

/*!
 * \brief Implementations from the Guideline Support Library
 * \ingroup utils
 */
namespace gsl {

  /*!
   * \ingroup UtilitiesGroup
   * \brief Require a pointer to not be a `nullptr`
   *
   * Restricts a pointer or smart pointer to only hold non-null values.
   *
   * Has zero size overhead over `T`.
   *
   * If `T` is a pointer (i.e. `T == U*`) then
   * - allow construction from `U*`
   * - disallow construction from `nullptr_t`
   * - disallow default construction
   * - ensure construction from null `U*` fails
   * - allow implicit conversion to `U*`
   */
  template <class T>
  class not_null {
   public:
    static_assert(std::is_assignable<T&, std::nullptr_t>::value,
                  "T cannot be assigned nullptr.");

    template <typename U, Requires<std::is_convertible<U, T>::value> = nullptr>
    constexpr not_null(U&& u) : ptr_(std::forward<U>(u)) {
      Expects(ptr_ != nullptr);
    }

    template <typename U, Requires<std::is_convertible<U, T>::value> = nullptr>
    constexpr not_null(const not_null<U>& other) : not_null(other.get()) {}

    not_null(const not_null& other) = default;
    not_null& operator=(const not_null& other) = default;

    constexpr T get() const {
      Ensures(ptr_ != nullptr);
      return ptr_;
    }

    constexpr operator T() const { return get(); }
    constexpr T operator->() const { return get(); }
    constexpr decltype(auto) operator*() const { return *get(); }

    // prevents compilation when someone attempts to assign a null pointer
    // constant
    not_null(std::nullptr_t) = delete;
    not_null& operator=(std::nullptr_t) = delete;

    // unwanted operators...pointers only point to single objects!
    not_null& operator++() = delete;
    not_null& operator--() = delete;
    not_null operator++(int) = delete;
    not_null operator--(int) = delete;
    not_null& operator+=(std::ptrdiff_t) = delete;
    not_null& operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

   private:
    T ptr_;
  };

  template <class T>
  std::ostream& operator<<(std::ostream& os, const not_null<T>& val) {
    os << val.get();
    return os;
  }

  template <class T, class U>
  auto operator==(const not_null<T>& lhs, const not_null<U>& rhs)
      -> decltype(lhs.get() == rhs.get()) {
    return lhs.get() == rhs.get();
  }

  template <class T, class U>
  auto operator!=(const not_null<T>& lhs, const not_null<U>& rhs)
      -> decltype(lhs.get() != rhs.get()) {
    return lhs.get() != rhs.get();
  }

  template <class T, class U>
  auto operator<(const not_null<T>& lhs, const not_null<U>& rhs)
      -> decltype(lhs.get() < rhs.get()) {
    return lhs.get() < rhs.get();
  }

  template <class T, class U>
  auto operator<=(const not_null<T>& lhs, const not_null<U>& rhs)
      -> decltype(lhs.get() <= rhs.get()) {
    return lhs.get() <= rhs.get();
  }

  template <class T, class U>
  auto operator>(const not_null<T>& lhs, const not_null<U>& rhs)
      -> decltype(lhs.get() > rhs.get()) {
    return lhs.get() > rhs.get();
  }

  template <class T, class U>
  auto operator>=(const not_null<T>& lhs, const not_null<U>& rhs)
      -> decltype(lhs.get() >= rhs.get()) {
    return lhs.get() >= rhs.get();
  }

  // more unwanted operators
  template <class T, class U>
  std::ptrdiff_t operator-(const not_null<T>&, const not_null<U>&) = delete;
  template <class T>
  not_null<T> operator-(const not_null<T>&, std::ptrdiff_t) = delete;
  template <class T>
  not_null<T> operator+(const not_null<T>&, std::ptrdiff_t) = delete;
  template <class T>
  not_null<T> operator+(std::ptrdiff_t, const not_null<T>&) = delete;

}  // namespace gsl