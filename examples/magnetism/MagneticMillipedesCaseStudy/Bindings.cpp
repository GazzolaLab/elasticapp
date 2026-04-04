//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for BackBoneCrissRod
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
#include "Types.hpp"

namespace py = pybind11;

namespace py_bindings {

  //****************************************************************************
  /*!\brief Helps bind a rod without damping in \elastica
   * \ingroup python_bindings
   */
  void bind_criss_cosserat_rod(py::module& m) {  // NOLINT
    using UnwantedVariableTags =
        tmpl::list<::elastica::tags::_DummyElementVector,
            ::elastica::tags::_DummyElementVector2,
            ::elastica::tags::_DummyVoronoiVector>;

    {
      using type =
          ::elastica::cosserat_rod::detail::BackBoneCrissRodBlockSlice;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "BackBoneCrissRod")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__", +[](const type& /*meta*/) {
                    return std::string("BackBoneCrissRod");
                  });
      bind_member_access<UnwantedVariableTags>(py_cosserat_rod);
      bind_variable_locator<UnwantedVariableTags>(py_cosserat_rod);
      bind_pyelastica_interface<UnwantedVariableTags>(py_cosserat_rod);
      bind_energy_methods(py_cosserat_rod);
    }
    {
      using type =
          ::elastica::cosserat_rod::detail::BackBoneCrissRodBlockView;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "BackBoneCrissRodView")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__",
                  +[](const type& /*meta*/) {
                    return std::string("BackBoneCrissRodView");
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

  //****************************************************************************
  /*!\brief Helps bind a rod without damping in \elastica
   * \ingroup python_bindings
   */
  void bind_cross_cosserat_rod(py::module& m) {  // NOLINT
    using UnwantedVariableTags =
        tmpl::list<::elastica::tags::_DummyElementVector,
            ::elastica::tags::_DummyElementVector2,
            ::elastica::tags::_DummyVoronoiVector>;

    {
      using type =
          ::elastica::cosserat_rod::detail::BackBoneCrossRodBlockSlice;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "BackBoneCrossRod")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__", +[](const type& /*meta*/) {
                    return std::string("BackBoneCrossRod");
                  });
      bind_member_access<UnwantedVariableTags>(py_cosserat_rod);
      bind_variable_locator<UnwantedVariableTags>(py_cosserat_rod);
      bind_pyelastica_interface<UnwantedVariableTags>(py_cosserat_rod);
      bind_energy_methods(py_cosserat_rod);
    }
    {
      using type =
          ::elastica::cosserat_rod::detail::BackBoneCrossRodBlockView;

      // Wrapper for basic type operations
      auto py_cosserat_rod =
          py::class_<type>(m, "BackBoneCrossRodView")
              .def("__len__", [](const type& t) { return std::size(t); })
              .def(
                  "__str__",
                  +[](const type& /*meta*/) {
                    return std::string("BackBoneCrossRodView");
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

PYBIND11_MODULE(_PyMagneticMillipedeCosseratRods, m) {  // NOLINT
  // TODO : detailed documentation
  m.doc() = R"pbdoc(
    Bindings for Elastica++ Millipede CosseratRod types
    )pbdoc";
  // Experimental : what are the drawbacks
  py::module::import("elasticapp.Arrays");
  py::module::import("elasticapp.Systems.Tags");
  //
  py_bindings::bind_criss_cosserat_rod(m);
  py_bindings::bind_cross_cosserat_rod(m);
}
