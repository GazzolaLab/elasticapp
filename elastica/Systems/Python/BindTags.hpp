//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for system tags
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//******************************************************************************

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Systems/Block/TypeTraits.hpp"
//
#include "Utilities/PrettyType.hpp"
//
#include <pybind11/pybind11.h>
//
#include <type_traits>

namespace py_bindings {

  //****************************************************************************
  /*!\brief Helps bind (unique) tags of Plugins in \elastica
   * \ingroup python_bindings
   *
   * \details
   *
   *
   * \param m A python module
   */
  template <typename Plugins>
  void bind_tags(pybind11::module& m) {
    using AllVariables = tmpl::flatten<
        tmpl::transform<Plugins, tmpl::bind<blocks::variables_t, tmpl::_1>>>;
    using UniqueTags = tmpl::remove_duplicates<tmpl::transform<
        AllVariables, tmpl::bind<blocks::parameter_t, tmpl::_1>>>;

    // first define variables as a read only property for named evaluation
    tmpl::for_each<UniqueTags>([&](auto v) {
      namespace py = pybind11;
      using Tag = tmpl::type_from<decltype(v)>;

      std::string tag_name = pretty_type::short_name<Tag>();
      py::class_<Tag>(m, ("_" + tag_name).c_str(),
                      ("Symbol corresponding to C++ tag " + tag_name).c_str());
    });
  }

}  // namespace py_bindings