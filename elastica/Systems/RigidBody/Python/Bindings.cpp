//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for RigidBody types in \elastica
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
  void bind_sphere(py::module& m);                  // NOLINT
}  // namespace py_bindings

PYBIND11_MODULE(_PyRigidBody, m) {  // NOLINT
  // TODO : detailed documentation
  m.doc() = R"pbdoc(
    Bindings for Elastica++ RigidBody types
    )pbdoc";
  // Experimental : what are the drawbacks
  py::module::import("elasticapp.Arrays");
  py::module::import("elasticapp.Systems.Tags");
  //
  py_bindings::bind_sphere(m);
}
