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

// external dependency : needed at example build time
#include "cxxopts.hpp"
//
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
//
#include "Utilities/AppSupport/CaseStudy.hpp"

namespace standalone_bindings {

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

    auto register_each_ = [&options](auto input_parameter) {
      using Parameter = decltype(input_parameter);
      using Value = tmpl::type_from<Parameter>;

      options.add_options()(pretty_type::short_name<Parameter>(),
                            Parameter::help, cxxopts::value<Value>());
    };

    cpp17::apply(
        [register_each_](auto&&... input_parameter) {
          EXPAND_PACK_LEFT_TO_RIGHT(register_each_(input_parameter));
        },
        Parameters{});

    auto result = options.parse(argc, argv);

    if ((not ::elastica::size(result.arguments())) || result.count("help")) {
      std::cout << options.help() << std::endl;
      std::exit(EXIT_SUCCESS);
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
