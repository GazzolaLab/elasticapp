//==============================================================================
// \brief Beginner tutorial for setting up a case study with \elastica
// \details
// A (beginner) tutorial to demonstrate setting up a case study
//==============================================================================

#pragma once

/*
 * The purpose of this tutorial is to demonstrate setting up a case study.
 * Elastica case-studies are a quick and powerful way to describe on a high
 * level what the program is for. \elastica then provides the option (via CMake)
 * to generate code for a
 * 1) standalone C++ executable that runs this case-study, with proper setup
 * of elastica, input parsing and help messages.
 * 2) generate bindings for this case-study, to other languages (only Python
 * is supported for now).
 *
 * Here we describe how to write a case-study that \elastica (and users
 * reading the code) can recognize for generating code. As such, we do not run
 * a simulation here. Rather, a more descriptive and complete example that runs
 * an elastica simulation can be found in [TODO : insert link here]
 */

/* 1. The first step is to include this CaseStudy file that helps generate
 * metadata for setting up a case-study. This include must happen in a header
 * file!
 */
#include <elastica/CaseStudy.hpp>

//
#include <elastica/Utilities/TMPL.hpp>

using String = const char* const;

/* 2. We then setup a case-study. Case studies are simple structs in \elastica
 * that encapsulates
 * - The purpose of the case study
 * - The input parameters to setup the study
 * - Set up the actual case using the intuitive interface of \elastica
 * - Finally runs the simulation and communicates the result
 *
 * For the purposes of this tutorial, we create an albeit simple case-study
 * that takes a number and a character as input, performs computation on them,
 * and prints out the result.
 */
struct BeginnerCaseStudy {
  /* 3. We first describe the purpose/details of the case-study using a static
   * compile- time constant string called `help` as shown below. The case-study
   * mechanism uses this string to print useful help messages
   */
  static constexpr String help = {"A beginner case study"};

  /* 3. We now define the input parameters to this case-study. Parameters are
   * also simple structs that contains a nested `type` which indicates the type
   * of the parameter. They also have a `help` string (like above) that
   * documents what the parameter means.
   */

  /* 3.1 The parameter name pops out in the help messages. In this case, the
   * documentation that gets generated looks like
   *
   * first_parameter : Parameter that is an integer (int)
   */
  struct first_parameter {
    // integer parameter
    using type = int;
    // documentation
    static constexpr String help = {"Parameter that is an integer"};
  };

  /* 3.2 Another parameter of a different type*/
  struct second_parameter {
    using type = char;
    static constexpr String help = {"Another parameter but its a string"};
  };

  /*
   * 3.3 We now collect all these parameters in a list called Parameters.
   * This eases readability of this study-case cpp file but also helps in the
   * generated executables.
   */
  using Parameters = tmpl::list<first_parameter, second_parameter>;

  /*
   * 4. If you want, you can also store the input parameters above
   * for use in the case-study class in its methods.
   */
  int first{0};
  char second{'a'};

  /*
   * 5. Finally, the case-study must declare these (required) methods for it to
   * work.
   *
   * 5.1 The first method is to actually setup the case study from the input
   * parameters. The signature of this function can be implemented in the hpp
   * file. It has a single parameter of `InputTypes<BeginnerCaseStudy>`.
   * (Note the use of the case-study name within its method definition, this is
   * valid C++ code). We will look at the definition of this method soon.
   */
  void setup(InputTypes<BeginnerCaseStudy> const& inputs);

  /*
   * 5.2 The second method is intended to run the case study and is helpfully
   * called `run`.
   */
  void run();

  /*
   * Stub methods : Todo : refactor
   */
  inline void step(){};
  inline int& output() { return first; }
};

/*
 * 6. Finally, after completing the definition of the class, you can use this
 * macro to tell \elastica that we intend to setup `BeginnerCaseStudy` as a
 * \elastica case study.
 */
ELASTICA_CASE_STUDY(BeginnerCaseStudy);

/*
 * 7. Congratulations! We now have registered our first case study. Before
 * looking at how to compile-this, we look at the method definitions of
 * `BeginnerCaseStudy` first. We implement these in a cpp file
 */