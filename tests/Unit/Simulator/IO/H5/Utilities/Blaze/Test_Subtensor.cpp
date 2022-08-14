//
// Created by Tejaswin Parthasarathy on 7/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Simulator/IO/H5/Utilities/Blaze/Subtensor.hpp"
#include "Utilities/CountingIterator.hpp"
//
#include <algorithm>
#include <blaze_tensor/math/DynamicTensor.h>

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.blaze_subtensor",
                     "[Unit][Simulator][IO]") {
    auto group = generate_h5();
    // Setup
    blaze::DynamicTensor<int> tensor(3UL, 3UL, 6UL);
    auto const pages = tensor.pages();
    auto const rows = tensor.rows();
    auto const cols = tensor.columns();

    for (auto k = 0UL; k < pages; ++k)
      for (auto i = 0UL; i < rows; ++i)
        std::iota(tensor.begin(i, k), tensor.end(i, k), 2 + i * k);

    const auto slice_start = 1UL;
    const auto slice_length = 4UL;
    auto a =
        blaze::subtensor(tensor, 0UL, 0UL, slice_start, 2UL, 2UL, slice_length);

    const auto run_check = [=](std::string const& msg) {
      INFO(msg);
      for (auto k = 0UL; k < 2; ++k)
        for (auto i = 0UL; i < 2; ++i) {
          const int val = 2 + i * k + slice_start;
          CAPTURE(i, k);
          CHECK(std::vector<int>(a.cbegin(i, k), a.cend(i, k)) ==
                std::vector<int>(
                    elastica::make_counting_iterator(val),
                    elastica::make_counting_iterator(val + int(slice_length))));
        }
    };
    run_check("pre");

    // Write and flush
    elastica::io::write_data(group, a, "data");
    group.getFile().flush();

    // Clobber data
    for (auto k = 0UL; k < pages; ++k)
      for (auto i = 0UL; i < rows; ++i)
        std::iota(tensor.begin(i, k), tensor.end(i, k),
                  -200 + int((i + 1) * (k + 1)));

    // Check clobbering
    for (auto k = 0UL; k < 2; ++k)
      for (auto i = 0UL; i < 2; ++i) {
        const int val = (i + 1) * (k + 1) + slice_start;
        CHECK(std::vector<int>(a.cbegin(i, k), a.cend(i, k)) ==
              std::vector<int>(elastica::make_counting_iterator(-200 + val),
                               elastica::make_counting_iterator(
                                   -200 + val + int(slice_length))));
      }

    // Read data back
    elastica::io::read_data(group, a, "data");

    // Post check
    run_check("post");

    // last row of tensor should be unmodified.
    CHECK(std::vector<int>(tensor.cbegin(2UL, 2UL), tensor.cend(2UL, 2UL)) ==
          std::vector<int>(elastica::make_counting_iterator(-191),
                           elastica::make_counting_iterator(-191 + int(cols))));
  }

}  // namespace
