//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for Sphere
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include <utility>
//
#include <pybind11/pybind11.h>
//
//
#include "Systems/Block/Block.hpp"  // slice()
#include "Systems/RigidBody/Sphere.hpp"
//
#include "Systems/CosseratRods/Python/Generators/BoundChecks.hpp"
#include "Systems/RigidBody/Python/Generators/BindMemberAccess.hpp"
#include "Systems/common/Python/Generators/BindVariableLocator.hpp"
//
#include "Utilities/Math/Python/SliceHelpers.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace py = pybind11;

namespace py_bindings {

  //****************************************************************************
  /*!\brief Helps bind a cosserat rod block and slice in \elastica
   * \ingroup python_bindings
   */
  void bind_sphere(py::module& m) {  // NOLINT
    using UnwantedVariableTags = tmpl::list<>;

    {
      using type = ::elastica::rigid_body::detail::SphereBlockSlice;

      // Wrapper for basic type operations
      auto py_rigid_body =
          py::class_<type>(m, "Sphere")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__",
                  +[](const type& /*meta*/) { return std::string("Sphere"); });
      bind_member_access<UnwantedVariableTags>(py_rigid_body);
      bind_variable_locator<UnwantedVariableTags>(py_rigid_body);
    }
    {
      using type = ::elastica::rigid_body::detail::SphereBlockView;

      // Wrapper for basic type operations
      auto py_rigid_body =
          py::class_<type>(m, "SphereView")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__",
                  +[](const type& /*meta*/) {
                    return std::string("SphereView");
                  })
              .def(
                  "__getitem__",
                  +[](type& t, std::size_t index) {
                    variable_bounds_check(::blocks::n_units(t), index);
                    return ::blocks::slice(t, index);
                  })
              .def(
                  "__getitem__", +[](type& t, const py::slice slice) {
                    auto v = check_slice<0UL>(::blocks::n_units(t),
                                              std::move(slice));
                    return ::blocks::slice(t, v.start, v.start + v.slicelength);
                  });
      bind_member_access<UnwantedVariableTags>(py_rigid_body);
      bind_variable_locator<UnwantedVariableTags>(py_rigid_body);
    }
  }
  //****************************************************************************

}  // namespace py_bindings
