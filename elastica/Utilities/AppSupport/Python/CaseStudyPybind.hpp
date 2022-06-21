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
#include <pybind11/stl.h>
#include <string>

#include "PythonBindingHelpers.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

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
        Initializer::generate_argument_metadata());

    // Register the methods here, we need to know the interface here apriori
    // (see CaseStudyInterface)
    py_bound_case_study
        .def("__str__", []() { return pretty_type::short_name<CaseStudy>(); })
        .def(
            "help",
            +[](CaseStudy const&) { return std::string(CaseStudy::help); },
            "Prints a descriptive help message")
        .def("run", &CaseStudy::run,
             "Runs the simulator till the user-defined criterion")
        .def("step", &CaseStudy::step,
             "Steps forward the simulator one iteration")
        .def("communicate", &CaseStudy::communicate
             /* Instead of using references, we create slices/reference_wrappers
              * for value semantics and ease of programming*/
             /*,py::return_value_policy::reference_internal*/)
        .def("run_for", &CaseStudy::run_for, py::kw_only(),
             py::arg("additional_time"),
             "Runs the simulator for additional seconds")
        .def("run_for", &CaseStudy::run_for_n_iterations, py::kw_only(),
             py::arg("n_iterations"),
             "Runs the simulator for n additional iterations")
        .def("run_for_n_iterations", &CaseStudy::run_for_n_iterations,
             py::arg("n_iterations"), py::pos_only(),
             "Runs the simulator for n additional iterations")
        .def("current_time", &CaseStudy::current_time,
             "Gets the current time of the simulation")
        .def("current_dt", &CaseStudy::current_dt,
             "Gets the current timestep of the simulation")
        .def("current_iteration", &CaseStudy::current_iteration,
             "Gets the current iteration of the simulation")
        .def("reset", &CaseStudy::reset_case_study,
             "Resets the current case study");
  }
  //****************************************************************************

}  // namespace py_bindings
