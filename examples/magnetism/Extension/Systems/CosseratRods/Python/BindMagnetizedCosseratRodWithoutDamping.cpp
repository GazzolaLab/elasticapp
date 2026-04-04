//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for MagnetizedCosseratRodWithoutDamping
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
//
#include <pybind11/pybind11.h>
//
//
#include <PythonBindings/BoundChecks.hpp>
//
#include <elastica/Systems/Block/Block.hpp>  // slice()
//
#include <elastica/Systems/CosseratRods/Python/Generators/BindEnergyMethods.hpp>
#include <elastica/Systems/CosseratRods/Python/Generators/BindMemberAccess.hpp>
#include <elastica/Systems/CosseratRods/Python/Generators/BindPyElasticaInterface.hpp>
#include <elastica/Systems/common/Python/Generators/BindVariableLocator.hpp>
//
#include <elastica/Utilities/Math/Python/SliceHelpers.hpp>
#include <elastica/Utilities/Size.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include "Extension/Systems/CosseratRods/CosseratRods.hpp"

namespace py = pybind11;

namespace py_bindings {

  //****************************************************************************
  /*!\brief Helps bind a rod without damping in \elastica
   * \ingroup python_bindings
   */
  void bind_magnetized_cosserat_rod_without_damping(py::module& m) {  // NOLINT
    using UnwantedVariableTags =
        tmpl::list<::elastica::tags::_DummyElementVector,
                   ::elastica::tags::_DummyElementVector2,
                   ::elastica::tags::_DummyVoronoiVector>;

    {
      using type = ::elastica::cosserat_rod::detail::
          MagnetizedCosseratRodWithoutDampingBlockSlice;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "MagnetizedCosseratRodWithoutDamping")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__", +[](const type& /*meta*/) {
                    return std::string("MagnetizedCosseratRodWithoutDamping");
                  });
      bind_member_access<UnwantedVariableTags>(py_cosserat_rod);
      bind_variable_locator<UnwantedVariableTags>(py_cosserat_rod);
      bind_pyelastica_interface<UnwantedVariableTags>(py_cosserat_rod);
      bind_energy_methods(py_cosserat_rod);
    }
    {
      using type = ::elastica::cosserat_rod::detail::
          MagnetizedCosseratRodWithoutDampingBlockView;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "MagnetizedCosseratRodWithoutDampingView")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__",
                  +[](const type& /*meta*/) {
                    return std::string(
                        "MagnetizedCosseratRodWithoutDampingView");
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
      bind_member_access<UnwantedVariableTags>(py_cosserat_rod);
      bind_variable_locator<UnwantedVariableTags>(py_cosserat_rod);
      bind_pyelastica_interface<UnwantedVariableTags>(py_cosserat_rod);
    }
  }
  //****************************************************************************

}  // namespace py_bindings
