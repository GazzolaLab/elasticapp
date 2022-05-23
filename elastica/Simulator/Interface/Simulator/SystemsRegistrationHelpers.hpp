//
// Created by Tejaswin Parthasarathy on 5/15/22.
//

#pragma once

#include <type_traits>

#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace interface {

    namespace detail {

      namespace tt {

        template <typename Mod, typename R, typename... Args>
        using emplace_back_t =
            decltype(std::declval<Mod&>().template emplace_back<R>(
                std::declval<Args>()...));

        template <typename Module, typename T, typename... Args>
        struct has_emplace_back_t {
          using type = ::tt::is_detected<emplace_back_t, Module, T, Args...>;
        };

        template <typename T, typename R>
        using get_invalid_ref_t =
            decltype(std::declval<T const&>().template get_invalid_ref<R>());

        template <typename Module, typename T>
        struct has_get_invalid_ref {
          using type = ::tt::is_detected<get_invalid_ref_t, Module, T>;
        };

      }  // namespace tt

      template <typename T, typename U = T>
      struct GiveFeedbackToUserForType {
        using type = T;
        using simulator_error_no_valid_operation_found_across_modules =
            std::nullptr_t;
      };

      template <typename T>
      struct GiveFeedbackToUserForType<T, NoSuchType> {
        using type = T;
      };

      // This logic can be buried within brigand, but I want to give useful
      // feedback to user
      // Sadly, I need to pass in all arguments for the decltype to be valid
      // This will increase compile-time most probably
      template <template <typename...> class Op, typename L, typename T,
                typename... Modules>
      struct ChooseOverloadFor;

      template <template <typename...> class Op, typename T,
                typename... Modules>
      struct ChooseOverloadFor<Op, tmpl::list<>, T, Modules...> {
        using value_type = NoSuchType;
        using type = GiveFeedbackToUserForType<T, value_type>;
      };

      template <template <typename...> class Op, typename M, typename T,
                typename... OM, typename... Modules>
      struct ChooseOverloadFor<Op, tmpl::list<M, OM...>, T, Modules...> {
        using value_type = typename Op<M, T, Modules...>::type;
        using type = typename tmpl::eval_if<
            value_type, tmpl::type_<GiveFeedbackToUserForType<M>>,
            ChooseOverloadFor<Op, tmpl::list<OM...>, T, Modules...>>::type;
      };

    }  // namespace detail

    template <template <typename...> class Op, typename T>
    struct ChooseMethodOverload;

    template <template <typename...> class Op, typename... Modules>
    struct ChooseMethodOverload<Op, tmpl::list<Modules...>> {
      template <typename T, typename... R>
      struct For {
        using feedback_type =
            typename detail::ChooseOverloadFor<Op, tmpl::list<Modules...>, T,
                                               R...>::type;
        // decltypes are necessary to force instantiation rather than the
        // compiler smartly optimizing it away
        using feedback IGNORE_UNUSED = decltype(
            typename feedback_type::
                simulator_error_no_valid_operation_found_across_modules{});
        using type = typename feedback_type::type;
      };
    };

  }  // namespace interface

}  // namespace elastica