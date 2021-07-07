//
// Created by Tejaswin Parthasarathy on 5/7/21.
//

#include "CosseratRodTraits.hpp"
#include "Framework/TestingFramework.hpp"

// catch wants << declared and defined while comparing equality, so we include
// STD helpers
#include <functional>  // ref_wrapper
#include <sstream>     // ostream

#include "Utilities/StdHelpers.hpp"

// We also define a << for std::ref_wrapper since slice below wants it
template <typename T>
inline std::ostream& operator<<(std::ostream& os,
                                std::reference_wrapper<T> const& v) noexcept {
  os << v.get();
  return os;
}

namespace {

  ELASTICA_TEST_CASE("Unit.TestingFramework.CosseratRods.ScalarType",
                     "[Unit]") {
    using Traits = mock::ScalarType<double>;

    Traits::type d{45.03};
    CHECK(&Traits::slice(d, 635UL) == &d);
    CHECK(Traits::slice(d, 0UL) == approx(d));
    CHECK(Traits::slice(d, 635UL) == approx(d));

    const Traits::type f{45.02};
    CHECK(&Traits::slice(f, 635UL) == &f);
    CHECK(Traits::slice(f, 0UL) == approx(f));
    CHECK(Traits::slice(f, 635UL) == approx(f));
  }

  ELASTICA_TEST_CASE("Unit.TestingFramework.CosseratRods.VectorType",
                     "[Unit]") {
    using Traits = mock::VectorType<double>;

    static_assert(
        tt::assert_conforms_to<Traits,
                               elastica::cosserat_rod::protocols::DataTrait>,
        "Protocol Failure!");

    // slice
    Traits::type d{1.0, 2.0, 3.0, 4.0, 5.0};
    CHECK(&Traits::slice(d, 2UL) == &d[2]);

    const Traits::type f{6.0, 5.0, 4.0, 3.0, 2.0};
    CHECK(&Traits::slice(f, 4UL) == &f[4]);

    // slice on a range
    CHECK(Traits::slice(d, 1UL, 3UL) ==
          Traits::slice_type{d[1UL], d[2UL], d[3UL]});
    CHECK(Traits::slice(f, 1UL, 3UL) ==
          Traits::const_slice_type{f[1UL], f[2UL], f[3UL]});

    // slice of slice
    auto slice = Traits::slice(d, 1UL, 3UL);
    CHECK(&Traits::slice(slice, 1UL) == &d[2UL]);

    // ghost type
    CHECK(Traits::ghost_value() == approx(3.14159));
    Traits::resize(d, 10UL);
    CHECK(d == Traits::type{1.0, 2.0, 3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0});
  }

  ELASTICA_TEST_CASE("Unit.TestingFramework.CosseratRods.Placement", "[Unit]") {
    using PM = mock::PlacementMarker<0UL>;

    CHECK(PM::get_dofs(20UL) == 1UL);
    CHECK(PM::get_dofs(21UL) == 1UL);
    CHECK(PM::get_dofs(1221UL) == 1UL);

    static_assert(tt::assert_conforms_to<
                      mock::NodePlacementMarker,
                      elastica::cosserat_rod::protocols::PlacementTrait>,
                  "Protocol Failure!");
    CHECK(mock::NodePlacementMarker::get_dofs(20UL) == 21UL);
    static_assert(tt::assert_conforms_to<
                      mock::ElementPlacementMarker,
                      elastica::cosserat_rod::protocols::PlacementTrait>,
                  "Protocol Failure!");
    CHECK(mock::ElementPlacementMarker::get_dofs(20UL) == 20UL);
    static_assert(tt::assert_conforms_to<
                      mock::VoronoiPlacementMarker,
                      elastica::cosserat_rod::protocols::PlacementTrait>,
                  "Protocol Failure!");
    CHECK(mock::VoronoiPlacementMarker::get_dofs(20UL) == 19UL);
  }

}  // namespace
