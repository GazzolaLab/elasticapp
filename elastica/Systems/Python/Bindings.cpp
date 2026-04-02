//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for system tag types in \elastica
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
  void bind_tags(py::module& m);  // NOLINT
}  // namespace py_bindings

PYBIND11_MODULE(_PyTags, m) {  // NOLINT
  m.doc() = R"pbdoc(
    Python bindings for Elastica++ system tag types.
    )pbdoc";
  py_bindings::bind_tags(m);
}
