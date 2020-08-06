#pragma once

#include <type_traits>

#include "Utilities/Requires.hpp"

namespace cpp17 {
  /*!
   * \ingroup UtilitiesGroup
   * \brief Mark a return type as being "void". In C++17 void is a regular type
   * under certain circumstances, so this can be replaced by `void` then.
   *
   * The proposal is available
   * [here](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0146r1.html)
   */
  struct void_type {};

  /// \ingroup TypeTraitsGroup
  /// \brief A compile-time boolean
  ///
  /// \usage
  /// For any bool `B`
  /// \code
  /// using result = cpp17::bool_constant<B>;
  /// \endcode
  ///
  /// \see std::bool_constant std::integral_constant std::true_type
  /// std::false_type
  template <bool B>
  using bool_constant = std::integral_constant<bool, B>;

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief A logical AND on the template parameters
  ///
  /// \details
  /// Given a list of ::bool_constant template parameters computes their
  /// logical AND. If the result is `true` then derive off of std::true_type,
  /// otherwise derive from std::false_type. See the documentation for
  /// std::conjunction for more details.
  ///
  /// \usage
  /// For any set of types `Ti` that are ::bool_constant like
  /// \code
  /// using result = cpp17::conjunction<T0, T1, T2>;
  /// \endcode
  /// \pre For all types `Ti`, `Ti::value` is a `bool`
  ///
  /// \metareturns
  /// ::bool_constant
  ///
  /// \semantics
  /// If `T::value != false` for all `Ti`, then
  /// \code
  /// using result = cpp17::bool_constant<true>;
  /// \endcode
  /// otherwise
  /// \code
  /// using result = cpp17::bool_constant<false>;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp conjunction_example
  /// \see std::conjunction, disjunction, std::disjunction
  template <class...>
  struct conjunction : std::true_type {};
  /// \cond HIDDEN_SYMBOLS
  template <class B1>
  struct conjunction<B1> : B1 {};
  template <class B1, class... Bn>
  struct conjunction<B1, Bn...>
      : std::conditional_t<static_cast<bool>(B1::value), conjunction<Bn...>,
                           B1> {};
  /// \endcond
  /// \see std::conjunction
  template <class... B>
  constexpr bool conjunction_v = conjunction<B...>::value;
  // @}

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief A logical OR on the template parameters
  ///
  /// \details
  /// Given a list of ::bool_constant template parameters computes their
  /// logical OR. If the result is `true` then derive off of std::true_type,
  /// otherwise derive from std::false_type. See the documentation for
  /// std::conjunction for more details.
  ///
  /// \usage
  /// For any set of types `Ti` that are ::bool_constant like
  /// \code
  /// using result = cpp17::disjunction<T0, T1, T2>;
  /// \endcode
  /// \pre For all types `Ti`, `Ti::value` is a `bool`
  ///
  /// \metareturns
  /// ::bool_constant
  ///
  /// \semantics
  /// If `T::value == true` for any `Ti`, then
  /// \code
  /// using result = cpp17::bool_constant<true>;
  /// \endcode
  /// otherwise
  /// \code
  /// using result = cpp17::bool_constant<false>;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp disjunction_example
  /// \see std::disjunction, conjunction, std::conjunction
  template <class...>
  struct disjunction : std::false_type {};
  /// \cond HIDDEN_SYMBOLS
  template <class B1>
  struct disjunction<B1> : B1 {};
  template <class B1, class... Bn>
  struct disjunction<B1, Bn...>
      : std::conditional_t<static_cast<bool>(B1::value), B1,
                           disjunction<Bn...>> {};
  /// \endcond
  /// \see std::disjunction
  template <class... B>
  constexpr bool disjunction_v = disjunction<B...>::value;
  // @}

  /// \ingroup TypeTraitsGroup
  /// \brief Negate a ::bool_constant
  ///
  /// \details
  /// Given a ::bool_constant returns the logical NOT of it.
  ///
  /// \usage
  /// For a ::bool_constant `B`
  /// \code
  /// using result = cpp17::negate<B>;
  /// \endcode
  ///
  /// \metareturns
  /// ::bool_constant
  ///
  /// \semantics
  /// If `B::value == true` then
  /// \code
  /// using result = cpp17::bool_constant<false>;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp negation_example
  /// \see std::negation
  /// \tparam B the ::bool_constant to negate
  template <class B>
  struct negation : bool_constant<!B::value> {};

  /// \ingroup TypeTraitsGroup
  /// \brief Given a set of types, returns `void`
  ///
  /// \details
  /// Given a list of types, returns `void`. This is very useful for controlling
  /// name lookup resolution via partial template specialization.
  ///
  /// \usage
  /// For any set of types `Ti`,
  /// \code
  /// using result = cpp17::void_t<T0, T1, T2, T3>;
  /// \endcode
  ///
  /// \metareturns
  /// void
  ///
  /// \semantics
  /// For any set of types `Ti`,
  /// \code
  /// using result = void;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp void_t_example
  /// \see std::void_t
  /// \tparam Ts the set of types
  template <typename... Ts>
  using void_t = void;

