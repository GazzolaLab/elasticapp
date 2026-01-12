//==============================================================================
/*!
//  \file
//  \brief Interface adapter for a case study to include Options in \elastica
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
#include "Utilities/AppSupport/Helpers/ParamsHolder.hpp"
//
#include "Options/Options.hpp"
//
#include "Utilities/AsConst.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/TMPL.hpp"

namespace standalone_bindings {

  //****************************************************************************
  //! \brief Option for the case-study parameters
  //! \ingroup application_support
  template <typename ParametersList>
  struct CaseStudyParameters {
    // This should ideally be an enum, but we stick with strings for now
    using type =
        decltype(::elastica::params_holder(std::declval<ParametersList>()));
    static constexpr Options::String help = {"Parameters for the case-study"};
    static std::string name() { return "Parameters"; }
  };
  //****************************************************************************

  //****************************************************************************
  //! \brief Case study with options
  //! \ingroup application_support
  template <typename CaseStudy>
  class CaseStudyWithOptions : public CaseStudy {
   public:
    using CaseStudy::help;
    // Needed to disambiguate variadic parameters
    using Parameters = CaseStudyParameters<typename CaseStudy::Parameters>;
    using options = tmpl::list<Parameters>;

   public:
    CaseStudyWithOptions() = default;

    // Not ideal : may throw during setup
    explicit CaseStudyWithOptions(typename Parameters::type args)
        : CaseStudy(), parameters_(std::move(args)) {}

    void setup() { CaseStudy::setup(std::as_const(parameters_.data)); }

    auto parameters() const& noexcept -> typename Parameters::type const& {
      return parameters_;
    }
    auto parameters() & noexcept -> typename Parameters::type& {
      return parameters_;
    }

   private:
    //! Input parameters
    typename Parameters::type parameters_;
  };
  //****************************************************************************

  //****************************************************************************
  //! \brief Option for case-study
  //! \ingroup application_support
  template <typename CaseStudy>
  struct CaseStudyOption {
    using type = CaseStudyWithOptions<CaseStudy>;
    static std::string name() { return pretty_type::name<CaseStudy>(); }
    static constexpr Options::String help = {"Case-study initializer"};
  };
  //****************************************************************************

}  // namespace standalone_bindings
