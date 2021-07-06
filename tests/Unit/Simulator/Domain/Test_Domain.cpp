//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include <algorithm>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Domain/Domain.hpp"
#include "Simulator/Frames.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Domain.UnboundedDomain",
      "[Unit][Simulator][Domain]") {
    // clang-format on
    using elastica::UnboundedDomain;
    static_assert(
        ::tt::assert_conforms_to<UnboundedDomain,
                                 elastica::protocols::ComputationalDomain>,
        "Doesnt conform");

    UnboundedDomain domain{};
    using elastica::real_t;

    CHECK(domain.get_upper_bound(elastica::EulerianFrame::X) ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_upper_bound(elastica::EulerianFrame::Y) ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_upper_bound(elastica::EulerianFrame::Z) ==
          (elastica::real_t(elastica::inf)));

    CHECK(domain.get_upper_bound<elastica::EulerianFrame::X>() ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_upper_bound<elastica::EulerianFrame::Y>() ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_upper_bound<elastica::EulerianFrame::Z>() ==
          (elastica::real_t(elastica::inf)));

    CHECK(domain.get_lower_bound(elastica::EulerianFrame::X) ==
          -(elastica::real_t(elastica::inf)));
    CHECK(domain.get_lower_bound(elastica::EulerianFrame::Y) ==
          -(elastica::real_t(elastica::inf)));
    CHECK(domain.get_lower_bound(elastica::EulerianFrame::Z) ==
          -(elastica::real_t(elastica::inf)));

    CHECK(domain.get_lower_bound<elastica::EulerianFrame::X>() ==
          -(elastica::real_t(elastica::inf)));
    CHECK(domain.get_lower_bound<elastica::EulerianFrame::Y>() ==
          -(elastica::real_t(elastica::inf)));
    CHECK(domain.get_lower_bound<elastica::EulerianFrame::Z>() ==
          -(elastica::real_t(elastica::inf)));

    CHECK(domain.get_span<elastica::EulerianFrame::X>() ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_span<elastica::EulerianFrame::Y>() ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_span<elastica::EulerianFrame::Z>() ==
          (elastica::real_t(elastica::inf)));

    CHECK(domain.get_span<elastica::EulerianFrame::X>() ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_span<elastica::EulerianFrame::Y>() ==
          (elastica::real_t(elastica::inf)));
    CHECK(domain.get_span<elastica::EulerianFrame::Z>() ==
          (elastica::real_t(elastica::inf)));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Domain.PeriodicDomain",
      "[Unit][Simulator][Domain]") {
    // clang-format on
    using elastica::PeriodicDomain;
    static_assert(
        ::tt::assert_conforms_to<PeriodicDomain,
                                 elastica::protocols::ComputationalDomain>,
        "Doesnt conform");

    /// [periodic_construction]
    PeriodicDomain periodic_domain{-2.0, 2.0, -3.0, 3.0, -4.0, 4.0};
    PeriodicDomain another_periodic_domain(elastica::make_span(-2.0, 2.0),
                                           elastica::make_span(-3.0, 3.0),
                                           elastica::make_span(-4.0, 4.0));
    PeriodicDomain yet_another_periodic_domain(elastica::Vec3{-2.0, -3.0, -4.0},
                                               elastica::Vec3{2.0, 3.0, 4.0});
    /// [periodic_construction]

    using PeriodicDomains = std::vector<PeriodicDomain>;
    PeriodicDomains periodic_domains;
    periodic_domains.emplace_back(-2.0, 2.0, -3.0, 3.0, -4.0, 4.0);
    periodic_domains.emplace_back(elastica::make_span(-2.0, 2.0),
                                  elastica::make_span(-3.0, 3.0),
                                  elastica::make_span(-4.0, 4.0));
    periodic_domains.emplace_back(elastica::Vec3{-2.0, -3.0, -4.0},
                                  elastica::Vec3{2.0, 3.0, 4.0});

    auto apply = [&](auto f) {
      std::for_each(periodic_domains.cbegin(), periodic_domains.cend(), f);
    };

    auto check_upper_bound = [](auto const& domain) {
      CHECK(domain.get_upper_bound(elastica::EulerianFrame::X) == 2.0);
      CHECK(domain.get_upper_bound(elastica::EulerianFrame::Y) == 3.0);
      CHECK(domain.get_upper_bound(elastica::EulerianFrame::Z) == 4.0);

      CHECK(domain.template get_upper_bound<elastica::EulerianFrame::X>() ==
            2.0);
      CHECK(domain.template get_upper_bound<elastica::EulerianFrame::Y>() ==
            3.0);
      CHECK(domain.template get_upper_bound<elastica::EulerianFrame::Z>() ==
            4.0);
    };
    apply(check_upper_bound);

    auto check_lower_bound = [](auto const& domain) {
      CHECK(domain.get_lower_bound(elastica::EulerianFrame::X) == -2.0);
      CHECK(domain.get_lower_bound(elastica::EulerianFrame::Y) == -3.0);
      CHECK(domain.get_lower_bound(elastica::EulerianFrame::Z) == -4.0);

      CHECK(domain.template get_lower_bound<elastica::EulerianFrame::X>() ==
            -2.0);
      CHECK(domain.template get_lower_bound<elastica::EulerianFrame::Y>() ==
            -3.0);
      CHECK(domain.template get_lower_bound<elastica::EulerianFrame::Z>() ==
            -4.0);
    };
    apply(check_lower_bound);

    auto check_span = [](auto const& domain) {
      CHECK(domain.get_span(elastica::EulerianFrame::X) == 4.0);
      CHECK(domain.get_span(elastica::EulerianFrame::Y) == 6.0);
      CHECK(domain.get_span(elastica::EulerianFrame::Z) == 8.0);

      CHECK(domain.template get_span<elastica::EulerianFrame::X>() == 4.0);
      CHECK(domain.template get_span<elastica::EulerianFrame::Y>() == 6.0);
      CHECK(domain.template get_span<elastica::EulerianFrame::Z>() == 8.0);
    };
    apply(check_span);
  }

}  // namespace
