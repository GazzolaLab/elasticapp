#pragma once

#include <array>
#include <vector>

// StorageTraits, Vector/Matrix/Serial types and how to call them? (Some are
// template, some aren't)

/*
 * TODO
 * Does making a hierarchy for Traits make sense?
 *  RealTrait <-- VectorTrait <-- MatrixTrait <-- StorageTrait?
 */

namespace storage_detail {

  template <template <typename T, int Size, typename...> class Vector,
            typename T, int Size, int Dim, typename... Args>
  using matrix_from_type = Vector<Vector<T, Size, Args...>, Dim, Args...>;

  enum class StorageOption { STDVector, STDArray, Static, Hybrid, Dynamic };

  template <StorageOption SO>
  struct StorageTrait;

  struct RealTrait;
}  // namespace storage_detail

#include "blaze/Math.h"

namespace storage_detail {

  struct RealTrait {
    template <typename T, typename... Args>
    using scalar_type = T;
  };

  template <>
  struct StorageTrait<StorageOption::STDVector> : RealTrait {
    template <typename T, int Size, typename... Args>
    using vector_type = std::vector<T>;

    template <typename T, int Size, int Dim, typename... Args>
    using matrix_type = matrix_from_type<vector_type, T, Size, Dim, Args...>;
  };

  template <>
  struct StorageTrait<StorageOption::STDArray> : RealTrait {
    template <typename T, int Size, typename... Args>
    using vector_type = std::array<T, Size>;

    template <typename T, int Size, int Dim, typename... Args>
    using matrix_type = matrix_from_type<vector_type, T, Size, Dim, Args...>;
  };

  template <>
  struct StorageTrait<StorageOption::Static> : RealTrait {
    template <typename T, int Size, typename... Args>
    using vector_type = blaze::StaticVector<T, Size>;

    template <typename T, int Size, int Dim, typename... Args>
    using matrix_type = blaze::StaticMatrix<T, Dim, Size>;
  };

  template <>
  struct StorageTrait<StorageOption::Hybrid> : RealTrait {
    template <typename T, int Size, typename... Args>
    using vector_type = blaze::HybridVector<T, Size>;

    template <typename T, int Size, int Dim, typename... Args>
    using matrix_type = blaze::HybridMatrix<T, Dim, Size>;
  };

  template <>
  struct StorageTrait<StorageOption::Dynamic> : RealTrait {
    template <typename T, int Size, typename... Args>
    using vector_type = blaze::DynamicVector<T>;

    template <typename T, int Size, int Dim, typename... Args>
    using matrix_type = blaze::DynamicMatrix<T>;
  };

}  // namespace storage_detail

template <storage_detail::StorageOption SO>
using store_type = storage_detail::StorageTrait<SO>;
