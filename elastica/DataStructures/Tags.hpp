#pragma once

#include "DataStructures/StorageTraits.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/TypeTraits.hpp"

namespace Tags {

  /*
   * Tags can be of two kinds
   * Select which one
   */
  enum class TagType { Simple, Container };

  template <typename T>
  struct BaseTag {
    using value_type = T;

    template <storage_detail::StorageOption SO>
    using store_type = store_type<SO>;
  };

  template <typename T>
  struct ScalarTag : BaseTag<T> {
    template <class StoreType, typename U, typename... Args>
    using data_type = typename StoreType::template scalar_type<U, Args...>;
  };

  template <typename T>
  struct VectorTag : BaseTag<T> {
    template <class StoreType, typename U, size_t FirstDim, size_t,
              typename... Args>
    using data_type =
        typename StoreType::template vector_type<U, FirstDim, Args...>;
  };

  template <typename T>
  struct MatrixTag : BaseTag<T> {
    template <class StoreType, typename U, size_t FirstDim, size_t SecondDim,
              typename... Args>
    using data_type =
        typename StoreType::template matrix_type<U, FirstDim, SecondDim,
                                                 Args...>;
  };

}  // namespace Tags

/*
Simple Tags for plain data types
*/
/*
namespace Tags {

  struct SimplePODTag {
    using data_type = real_t;
    static std::string desc() { return "SimplePODTag"; }
  };

  struct SimpleVectorTag {
    using data_type = std::vector<float>;
    static std::string desc() { return "SimpleVectorTag"; }
  };

}  // namespace Tags
*/

/*
Derived Tags for aggregating data
namespace Tags {

  struct Position : MatrixTag<real_t> {
    static std::string desc() { return "Position"; }
  };

  struct Velocity : MatrixTag<real_t> {
    static std::string desc() { return "Velocity"; }
  };

  struct OneDimension : VectorTag<real_t> {
    static std::string desc() { return "OneDimension"; }
  };

  struct Dimension : MatrixTag<real_t> {
    static std::string desc() { return "Dimension"; }
  };

}  // namespace Tags
*/

/*
 * Metafunctions to test validity of simple and complex tags
 */
namespace Tags {
  namespace detail {

    /*
     * Checks if Simple Tag has data_type member alias
     */
    template <typename Tag>
    struct IsSimpleTagWellFormed {
      template <typename U>
      static std::true_type check(typename U::data_type*);
      template <typename U>
      static std::false_type check(...);
      static constexpr bool value = decltype(check<Tag>(nullptr))::value;
    };

    /*
     * Checks if Tag is derived from templated BaseTag
     */
    template <typename Tag>
    struct IsContainerTagWellFormed : tt::is_templated_base_of_t<BaseTag, Tag> {
    };

  }  // namespace detail

  template <typename Tag>
  constexpr bool is_simple_tag_v = detail::IsSimpleTagWellFormed<Tag>::value;

  template <typename Tag>
  constexpr bool is_container_tag_v =
      detail::IsContainerTagWellFormed<Tag>::value;

}  // namespace Tags
