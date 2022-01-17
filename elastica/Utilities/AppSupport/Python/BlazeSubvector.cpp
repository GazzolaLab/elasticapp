//==============================================================================
/*!
//  \file
//  \brief Defines python bindings for a \elastica vector view
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

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
//
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
//
#include <blaze/math/DynamicVector.h>
#include <blaze/math/Subvector.h>
//
#include "Utilities/AppSupport/Python/BoundChecks.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/GetOutput.hpp"

namespace py = pybind11;

namespace py_bindings {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Helps bind a vector view in \elastica
   * \ingroup python_bindings
   */
  void bind_blaze_subvector(py::module& m) {  // NOLINT

    using Real = ::elastica::real_t;
    using data_type = ::blaze::DynamicVector<Real, ::blaze::columnVector,
                                             ::blaze::AlignedAllocator<Real>>;
    using type = ::blaze::Subvector_<data_type, ::blaze::unaligned>;

    // Wrapper for basic DataVector operations
    py::class_<type>(m, "VectorView", py::buffer_protocol())
        // Expose the data as a Python buffer so it can be cast into Numpy
        // arrays
        .def_buffer([](type& t) {
          return py::buffer_info(t.data(),
                                 // Size of one scalar
                                 sizeof(Real),
                                 py::format_descriptor<Real>::format(),
                                 // Number of dimensions
                                 1,
                                 // Size of the buffer
                                 {t.size()},
                                 // Stride for each index (in bytes)
                                 {sizeof(Real)});
        })
        .def(
            "__iter__",
            [](const type& t) { return py::make_iterator(t.begin(), t.end()); },
            // Keep object alive while iterator exists
            py::keep_alive<0, 1>())
        // __len__ is for being able to write len(my_data_vector) in python
        .def("__len__", [](const type& t) { return t.size(); })
        // define shape mimicking numpy interface
        .def_property_readonly(
            "shape",
            +[](const type& t) { return std::tuple<std::size_t>(t.size()); })
        // __getitem__ and __setitem__ are the subscript operators (operator[]).
        // To define (and overload) operator() use __call__
        .def(
            "__getitem__",
            +[](const type& t, const size_t i) {
              bounds_check(t, i);
              return t[i];
            })
        .def(
            "__setitem__",
            +[](type& t, const size_t i, const Real v) {
              bounds_check(t, i);
              t[i] = v;
            })
        // Need __str__ for converting to string/printing
        .def(
            "__str__", +[](type const& t) { return get_output(t); })
        // repr allows you to output the object in an interactive python
        // terminal using obj to get the "string REPResenting the object".
        .def(
            "__repr__", +[](const type& t) { return get_output(t); })
        .def(py::self == py::self)
        .def(py::self != py::self);
  }
  //****************************************************************************

}  // namespace py_bindings