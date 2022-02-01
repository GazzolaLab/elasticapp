//
// Created by Tejaswin Parthasarathy on 2/1/22.
//

#pragma once

#include <cstdint>
#include <functional>

namespace mock {

  template <typename T>
  class reference_wrapper : public std::reference_wrapper<T> {
   protected:
    using P = std::reference_wrapper<T>;

   public:
    using P::P;
    reference_wrapper(P p) : P(std::move(p)) {}
    ~reference_wrapper() = default;

    auto uuid() const -> std::size_t {
      return reinterpret_cast<std::uintptr_t>(std::addressof(P::get()));
    }
  };

  template <class _Tp>
  inline reference_wrapper<_Tp> ref(_Tp& _t) noexcept {
    return reference_wrapper<_Tp>(_t);
  }

  template <class _Tp>
  inline reference_wrapper<_Tp> ref(_Tp const&&) = delete;

}  // namespace mock
