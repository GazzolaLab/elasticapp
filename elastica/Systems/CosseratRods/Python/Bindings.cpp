//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for CosseratRod types in \elastica
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
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace py_bindings {
  void bind_cosserat_rod(py::module& m);                  // NOLINT
  void bind_cosserat_rod_without_damping(py::module& m);  // NOLINT
}  // namespace py_bindings

PYBIND11_MODULE(_PyCosseratRods, m) {  // NOLINT
  m.doc() = R"pbdoc(
    Python bindings for Elastica++ Cosserat rod types.
    )pbdoc";
  py::module::import("elasticapp.Arrays");
  py::module::import("elasticapp.Systems.Tags");
  //
  py_bindings::bind_cosserat_rod(m);
  py_bindings::bind_cosserat_rod_without_damping(m);
}
