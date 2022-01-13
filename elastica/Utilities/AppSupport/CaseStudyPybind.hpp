//==============================================================================
/*!
//  \file
//  \brief Generates Pybind-11 based python bindings of case study in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <pybind11/pybind11.h>
#include <string>

#include "Utilities/AppSupport/PythonBindingHelpers.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/PrettyType.hpp"

namespace py_bindings {

  //****************************************************************************
  /*!\brief Binds a case-study into a python module
   * \ingroup python_bindings
   *
   * \example
   * \code
   *  using CaseStudy = MyCaseStudy;
   *  using ::py_bindings::bind_case_study;
   *
   *  PYBIND11_MODULE(my_case_study, m) {  // NOLINT
   *    m.doc() = R"pbdoc(
   *      Pybind11 bindings for MyCaseStudy
   *      )pbdoc";
   *    bind_case_study<ToyExample>(m);
   *  }
   * \endcode
   *
   * \tparam CaseStudy A user defined case-study whose parameters have to be
   * parsed
   */
  template <typename CaseStudy>
  void bind_case_study(pybind11::module& m) {
    namespace py = pybind11;

    using Initializer = Initializer<CaseStudy>;
    using UnpackedInitializer = Unpack<Initializer>;

    // Define a case-study as a class
    py::class_<CaseStudy> py_bound_case_study(
        m,
        // class name
        pretty_type::short_name<CaseStudy>().c_str(),
        // docstring
        std::string(CaseStudy::help).c_str());

    // Only one initialize method as an entry point
    cpp17::apply(
        [&py_bound_case_study](auto... arg_name) {
          py_bound_case_study.def(
              py::init(UnpackedInitializer{}),
              Initializer::generate_docstring().c_str(),
              py::arg(std::move(arg_name).c_str()).noconvert()...);
        },
        Initializer::generate_argument_names());

    // Register the methods here, we need to know the interface here apriori
    // (see the stub classes) as part of ELASTICA_CASE_STUDY
    py_bound_case_study
        .def("__str__", []() { return pretty_type::short_name<CaseStudy>(); })
        .def("run", &CaseStudy::run)
        .def("step", &CaseStudy::step)
        .def("output", &CaseStudy::output
             /* Instead of using references, we create slices/reference_wrappers
              * for value semantics and ease of programming*/
             /*,py::return_value_policy::reference_internal*/);
  }
  //****************************************************************************

}  // namespace py_bindings
