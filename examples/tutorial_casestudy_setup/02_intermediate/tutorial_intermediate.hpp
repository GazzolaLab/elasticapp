//==============================================================================
// \brief Beginner tutorial for setting up a case study with \elastica
// \details
// A (beginner) tutorial to demonstrate setting up a case study
//==============================================================================

#pragma once

#include <blaze/math/DynamicMatrix.h>
#include <elastica/CaseStudy.hpp>
#include <elastica/Utilities/TMPL.hpp>

using String = const char* const;

struct IntermediateCaseStudy {
  static constexpr String help = {"An intermediate case study"};

  struct n_rows {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of rows in the simulation matrix"};
  };

  struct n_cols {
    using type = std::size_t;
    static constexpr String help = {
        "Number of columns in the simulation matrix"};
  };

  using Parameters = tmpl::list<n_rows, n_cols>;

  void setup(InputTypes<IntermediateCaseStudy> const& inputs);
  void run();

  // communication routines
  void step();

  using Type = blaze::DynamicMatrix<double>;

  inline Type& output() { return data; }

  blaze::DynamicMatrix<double> data;
};

ELASTICA_CASE_STUDY(IntermediateCaseStudy);
