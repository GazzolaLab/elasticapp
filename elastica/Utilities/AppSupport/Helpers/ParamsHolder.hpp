//==============================================================================
/*!
//  \file
//  \brief Parameter holder for case-studies in \elastica
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include <ostream>

#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/StdHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace elastica {

  //****************************************************************************
  //! Parameter storage
  //! \ingroup application_support
  template <typename T, typename... Ts>
  // Explicitly declare a template parameter for the first type T to get rid of
  // compiler errors when using operator<<
  // https://stackoverflow.com/questions/72188017/why-does-my-c-compiler-want-to-instantiate-my-parameter-pack-class-when-usin
  struct ParamsHolder {
    //! Options
    using options = tmpl::list<T, Ts...>;
    //! Help string
    static constexpr const char* const help = {"Holds case-study parameters"};
    //! Data type
    using Data = tmpl::as_tagged_tuple<options>;

    //! Constructors
    ParamsHolder() = default;
    explicit ParamsHolder(typename T::type value, typename Ts::type... values)
        : data(std::move(value), std::move(values)...){};
    explicit ParamsHolder(Data inputs) : data(std::move(inputs)){};
    //

    //! Data, tagged tuple of parameters -> types
    Data data;
  };
  //****************************************************************************

  //****************************************************************************
  //! Typelist to ParamsHolder
  //! \ingroup application_support
  template <typename... T>
  static constexpr auto params_holder(tmpl::list<T...>) noexcept
      -> ParamsHolder<T...>;
  //****************************************************************************

  using ::operator<<;

  //****************************************************************************
  /*!\brief Stream operator for ParamsHolder
   * \ingroup application_support
   */
  template <typename T, typename... Ts>
  std::ostream& operator<<(std::ostream& os,
                           ParamsHolder<T, Ts...> const& params) {
    os << "(";
    tmpl::for_each<typename ParamsHolder<T, Ts...>::options>([&](auto opt) {
      using Opt = tmpl::type_from<decltype(opt)>;
      os << pretty_type::name<Opt>() << ":" << tuples::get<Opt>(params.data);
      os << ",";
    });
    return os << ")";
  }
  //****************************************************************************

}  // namespace elastica