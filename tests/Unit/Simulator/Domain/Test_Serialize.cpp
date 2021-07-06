//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <sstream>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Domain/Serialize.hpp"
#include "Simulator/Frames.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Domain.Serialize",
      "[Unit][Simulator][Domain][Serialization]") {
    // clang-format on
    using namespace elastica::io;
    using namespace elastica;

    auto check = [](auto const& domain) {
      CHECK(domain.get_upper_bound(elastica::EulerianFrame::X) == 2.0);
      CHECK(domain.get_upper_bound(elastica::EulerianFrame::Y) == 3.0);
      CHECK(domain.get_upper_bound(elastica::EulerianFrame::Z) == 4.0);

      CHECK(domain.get_lower_bound(elastica::EulerianFrame::X) == -2.0);
      CHECK(domain.get_lower_bound(elastica::EulerianFrame::Y) == -3.0);
      CHECK(domain.get_lower_bound(elastica::EulerianFrame::Z) == -4.0);

      CHECK(domain.get_span(elastica::EulerianFrame::X) == 4.0);
      CHECK(domain.get_span(elastica::EulerianFrame::Y) == 6.0);
      CHECK(domain.get_span(elastica::EulerianFrame::Z) == 8.0);
    };

    std::stringstream ss;  // any stream can be used
    {
      BinaryOutputArchive oarchive(ss);  //  Create an output archive

      UnboundedDomain unbounded_domain{};
      PeriodicDomain periodic_domain(elastica::make_span(-2.0, 2.0),
                                     elastica::make_span(-3.0, 3.0),
                                     elastica::make_span(-4.0, 4.0));
      oarchive(unbounded_domain, periodic_domain);
    }
    {
      BinaryInputArchive iarchive(ss);  //  Create an output archive
      UnboundedDomain unbounded_domain{};
      PeriodicDomain periodic_domain(elastica::make_span(-10.0, 10.0),
                                     elastica::make_span(-10.0, 10.0),
                                     elastica::make_span(-10.0, 10.0));
      iarchive(unbounded_domain, periodic_domain);

      check(periodic_domain);
    }
  }

}  // namespace
