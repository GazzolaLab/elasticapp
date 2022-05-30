//==============================================================================
/*!
//  \file
//  \brief Parser for handling inputs to a case study in \elastica
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

#include <iostream>
#include <string>

// external dependency : needed at example build time
#include "cxxopts.hpp"
//
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"
//
#include "Utilities/AppSupport/CaseStudy.hpp"

namespace standalone_bindings {

  namespace detail {

    template <class T>
    using default_t = decltype(T::default_value());

    using default_detected = std::true_type;
    using default_not_detected = std::false_type;

    template <typename T>
    inline std::string as_string(T const& t) {
      return std::to_string(t);
    }

    inline std::string as_string(std::string const& s) { return s; }

    template <typename Parameter>
    void add_options(cxxopts::Options& options, Parameter /*meta*/,
                     default_detected /*meta*/) {
      using Value = tmpl::type_from<Parameter>;
      options.add_options()(pretty_type::short_name<Parameter>(),  //
                            Parameter::help,                       //
                            cxxopts::value<Value>()->default_value(
                                as_string(Parameter::default_value())));
    }

    template <typename Parameter>
    void add_options(cxxopts::Options& options, Parameter /*meta*/,
                     default_not_detected /*meta*/
    ) {
      using Value = tmpl::type_from<Parameter>;
      options.add_options()(pretty_type::short_name<Parameter>(),  //
                            Parameter::help,                       //
                            cxxopts::value<Value>());
    }

  }  // namespace detail

  //****************************************************************************
  /*!\brief Registers a parameter `p` with options `options`
   * \ingroup application_support
   */
  template <typename Parameter>
  void register_(Parameter p, cxxopts::Options& options) noexcept {
    // copying is cheap, so no forward references
    detail::add_options(options, p,
                        tt::is_detected<detail::default_t, Parameter>{});
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Parses commandline parameters for setting up a case-study
   * \ingroup application_support
   *
   * \example
   * \code
   *
   * int main(int argc, char* argv[]){
   *  using CaseStudy = MyCaseStudy;
   *  char const* case_study_name("my_case_study");
   *
   *  using ::standalone_bindings::parse_case_study_parameters;
   *
   *  auto input_parameters =
   *  parse_case_study_parameters<CaseStudy>(case_study_name, argc, argv);
   * }
   *
   * \endcode
   *
   * \tparam CaseStudy A user defined case-study whose parameters have to be
   * parsed
   */
  template <typename CaseStudy, typename RT = InputTypes<CaseStudy>>
  RT parse_case_study_parameters(const char* case_study_name, int argc,
                                 char* argv[]) {
    cxxopts::Options options(case_study_name, CaseStudy::help);
    options.add_options()("h,help", "Prints help message",
                          cxxopts::value<bool>());

    // Parameters are all empty structs
    using Parameters = tmpl::as_tuple<typename CaseStudy::Parameters>;

    auto register_each_ = [&options](auto&& input_parameter) {
      register_(std::forward<decltype(input_parameter)>(input_parameter),
                options);
    };

    cpp17::apply(
        [register_each_](auto&&... input_parameter) {
          EXPAND_PACK_LEFT_TO_RIGHT(register_each_(input_parameter));
        },
        Parameters{});

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      std::exit(EXIT_SUCCESS);
    }

    if (not ::elastica::size(result.arguments())) {
      auto change_color_to_yellow = "\033[33m";
      auto reset_color = "\033[0m";
      std::cout << change_color_to_yellow << options.help() << reset_color
                << std::endl;
      std::cout << "Running " << case_study_name << " with default parameters"
                << std::endl;
    }

    auto parse_each_ = [&result](auto input_parameter) {
      using Parameter = decltype(input_parameter);
      using Value = tmpl::type_from<Parameter>;

      return result[pretty_type::short_name<Parameter>()].template as<Value>();
    };

    return cpp17::apply(
        [parse_each_](auto... input_parameter) {
          return RT(parse_each_(input_parameter)...);
        },
        Parameters{});
  }
  //****************************************************************************

}  // namespace standalone_bindings
