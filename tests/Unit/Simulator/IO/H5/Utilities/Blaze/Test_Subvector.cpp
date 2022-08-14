//
// Created by Tejaswin Parthasarathy on 7/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Simulator/IO/H5/Utilities/Blaze/Subvector.hpp"
#include "Utilities/CountingIterator.hpp"
//
#include <algorithm>
#include <blaze/math/DynamicVector.h>

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.blaze_subvector",
                     "[Unit][Simulator][IO]") {
    auto group = generate_h5();
    // Setup
    blaze::DynamicVector<int> vector(20UL);
    std::iota(vector.begin(), vector.end(), 2);

    const auto slice_start = 5UL;
    const auto slice_length = 10UL;
    auto a = blaze::subvector(vector, slice_start, slice_length);

    // Pre check
    CHECK(std::equal(a.begin(), a.end(), elastica::make_counting_iterator(7)));

    // Write and flush
    elastica::io::write_data(group, a, "data");
    group.getFile().flush();

    // Clobber data
    std::iota(vector.begin(), vector.end(), -200);
    CHECK(
        std::equal(a.begin(), a.end(), elastica::make_counting_iterator(-195)));

    // Read data back
    elastica::io::read_data(group, a, "data");

    // Post check
    CHECK(std::equal(a.begin(), a.end(), elastica::make_counting_iterator(7)));
    CHECK(std::equal(vector.begin(), vector.begin() + slice_start,
                     elastica::make_counting_iterator(-200)));
    CHECK(std::equal(vector.begin() + slice_start + slice_length, vector.end(),
                     elastica::make_counting_iterator(
                         -200 + int(slice_start + slice_length))));
  }

}  // namespace
