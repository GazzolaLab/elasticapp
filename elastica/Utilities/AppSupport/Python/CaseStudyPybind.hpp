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
#include "Utilities/MakeString.hpp"
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
        // This hack is necessary to delay the symbols to python runtime
        std::string(CaseStudy::help).c_str());

    // init taking POD arguments
    cpp17::apply(
        [&py_bound_case_study](auto... arg_name) {
          py_bound_case_study.def(
              py::init(UnpackedInitializer{}),
              Initializer::generate_docstring().c_str(),
              py::arg(std::move(arg_name).c_str()).noconvert()...);
        },
        Initializer::generate_argument_metadata());

    // init taking Params argument
    py_bound_case_study.def(py::init([](Params<CaseStudy> const& params) {
                              CaseStudy caseStudy;
                              caseStudy.setup(params.data);
                              return caseStudy;
                            }),
                            "Initializes case-study with its parameters",
                            py::arg("params").noconvert());

    // access params via class property
    py_bound_case_study.def_property_readonly_static(
        "params",
        +[](const py::object& /* self */) {
          return py::type::of<Params<CaseStudy>>();
        },
        "Case-study parameters");

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

  //****************************************************************************
  // Constructs parameters of a case-study from pod types
  template <typename P>
  static auto from_parameters() {
    return cpp17::apply(
        [](auto... parameters) {
          // This is the actual function with compile-time fixed signature.
          return [](tmpl::type_from<decltype(parameters)> const&... t) -> P {
            return P::from_tuple(std::make_tuple(t...));
          };
        },
        // tuple of tags
        typename P::Parameters{});
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Binds the parameters for a case-study into a python module
   * \ingroup python_bindings
   *
   * \tparam CaseStudy A user defined case-study whose parameters have to be
   * parsed
   */
  template <typename CaseStudy>
  void bind_case_study_parameters(pybind11::module& m) {
    namespace py = pybind11;

    // Define a case-study as a class
    using Initializer = Initializer<CaseStudy>;
    using Params = Params<CaseStudy>;

    // This is just a tagged tuple
    using type = Params;
    using Parameters = typename type::tags_list;

    py::class_<type> py_bound_case_study_parameters(
        m,
        // class name
        (pretty_type::short_name<CaseStudy>() + "Parameters").c_str(),
        // docstring
        Initializer::generate_docstring().c_str());

    // Default initializer
    py_bound_case_study_parameters.def(py::init<>());
    // Initializer for all types
    {
      cpp17::apply(
          [&](auto... arg_name) {
            py_bound_case_study_parameters.def(
                pybind11::init(from_parameters<type>()),
                py::arg(std::move(arg_name).c_str()).noconvert()...);
          },
          Initializer::generate_argument_metadata());
    }

    py_bound_case_study_parameters
        // Print out the parameters as a formatted string
        .def(
            "__str__", +[](type const& self) {
              // update
              return std::string(MakeString{} << self);
            });

    // Register each variable as a property
    tmpl::for_each<Parameters>([&py_bound_case_study_parameters](auto p) {
      using Parameter = tmpl::type_from<decltype(p)>;
      py_bound_case_study_parameters.def_property(
          pretty_type::name<Parameter>().c_str(),
          +[](type& self) { return ::tuples::get<Parameter>(self.data); },
          +[](type& self, typename Parameter::type value) {
            ::tuples::get<Parameter>(self.data) = value;
          },
          // This hack is necessary to delay the symbols to python runtime
          std::string(Parameter::help).c_str());
    });

    py_bound_case_study_parameters.def(py::pickle(
        [](type const& t) {  // __getstate__
          /* Return a tuple that fully encodes the state of the object */
          return tuples::apply(
              [](auto const&... args) { return py::make_tuple(args...); },
              t.data);
        },
        [](py::tuple t) {
          static constexpr std::size_t tags_size = type::InputTypes::size();
          if (t.size() != tags_size)
            throw std::runtime_error("Invalid state!");

          return index_apply<tags_size>([=](auto... Is) {
            return type{typename type::InputTypes{
                t[Is]
                    .template cast<
                        tmpl::type_from<tmpl::at_c<Parameters, Is>>>()...}};
          });
        }));
  }
  //****************************************************************************

}  // namespace py_bindings
