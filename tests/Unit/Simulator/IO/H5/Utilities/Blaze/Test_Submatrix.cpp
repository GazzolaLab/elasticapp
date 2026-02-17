//
// Created by Tejaswin Parthasarathy on 7/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Simulator/IO/H5/Utilities/Blaze/Submatrix.hpp"
#include "Utilities/CountingIterator.hpp"
//
#include <blaze/math/DynamicMatrix.h>
//
#include <algorithm>
#include <ranges>

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.blaze_submatrix",
                     "[Unit][Simulator][IO]") {
    auto group = generate_h5("blaze_submatrix");
    INFO(group.getFile().getName());

    //    auto msize = [](auto& matrix, bool is_column_major) {
    //      return is_column_major ? matrix.columns() : matrix.rows();
    //    };
    auto get_extent = [](bool is_column_major, auto col, auto row) {
      return is_column_major ? col : row;
    };

    auto run_matrix_test = [&](auto& matrix) {
      static constexpr auto column_major =
          blaze::IsColumnMajorMatrix<std::remove_cvref_t<decltype(matrix)>>{};

      const auto total =
          get_extent(column_major, matrix.columns(), matrix.rows());
      for (auto i = 0UL; i < total; ++i)
        std::iota(matrix.begin(i), matrix.end(i), i + 10);

      struct range {
        std::size_t start, size;
      };
      const range row_range{0UL, matrix.rows() - 1};
      const range col_range{1UL, matrix.columns() - 2};
      auto a = blaze::submatrix(matrix, row_range.start, col_range.start,
                                row_range.size, col_range.size);

      const auto start = get_extent(column_major, col_range, row_range).start;
      const auto stop =
          start + get_extent(column_major, col_range, row_range).size;

      const auto stride_start =
          get_extent(!column_major, col_range, row_range).start;
      const auto stride_stop =
          stride_start + get_extent(!column_major, col_range, row_range).size;

      const auto run_check = [=]() {
        for (auto i = start; i < stop; ++i) {
          auto s = i + 10 + stride_start;
          auto ss = i + 10 + stride_stop;
          CAPTURE(i, stride_start, stride_stop);
          CHECK(
              std::ranges::equal(std::ranges::subrange(a.cbegin(i), a.cend(i)),
                                 std::ranges::iota_view{s, ss}));
        }
      };
      run_check();

      // Write and flush
      elastica::io::write_data(group, a, "data");
      group.getFile().flush();

      // Clobber data
      for (auto i = 0UL; i < total; ++i)
        std::iota(matrix.begin(i), matrix.end(i), i + 200);

      for (auto i = start; i < stop; ++i) {
        auto s = i + 200 + stride_start;
        auto ss = i + 200 + stride_stop;
        CAPTURE(i, stride_start, stride_stop);
        CHECK(std::ranges::equal(std::ranges::subrange(a.cbegin(i), a.cend(i)),
                                 std::ranges::iota_view{s, ss}));
      }

      // Read data back
      elastica::io::read_data(group, a, "data");

      // Test again
      run_check();

      // Check to see if only third row is not affected, since testing others
      // is a paing
      for (auto i = stop; i < total; ++i)
        CHECK(std::equal(matrix.cbegin(i), matrix.cend(i),
                         std::ranges::iota_view{i + 200}.begin()));
    };

    SECTION("Row major") {
      // Setup
      blaze::DynamicMatrix<int, blaze::rowMajor> matrix(3UL, 5UL);
      run_matrix_test(matrix);
    }
// https://github.com/tp5uiuc/elasticapp/issues/412
// Resolve column major tests
#define ISSUE_412_RESOLVED 0
#if ISSUE_412_RESOLVED
    SECTION("Column major") {
      blaze::DynamicMatrix<int, blaze::columnMajor> matrix(3UL, 5UL);
      run_matrix_test(matrix);
    }
#endif
  }

}  // namespace
