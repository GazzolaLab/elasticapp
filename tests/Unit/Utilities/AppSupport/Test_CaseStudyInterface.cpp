//
// Created by Tejaswin Parthasarathy on 2/3/22.
//

#pragma once

#include "Framework/TestingFramework.hpp"
#include "Utilities/AppSupport/CaseStudyInterface.hpp"

namespace {

  using elastica::CaseStudyInterface;

  template <typename Step>
  auto make_study(Step fs) {
    struct S : public CaseStudyInterface<S> {
     public:
      using P = CaseStudyInterface<S>;
      using P::set_current_time;
      using P::set_dt;
      void step() { s(); }
      S(Step g) : s(std::move(g)){};
      Step s;
    };

    return S{std::move(fs)};
  }

  template <typename Step, typename Reset>
  auto make_study(Step fs, Reset fr) {
    struct S : public CaseStudyInterface<S> {
     public:
      using P = CaseStudyInterface<S>;
      using P::set_current_time;
      using P::set_dt;
      void step() { s(); }
      void reset() { r(); }
      S(Step g, Reset h) : s(std::move(g)), r(std::move(h)){};
      Step s;
      Reset r;
    };

    return S{std::move(fs), std::move(fr)};
  }

  void test_reset_metaprogramming() {
    auto step_function = []() {};
    auto no_reset_study = make_study(step_function);
    no_reset_study.reset_case_study();

    int i = 0UL;
    auto reset_function = [&]() { i += 5UL; };
    auto reset_study = make_study(step_function, reset_function);
    reset_study.reset_case_study();
    CHECK(i == 5UL);
  }

  void test_run_functions() {}

  ELASTICA_TEST_CASE("Unit.Utilities.AppSupport.CaseStudyInterface",
                     "[Unit][Utilities]") {
    SECTION("Test reset metaprogramming") { test_reset_metaprogramming(); }
    SECTION("Test run functions") {
      SECTION("run_till") {
        int i = 0UL;
        auto step_function = [&]() { ++i; };
        auto step_study = make_study(step_function);
        step_study.set_dt(0.1);
        step_study.run_till(2.0);
        CHECK(i == 20UL);
        step_study.set_current_time(
            4.0);  // time is not updated inside step() :/

        step_study.run_till(1.0);
        CHECK(i == 20UL);
      }
      SECTION("run_for") {
        int i = 0UL;
        auto step_function = [&]() { ++i; };
        auto step_study = make_study(step_function);
        step_study.set_dt(0.1);
        step_study.run_for(2.0);
        CHECK(i == 20UL);
        step_study.run_for(1.0);
        CHECK(i == 30UL);
      }
    }
  }

}  // namespace
