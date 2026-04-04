//==============================================================================
/*!
//  \file
//  \brief Helpers for binding case study to python in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Utilities/AppSupport/Helpers/ParamsHolder.hpp"
//
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/NonCreatable.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/TMPL.hpp"
//
#include <ostream>
#include <string>
#include <tuple>

namespace py_bindings {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Helps unpack arguments for an Initializer to the interface expected
   * by pybind
   * \ingroup python_bindings
   */
  template <typename Init, typename Ts = typename Init::ValueTypes>
  struct Unpack;

  template <typename Init, template <typename...> class C, typename... Ts>
  struct Unpack<Init, C<Ts...>> {
    decltype(auto) operator()(Ts const&... ts) const {
      return Init::setup(std::make_tuple(ts...));
    }
  };
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Helper to initialize a case-study in Python
   * \ingroup python_bindings
   *
   * \tparam CaseStudy A user defined case-study whose parameters have to be
   * parsed
   */
  template <typename CaseStudy>
  struct Initializer : public ::elastica::NonCreatable {
    //**Type definitions********************************************************
    //! Parameter (tags) of the case study
    using Parameters = tmpl::as_tuple<typename CaseStudy::Parameters>;
    // Parameters are all empty structs
    //! Value Types of the parameters
    using ValueTypes =
        tmpl::as_tuple<tmpl::transform<typename CaseStudy::Parameters,
                                       tmpl::bind<tmpl::type_from, tmpl::_1>>>;
    //! Type of input to the Case study
    using InputTypes = tmpl::as_tagged_tuple<typename CaseStudy::Parameters>;
    //**************************************************************************

    //**************************************************************************

    //**************************************************************************
    /*!\brief Generate the argument metadata
     *
     * \details
     * Generates the names, and default values of the arguments for the __init__
     * method
     */
    static auto generate_argument_metadata() {
      auto generate_arg_metadata_for = [](auto input_parameter) -> std::string {
        using Parameter = decltype(input_parameter);
        return pretty_type::name<Parameter>();
      };
      return std::apply(
          [generate_arg_metadata_for](auto... input_parameter) {
            return std::make_tuple(
                generate_arg_metadata_for(input_parameter)...);
          },
          Parameters{});
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Generate the class docstring
     *
     * \details
     * Generates the docstring by composing the parameter helps
     */
    static std::string generate_docstring() {
      auto format_per_sphinxdoc =
          [](std::string parameter_name,
             std::string parameter_description) -> std::string {
        return std::string(":param ") + std::move(parameter_name) +
               std::string(": ") + std::move(parameter_description) +
               std::string("\n");
      };

      auto generate_docstring_for =
          [docstring_formatter =
               format_per_sphinxdoc](auto input_parameter) -> std::string {
        using Parameter = decltype(input_parameter);
        return docstring_formatter(pretty_type::name<Parameter>(),
                                   std::string(Parameter::help));
      };

      auto generate_header = []() -> std::string {
        return std::string("Initializes the ") + pretty_type::name<CaseStudy>();
      };

      return std::apply(
          [generate_header,
           generate_docstring_for](auto... input_parameter) mutable {
            std::ostringstream docstream;
            docstream << generate_header() << "\n\n";
            EXPAND_PACK_LEFT_TO_RIGHT(
                (docstream << generate_docstring_for(input_parameter)));
            return docstream.str();
          },
          Parameters{});
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Sets-up the case study by forwarding arguments to the case study
     * initializer
     *
     * \details
     * Takes value as a sink because unpack<> makes a copy anyway!
     */
    static auto setup(ValueTypes inputs) {
      // Heap allocation to prevent copies in pybind, see #580.
      auto caseStudy = std::make_unique<CaseStudy>();
      // tuples -> tagged tuple needed here
      caseStudy->setup(std::apply(
          [](auto&&... input_parameter) {
            return InputTypes(std::move(input_parameter)...);
          },
          std::move(inputs)));
      return caseStudy;
    }
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Helper that marks the parameters of a case-study in Python
   * \ingroup python_bindings
   *
   * \tparam CaseStudy A user defined case-study
   */
  template <typename CaseStudy>
  struct Params {
    //**Type definitions********************************************************
    //! Type of input to the Case study
    using InputTypes = typename Initializer<CaseStudy>::InputTypes;
    //! List of tags
    using tags_list = typename InputTypes::tags_list;
    //! Value types
    using ValueTypes = typename Initializer<CaseStudy>::ValueTypes;
    //! Parameter (tags) of the case study
    using Parameters = typename Initializer<CaseStudy>::Parameters;
    //! Data type
    using Data = decltype(::elastica::params_holder(std::declval<tags_list>()));
    //**************************************************************************

    //**************************************************************************
    //! Default constructor
    Params() : data_(){};
    //! Dataholder constructor
    explicit Params(Data inputs) : data_(std::move(inputs)) {}
    //! Parameter constructor
    explicit Params(InputTypes inputs) : data_(std::move(inputs)){};
    //**************************************************************************

    //**************************************************************************
    //! Data methods
    auto data() const& noexcept -> typename Data::Data const& {
      return data_.data;
    }
    auto data() & noexcept -> typename Data::Data& { return data_.data; }
    //**************************************************************************

    //! parameter data
    Data data_;
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Stream operator for Params
   */
  template <typename CaseStudy>
  std::ostream& operator<<(std::ostream& os, Params<CaseStudy> const& params) {
    return os << params.data_; // ParamsHolder has an overloaded <<
  }
  //****************************************************************************

}  // namespace py_bindings
