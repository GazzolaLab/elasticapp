//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for CosseratRod types in \elastica
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
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace py_bindings {
  void bind_magnetized_cosserat_rod(py::module& m);                  // NOLINT
  void bind_magnetized_cosserat_rod_without_damping(py::module& m);  // NOLINT
}  // namespace py_bindings

PYBIND11_MODULE(_PyMagnetizedCosseratRods, m) {  // NOLINT
  // TODO : detailed documentation
  m.doc() = R"pbdoc(
    Bindings for Elastica++ Magnetic CosseratRod types
    )pbdoc";
  // Experimental : what are the drawbacks
  py::module::import("elasticapp.Arrays");
  py::module::import("elasticapp.Systems.Tags");
  //
  py_bindings::bind_magnetized_cosserat_rod(m);
  py_bindings::bind_magnetized_cosserat_rod_without_damping(m);
}
