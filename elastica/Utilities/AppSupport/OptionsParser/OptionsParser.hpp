//==============================================================================
/*!
//  \file
//  \brief Parser for handling inputs via Options to a case study in \elastica
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Options/ParseOptions.hpp"
#include "Utilities/AppSupport/OptionsParser/CaseStudyWithOptions.hpp"
#include "Utilities/TMPL.hpp"
//
#include <cxxopts.hpp>
//
#include <initializer_list>
#include <ostream>
#include <string>

namespace standalone_bindings {

  //****************************************************************************
  /*!\brief Parses commandline parameters for case-study setup via options
   * \ingroup application_support
   * \example
   * \code
   *
   * int main(int argc, char* argv[]){
   *  using CaseStudy = MyCaseStudy;
   *  char const* case_study_name("my_case_study");
   *
   *  using ::standalone_bindings::parse_case_study_from_options;
   *
   *  auto case_study =
   *  parse_case_study_from_options<CaseStudy>(case_study_name, argc, argv);
   * }
   *
   * \endcode
   *
   * \tparam CaseStudy A user defined case-study whose parameters have to be
   * parsed
   */
  template <typename CaseStudy>
  auto parse_case_study_from_options(const char* case_study_name, int argc,
                                     char* argv[])
      -> CaseStudyWithOptions<CaseStudy> {
    cxxopts::Options cli_options(case_study_name, std::string(CaseStudy::help));
    cli_options.add_options()("h,help", "Describes program options",
                              cxxopts::value<bool>());
    cli_options.add_options()("input-file",      //
                              "Input filename",  //
                              cxxopts::value<std::string>());
    cli_options.add_options()("check-options", "Check input file options",
                              cxxopts::value<bool>());

    auto parsed_cli = cli_options.parse(argc, argv);

    if (parsed_cli.count("help")) {
      std::cout << cli_options.help() << std::endl;
      std::exit(EXIT_SUCCESS);
    }

    if (parsed_cli.count("input-file") == 0) {
      std::cerr << "No default input file name.  Pass --input-file."
                << std::endl;
      std::exit(EXIT_FAILURE);
    }

    auto const input_file = parsed_cli["input-file"].as<std::string>();

    // Leave the help string to be empty for now.
    using option_list = tmpl::list<CaseStudyOption<CaseStudy>>;
    Options::Parser<option_list> options("");
    options.parse_file(input_file);

    if (parsed_cli.count("check-options")) {
      // Force all the options to be created.
      options.template apply<option_list>([](auto... args) {
        (void)std::initializer_list<char>{((void)args, '0')...};
      });
      std::exit(EXIT_SUCCESS);
    }

    return options.template get<CaseStudyOption<CaseStudy>>();
  }
  //****************************************************************************

}  // namespace standalone_bindings