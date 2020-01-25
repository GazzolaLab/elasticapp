#pragma once

namespace elastica{
  struct PtrDelete{
    template <typename T>
    inline void operator()(T ptr) const;
  };

  template <typename T>
  void PtrDelete::operator()(T ptr) const {
    ptr->~T();
    delete ptr;
  }

}