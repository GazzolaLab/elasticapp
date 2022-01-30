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

#include "Utilities/AppSupport/PythonBindingHelpers.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace py_bindings {

  namespace detail {

    // reset should be called in a non-const context
    template <class T>
    using reset_t = decltype(std::declval<T&>().reset());

    using reset_function_detected = std::true_type;
    using reset_function_not_detected = std::false_type;

    template <typename CaseStudy>
    void add_reset(pybind11::class_<CaseStudy>& py_case_study,
                   reset_function_detected /*meta*/) {
      py_case_study.def("reset", &CaseStudy::reset,
                        "Resets the current case study");
    }

    template <typename CaseStudy>
    void add_reset(pybind11::class_<CaseStudy>& py_case_study,
                   reset_function_not_detected /*meta*/) noexcept {
      // can add an empty reset function in case it is not defined
      py_case_study.def(
          "reset",
          +[](CaseStudy const&) { return std::string("Reset not defined!"); },
          "Resets the current case study");
    }

  }  // namespace detail

  //****************************************************************************
  /*!\brief Binds a case-study into a python module
   * \ingroup python_bindings
   *
   * \details
   * Adds a `reset` function to the python binding, provided it exists in the
   * C++ side
   */
  template <typename CaseStudy>
  void add_reset_function_to(pybind11::class_<CaseStudy>& py_case_study) {
    detail::add_reset(py_case_study,
                      tt::is_detected<detail::reset_t, CaseStudy>{});
  }
  //****************************************************************************

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
        .def("run_till", &CaseStudy::run_till, py::arg("time"),
             "Runs the simulator till given time")
        .def("current_time", &CaseStudy::current_time,
             "Gets the current time of the simulation")
        .def("current_dt", &CaseStudy::current_dt,
             "Gets the current timestep of the simulation")
        .def("current_iteration", &CaseStudy::current_iteration,
             "Gets the current iteration of the simulation");

    // Addendum
    add_reset_function_to(py_bound_case_study);
  }
  //****************************************************************************

}  // namespace py_bindings