  /*!
   * \ingroup TypeTraitsGroup
   * \brief Variable template for is_same
   */
  template <typename T, typename U>
  constexpr bool is_same_v = std::is_same<T, U>::value;

}  // namespace cpp17

namespace tt {

  /// \ingroup TypeTraitsGroup
  /// \brief Given a set of templated types, returns `void`
  ///
  /// \details
  /// Given a list of templated types, returns `void`. This is useful for
  /// controlling name lookup resolution via partial template specialization.
  ///
  /// \usage
  /// For any set of templated types `template <...> Ti`,
  /// \code
  /// using result = cpp17::void_t<T0, T1, T2, T3>;
  /// \endcode
  ///
  /// \metareturns
  /// void
  ///
  /// \semantics
  /// For any set of types `Ti`,
  /// \code
  /// using result = void;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp void_t_example
  /// \see std::void_t
  /// \tparam Ts the set of types
  template <template <class...> class...>
  using templated_void_t = void;

  // Count number of types in the tagged tuple
  namespace detail {
    // Check whether a type is contained in the parameter pack
    template <typename ToBeChecked, typename... Args>
    struct IsContained;

    // Not contained
    template <typename ToBeChecked>
    struct IsContained<ToBeChecked> : std::false_type {};

    //
    template <typename ToBeChecked, typename Head, typename... Args>
    struct IsContained<ToBeChecked, Head, Args...> {
      constexpr static bool value = std::is_same<ToBeChecked, Head>::value or
                                    IsContained<ToBeChecked, Args...>::value;
    };
  }  // namespace detail

  template <typename T, typename... Args>
  constexpr bool is_contained_v = detail::IsContained<T, Args...>::value;

  namespace detail {
    template <typename... Args>
    struct IsAllUnique;

    template <typename ToBeChecked, typename... Args>
    struct IsAllUnique<ToBeChecked, Args...> {
      constexpr static bool value = not is_contained_v<ToBeChecked, Args...> and
                                    IsAllUnique<Args...>::value;
    };

    // No repeats when its the final argument
    template <typename FinalArg>
    struct IsAllUnique<FinalArg> : std::true_type {};

  }  // namespace detail

  template <typename... Args>
  constexpr bool is_all_unique_v = detail::IsAllUnique<Args...>::value;

  namespace detail {
    /*
     * Taken from
     * https://stackoverflow.com/a/34672753
     * But is similar in spirit to Walter Brown's talks.
     * Needs to publicly inherit!!!
     */
    template <template <typename...> class Base, typename Derived>
    struct IsBaseOfTemplateImpl {
      template <typename... Ts>
      static constexpr std::true_type check(const Base<Ts...>*);
      static constexpr std::false_type check(...);
      static constexpr bool value =
          decltype(check(std::declval<Derived*>()))::value;
    };
  }  // namespace detail

  template <template <typename...> class Base, typename Derived>
  constexpr bool is_templated_base_of_v =
      detail::IsBaseOfTemplateImpl<Base, Derived>::value;

  template <template <typename...> class Base, typename Derived>
  using is_templated_base_of_t =
      cpp17::bool_constant<is_templated_base_of_v<Base, Derived>>;

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief Check if type T has a begin() and end() function
  ///
  /// \details
  /// Given a type `T` inherits from std::true_type if `T` has member functions
  /// `begin()` and `end()`, otherwise inherits from std::false_type
  ///
  /// \usage
  /// For any type `T`
  /// \code
  /// using result = tt::is_iterable<T>;
  /// \endcode
  ///
  /// \metareturns
  /// cpp17::bool_constant
  ///
  /// \semantics
  /// If `T` has member function `begin()` and `end()` then
  /// \code
  /// typename result::type = std::true_type;
  /// \endcode
  /// otherwise
  /// \code
  /// typename result::type = std::false_type;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp is_iterable_example
  /// \see has_size
  /// \tparam T the type to check
  namespace detail {
    template <typename T, typename = cpp17::void_t<>>
    struct is_iterable : std::false_type {};
    /// \cond HIDDEN_SYMBOLS
    template <typename T>
    struct is_iterable<T, cpp17::void_t<decltype(std::declval<T>().begin(),
                                                 std::declval<T>().end())>>
        : std::true_type {};
    /// \endcond
    /// \see is_iterable
  }  // namespace detail
  template <typename T>
  constexpr bool is_iterable_v = detail::is_iterable<T>::value;

