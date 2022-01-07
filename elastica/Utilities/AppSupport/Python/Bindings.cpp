//
// Created by Tejaswin Parthasarathy on 1/3/22.
//

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace py_bindings {
  void bind_blaze_vector(py::module& m);  // NOLINT
  void bind_blaze_matrix(py::module& m);  // NOLINT
  void bind_blaze_tensor(py::module& m);  // NOLINT
}  // namespace py_bindings

PYBIND11_MODULE(PyBindings, m) {  // NOLINT
  py_bindings::bind_blaze_vector(m);
  py_bindings::bind_blaze_matrix(m);
  py_bindings::bind_blaze_tensor(m);
}
