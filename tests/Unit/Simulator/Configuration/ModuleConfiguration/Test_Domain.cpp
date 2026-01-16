//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <cstddef>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Domain.hpp"
#include "Simulator/Configuration/TypeTraits/Domain.hpp"
#include "Simulator/Domain.hpp"
#include "Simulator/Frames.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Math/Infinity.hpp"

namespace {

  using namespace elastica::configuration;
  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Domain.make_domain_configuration",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    /// [unbounded_eg]
    auto&& unbounded_domain_configuration = make_domain_configuration(
        elastica::boundary_along<elastica::EulerianFrame::X,
                                 elastica::IsUnbounded>(),
        elastica::boundary_along<elastica::EulerianFrame::Y,
                                 elastica::IsUnbounded>(),
        elastica::boundary_along<elastica::EulerianFrame::Z,
                                 elastica::IsUnbounded>());
    /// [unbounded_eg]

    static_assert(
        ::elastica::tt::IsValidDomainConfiguration<std::remove_cvref_t<
            decltype(unbounded_domain_configuration)>>::value,
        "meta failure!");

    static_assert(
        std::is_same_v<
            std::remove_cvref_t<decltype(unbounded_domain_configuration)>,
            DomainConfiguration<elastica::IsUnbounded, elastica::IsUnbounded,
                                elastica::IsUnbounded>>,
        "meta failure");

    CHECK(unbounded_domain_configuration.domain().get_upper_bound(
              elastica::EulerianFrame::Y) == (elastica::real_t(elastica::inf)));
    CHECK(unbounded_domain_configuration.domain().get_lower_bound(
              elastica::EulerianFrame::X) ==
          (elastica::real_t(-elastica::inf)));

    /// [periodic_eg]
    auto&& periodic_domain_configuration = make_domain_configuration(
        elastica::boundary_along<elastica::EulerianFrame::X,
                                 elastica::IsPeriodic>(-5.0, 5.0),
        elastica::boundary_along<elastica::EulerianFrame::Y,
                                 elastica::IsPeriodic>(-50.0, 50.0),
        elastica::boundary_along<elastica::EulerianFrame::Z,
                                 elastica::IsPeriodic>(-5.0, 5.0));
    /// [periodic_eg]

    static_assert(
        ::elastica::tt::IsValidDomainConfiguration<std::remove_cvref_t<
            decltype(periodic_domain_configuration)>>::value,
        "meta failure!");
    static_assert(
        std::is_same_v<
            std::remove_cvref_t<decltype(periodic_domain_configuration)>,
            DomainConfiguration<elastica::IsPeriodic, elastica::IsPeriodic,
                                elastica::IsPeriodic>>,
        "meta failure");
    CHECK(periodic_domain_configuration.domain().get_upper_bound(
              elastica::EulerianFrame::Y) == approx(50.0));
    CHECK(periodic_domain_configuration.domain().get_lower_bound(
              elastica::EulerianFrame::X) == approx(-5.0));

    /// [mixed_eg]
    // periodic only in the Y direction
    auto&& mixed_domain_configuration = make_domain_configuration(
        elastica::boundary_along<elastica::EulerianFrame::X,
                                 elastica::IsUnbounded>(),
        elastica::boundary_along<elastica::EulerianFrame::Y,
                                 elastica::IsPeriodic>(-54.0, 54.0),
        elastica::boundary_along<elastica::EulerianFrame::Z,
                                 elastica::IsUnbounded>());
    /// [mixed_eg]

    static_assert(
        ::elastica::tt::IsValidDomainConfiguration<
            std::remove_cvref_t<decltype(mixed_domain_configuration)>>::value,
        "meta failure!");
    static_assert(
        std::is_same_v<
            std::remove_cvref_t<decltype(mixed_domain_configuration)>,
            DomainConfiguration<elastica::IsUnbounded, elastica::IsPeriodic,
                                elastica::IsUnbounded>>,
        "meta failure");
    CHECK(mixed_domain_configuration.domain().get_upper_bound(
              elastica::EulerianFrame::Y) == approx(54.0));
    CHECK(mixed_domain_configuration.domain().get_lower_bound(
              elastica::EulerianFrame::Y) == approx(-54.0));

    CHECK(mixed_domain_configuration.domain().get_upper_bound(
              elastica::EulerianFrame::Z) == (elastica::real_t(elastica::inf)));
    CHECK(mixed_domain_configuration.domain().get_lower_bound(
              elastica::EulerianFrame::X) ==
          (elastica::real_t(-elastica::inf)));

    {
      /// [shorthand_eg]
      using namespace elastica;
      using namespace elastica::configuration;
      auto&& domain_configuration = make_domain_configuration(
          boundary_along<EulerianFrame::X, IsUnbounded>(),
          boundary_along<EulerianFrame::Y, IsUnbounded>(),
          boundary_along<EulerianFrame::Z, IsUnbounded>());
      /// [shorthand_eg]
      ignore_unused(domain_configuration);
    }
  }

}  // namespace