  /// \see is_iterable
  template <typename T>
  using is_iterable_t = typename detail::is_iterable<T>::type;
  // @}

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief Check if type `T` is a template specialization of `U`
  ///
  /// \requires `U` is a class template
  /// \effects If `T` is a template specialization of `U`, then inherits from
  /// std::true_type, otherwise inherits from std::false_type
  ///
  /// \usage
  /// For any type `T` and class template `U`
  /// \code
  /// using result = tt::is_a<U, T>;
  /// \endcode
  /// \metareturns
  /// cpp17::bool_constant
  ///
  /// \semantics
  /// If the type `T` is a template specialization of the type `U`, then
  /// \code
  /// typename result::type = std::true_type;
  /// \endcode
  /// otherwise
  /// \code
  /// typename result::type = std::false_type;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp is_a_example
  /// \see is_std_array
  /// \tparam T type to check
  /// \tparam U the type that T might be a template specialization of
  namespace detail {
    template <template <typename...> class U, typename T>
    struct is_a : std::false_type {};
    /// \cond HIDDEN_SYMBOLS
    template <template <typename...> class U, typename... Args>
    struct is_a<U, U<Args...>> : std::true_type {};
    /// \endcond
  }  // namespace detail

  /// \see is_a
  template <template <typename...> class U, typename... Args>
  constexpr bool is_a_v = detail::is_a<U, Args...>::value;

  /// \see is_a
  template <template <typename...> class U, typename... Args>
  using is_a_t = typename detail::is_a<U, Args...>::type;
  // @}

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief Check if type `T` is like a std::map or std::unordored_map
  ///
  /// \details
  /// Inherits from std::true_type if the type `T` has a type alias `key_type`,
  /// type alias `mapped_type`, and `operator[](const typename T::key_type&)`
  /// defined, otherwise inherits from std::false_type
  ///
  /// \usage
  /// For any type `T`,
  /// \code
  /// using result = tt::is_maplike<T>;
  /// \endcode
  ///
  /// \metareturns
  /// cpp17::bool_constant
  ///
  /// \semantics
  /// If the type `T` has a type alias `key_type`,
  /// type alias `mapped_type`, and `operator[](const typename T::key_type&)`
  /// defined, then
  /// \code
  /// typename result::type = std::true_type;
  /// \endcode
  /// otherwise
  /// \code
  /// typename result::type = std::false_type;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp is_maplike_example
  /// \see std::map std::unordered_map is_a
  /// \tparam T the type to check
  namespace detail {
    template <typename T, typename = cpp17::void_t<>>
    struct is_maplike : std::false_type {};
    /// \cond HIDDEN_SYMBOLS
    template <typename T>
    struct is_maplike<
        T, cpp17::void_t<typename T::key_type, typename T::mapped_type,
                         decltype(std::declval<T&>()[std::declval<
                             const typename T::key_type&>()]),
                         Requires<tt::is_iterable_v<T>>>> : std::true_type {};
    /// \endcond
  }  // namespace detail

  /// \see is_maplike
  template <typename T>
  constexpr bool is_maplike_v = detail::is_maplike<T>::value;

  /// \see is_maplike
  template <typename T>
  using is_maplike_t = typename detail::is_maplike<T>::type;
  // @}

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief Check if type `T` has operator<<(`S`, `T`) defined.
  ///
  /// \details
  /// Inherits from std::true_type if the type `T` has operator<<(`S`, `T`)
  /// defined for a stream `S`, otherwise inherits from std::false_type
  ///
  /// \usage
  /// For any type `T` and stream type `S`,
  /// \code
  /// using result = tt::is_streamable<S, T>;
  /// \endcode
  ///
  /// \metareturns
  /// cpp17::bool_constant
  ///
  /// \semantics
  /// If the type `T` has operator<<(`S`, `T`) defined for stream `S`, then
  /// \code
  /// typename result::type = std::true_type;
  /// \endcode
  /// otherwise
  /// \code
  /// typename result::type = std::false_type;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp is_streamable_example
  /// \see std::cout std::ifstream std::sstream std::ostream
  /// \tparam S the stream type, e.g. std::stringstream or std::ostream
  /// \tparam T the type we want to know if it has operator<<
  namespace detail {
    template <typename S, typename T, typename = cpp17::void_t<>>
    struct is_streamable : std::false_type {};
    /// \cond HIDDEN_SYMBOLS
    template <typename S, typename T>
    struct is_streamable<
        S, T,
        cpp17::void_t<decltype(std::declval<std::add_lvalue_reference_t<S>>()
                               << std::declval<T>()),
                      Requires<not std::is_same<S, T>::value>>>
        : std::true_type {};
    /// \endcond
  }  // namespace detail

  /// \see is_streamable
  template <typename S, typename T>
  constexpr bool is_streamable_v = detail::is_streamable<S, T>::value;

  /// \see is_streamable
  template <typename S, typename T>
  using is_streamable_t = typename detail::is_streamable<S, T>::type;

  // @}
}  // namespace tt
