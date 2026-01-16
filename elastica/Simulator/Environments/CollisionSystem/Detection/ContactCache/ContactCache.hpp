#pragma once

//
#include "Simulator/Environments/CollisionSystem/Detection/ContactCache/ContainerPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/ContactCache/Types.hpp"
//
#include "Utilities/TMPL.hpp"

namespace elastica {

  // List of pairwise to be added
  template <typename L, typename ContainerPolicy_>
  struct ContactCache {
    using ContainerPolicy = ContainerPolicy_;
    using data_type = tmpl::as_tuple<
        tmpl::transform<L, typename ContainerPolicy::template apply<tmpl::_1>>>;

    // pair<A, B> -> Data<A, B>
    using LookupTable = tmpl::wrap<
        tmpl::transform<
            L, tmpl::bind<tmpl::pair, tmpl::_1,
                          typename ContainerPolicy::template apply<tmpl::_1>>>,
        tmpl::map>;

    ContactCache() = default;
    ~ContactCache() = default;

    template <typename T1, typename T2>
    static constexpr auto lookup() noexcept
        -> tmpl::lookup<LookupTable, tmpl::pair<T1, T2>>;

    // This indirection is needed since algorithms like HashGrid may not always
    // respect ordering from different cells
    template <typename T1, typename T2>
    void emplace_back_impl(tmpl::type_<tmpl::no_such_type_>, T1& t1,
                           T2& t2) /* const noexcept*/ {
      using C = decltype(lookup<T2, T1>());
      emplace_back_impl(tmpl::type_<C>{}, t2, t1);
    }

    template <typename C, typename T1, typename T2>
    void emplace_back_impl(tmpl::type_<C>, T1& t1, T2& t2) /* const noexcept*/ {
      std::get<C>(data_).emplace_back(t1, t2);
    }

    template <typename T1, typename T2>
    void emplace_back(T1& t1, T2& t2) /* const noexcept*/ {
      using C = decltype(lookup<T1, T2>());
      emplace_back_impl(tmpl::type_<C>{}, t1, t2);
    }

    auto clear() /*const noexcept */ -> void {
      std::apply(
          [](auto&... data) { EXPAND_PACK_LEFT_TO_RIGHT(data.clear()); },
          data_);
    }

    auto empty() const noexcept -> bool {
      return std::apply(
          [](auto&... data) {
            bool is_empty = true;
            EXPAND_PACK_LEFT_TO_RIGHT((is_empty &= data.empty()));
            return is_empty;
          },
          data_);
    }

    constexpr auto data() & noexcept -> data_type& { return data_; }
    constexpr auto data() const& noexcept -> const data_type& { return data_; }
    template <typename T1, typename T2>
    constexpr decltype(auto) data() const& noexcept {
      using C = decltype(lookup<T1, T2>());
      return std::get<C>(data_);
    }
    template <typename T1, typename T2>
    constexpr decltype(auto) data() & noexcept {
      using C = decltype(lookup<T1, T2>());
      return std::get<C>(data_);
    }

    //**Friendships*********************************************************
    //! Friend the equality operator
    // friend bool operator==(data_t const& lhs, data_t const& rhs) noexcept
    // {
    //   return lhs.data() == rhs.data();
    // }
    // noexcept(noexcept(detail::blocks_equality_impl(lhs, rhs)));
    //**********************************************************************

    data_type data_;
  };
  //****************************************************************************

}  // namespace elastica