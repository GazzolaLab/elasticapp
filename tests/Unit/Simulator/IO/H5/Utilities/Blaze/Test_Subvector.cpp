//
// Created by Tejaswin Parthasarathy on 7/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Simulator/IO/H5/Utilities/Blaze/Subvector.hpp"
#include "Utilities/CountingIterator.hpp"
//
#include <blaze/math/DynamicVector.h>
//
#include <algorithm>
#include <ranges>

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
    CHECK(std::ranges::equal(
        a, std::ranges::iota_view{7, 7 + static_cast<int>(slice_length)}));

    // Write and flush
    elastica::io::write_data(group, a, "data");
    group.getFile().flush();

    // Clobber data
    std::iota(vector.begin(), vector.end(), -200);
    CHECK(std::ranges::equal(
        a,
        std::ranges::iota_view{-195, -195 + static_cast<int>(slice_length)}));

    // Read data back
    elastica::io::read_data(group, a, "data");

    // Post check
    CHECK(std::ranges::equal(
        a, std::ranges::iota_view{7, 7 + static_cast<int>(slice_length)}));
    CHECK(std::equal(vector.begin(), vector.begin() + slice_start,
                     std::ranges::iota_view{-200}.begin()));
    CHECK(std::equal(
        vector.begin() + slice_start + slice_length, vector.end(),
        std::ranges::iota_view{-200 + int(slice_start + slice_length)}
            .begin()));
  }

}  // namespace
