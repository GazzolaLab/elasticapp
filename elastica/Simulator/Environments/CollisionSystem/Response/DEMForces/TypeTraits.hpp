#pragma once

#include <type_traits>

#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      namespace tt {

        namespace detail {

          // Checks for operator () with signature real_t(ContactID)
          // https://fekir.info/post/detect-member-variables/, especially the
          // ill-formed class
          template <class, class = real_t>
          struct IsValidForceModel : std::false_type {};

          // Note T& here because its always called from a lvalue
          template <class T>
          struct IsValidForceModel<T, decltype(std::declval<T&>().operator()(
                                          std::declval<ContactID>()))>
              : std::true_type {};
        }  // namespace detail

        template <class T>
        constexpr bool is_valid_force_model_v =
            detail::IsValidForceModel<T>::value;

        namespace detail {

          // primary template handles types that have no nested ::ForceModel
          // member:
          template <class, class = std::void_t<>>
          struct HasValidForceModelMember : std::false_type {};

          // specialization recognizes types that do have a nested ::ForceModel
          // member:
          template <class T>
          struct HasValidForceModelMember<T,
                                        std::void_t<typename T::ForceModel>>
              : std::true_type {};

        }  // namespace detail

        template <class T>
        constexpr bool has_valid_force_model_member_v =
            detail::HasValidForceModelMember<T>::value;

      }  // namespace tt

    }  // namespace response

  }  // namespace collision

}  // namespace elastica