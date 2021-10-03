//
// Created by Tejaswin Parthasarathy on 5/11/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Traits/DataType/Protocols.hpp"
#include "Systems/CosseratRods/Traits/DataType/BlazeBackend/TensorTag.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace elastica::cosserat_rod;

  using real_type = double;
  // doesn't pass because of some issue that is not critical for now
  using Traits = TensorTag<real_type>;
  static_assert(tt::assert_conforms_to<
                    Traits, elastica::cosserat_rod::protocols::DataTrait>,
                "Protocol Failure!");

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.blazeTensorTag",
                     "[Systems][Unit]") {
    using ghost_type = Traits::ghost_type;
    // check ghost
    {
      auto ghost = Traits::ghost_value();
      auto expected_ghost = ghost_type{
          {{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}}, {{0.0}, {0.0}, {1.0}}};
      // (page, row, column)
      auto checker = [&ghost, &expected_ghost](auto Is) {
        CHECK_ITERABLE_APPROX(
            blaze::row<Is>(blaze::columnslice(ghost, 0UL)),
            blaze::row<Is>(blaze::columnslice(expected_ghost, 0UL)));
      };
      index_apply<3UL>(
          [checker](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(checker(Is)); });
    }

    // check data type
    {
      // 3 x 3 x 5 matrix (hardcoded to 3 x 3 inside, so we retain dimension)
      const std::size_t n_pages(3UL);
      const std::size_t n_rows(3UL);
      const std::size_t n_cols(5UL);
      Traits::data_type data(n_pages, n_rows, n_cols);

      // Initialization
      {
        for (std::size_t page_idx = 0UL; page_idx < n_pages; ++page_idx)
          for (std::size_t row_idx = 0UL; row_idx < n_rows; ++row_idx)
            for (std::size_t col_idx = 0UL; col_idx < n_cols; ++col_idx)
              data(page_idx, row_idx, col_idx) =
                  static_cast<real_type>(page_idx + row_idx + col_idx);
      }

      // single slice of data
      const std::size_t col_slice_idx(3UL);
      // expected to be (3, 4, 2)
      auto slice = Traits::slice(data, col_slice_idx, 2UL);
      REQUIRE(slice.pages() == 3UL);
      REQUIRE(slice.rows() == 3UL);
      REQUIRE(slice.columns() == 2UL);
      {
        // slice of slice
        // submatrix of size (3, 3)
        auto first_slice = Traits::slice(slice, 0UL);
        REQUIRE(first_slice.rows() == 3UL);
        REQUIRE(first_slice.columns() == 3UL);

        // check using slice of slice
        for (std::size_t page_idx = 0UL; page_idx < n_pages; ++page_idx)
          for (std::size_t row_idx = 0UL; row_idx < n_rows; ++row_idx)
            // this [] should be forbidden!
            CHECK(first_slice(page_idx, row_idx) ==
                  approx(page_idx + row_idx + col_slice_idx));
      }

      {
        CHECK(data(0UL, 0UL, col_slice_idx) == approx(3.0));
        CHECK(data(0UL, 1UL, col_slice_idx) == approx(4.0));
        CHECK(data(0UL, 2UL, col_slice_idx) == approx(5.0));

        CHECK(data(1UL, 0UL, col_slice_idx) == approx(4.0));
        CHECK(data(1UL, 1UL, col_slice_idx) == approx(5.0));
        CHECK(data(1UL, 2UL, col_slice_idx) == approx(6.0));

        CHECK(data(2UL, 0UL, col_slice_idx) == approx(5.0));
        CHECK(data(2UL, 1UL, col_slice_idx) == approx(6.0));
        CHECK(data(2UL, 2UL, col_slice_idx) == approx(7.0));

        for (std::size_t page_idx = 0UL; page_idx < n_pages; ++page_idx)
          for (std::size_t row_idx = 0UL; row_idx < n_rows; ++row_idx)
            // this [] should be forbidden!
            slice(page_idx, row_idx, 0UL) += 1.0;

        CHECK(data(0UL, 0UL, col_slice_idx) == approx(4.0));
        CHECK(data(0UL, 1UL, col_slice_idx) == approx(5.0));
        CHECK(data(0UL, 2UL, col_slice_idx) == approx(6.0));

        CHECK(data(1UL, 0UL, col_slice_idx) == approx(5.0));
        CHECK(data(1UL, 1UL, col_slice_idx) == approx(6.0));
        CHECK(data(1UL, 2UL, col_slice_idx) == approx(7.0));

        CHECK(data(2UL, 0UL, col_slice_idx) == approx(6.0));
        CHECK(data(2UL, 1UL, col_slice_idx) == approx(7.0));
        CHECK(data(2UL, 2UL, col_slice_idx) == approx(8.0));

        // submatrix of size (3, 3)
        // update the slice and see if it reflects in the original data
        auto first_slice = Traits::slice(slice, 0UL);
        // check using slice of slice
        for (std::size_t page_idx = 0UL; page_idx < n_pages; ++page_idx)
          for (std::size_t row_idx = 0UL; row_idx < n_rows; ++row_idx)
            // this [] should be forbidden!
            first_slice(page_idx, row_idx) += 1.0;

        CHECK(data(0UL, 0UL, col_slice_idx) == approx(5.0));
        CHECK(data(0UL, 1UL, col_slice_idx) == approx(6.0));
        CHECK(data(0UL, 2UL, col_slice_idx) == approx(7.0));

        CHECK(data(1UL, 0UL, col_slice_idx) == approx(6.0));
        CHECK(data(1UL, 1UL, col_slice_idx) == approx(7.0));
        CHECK(data(1UL, 2UL, col_slice_idx) == approx(8.0));

        CHECK(data(2UL, 0UL, col_slice_idx) == approx(7.0));
        CHECK(data(2UL, 1UL, col_slice_idx) == approx(8.0));
        CHECK(data(2UL, 2UL, col_slice_idx) == approx(9.0));

        // use a & to const slice
        auto const const_first_slice =
            Traits::slice(static_cast<decltype(slice) const&>(slice), 0UL);
        REQUIRE(const_first_slice.rows() == 3UL);
        REQUIRE(const_first_slice.columns() == 3UL);

        // for iterable to work convert to a static vector
        auto checker = [&first_slice, &const_first_slice](std::size_t index) {
          CHECK_ITERABLE_APPROX(
              std::vector<double>(first_slice.cbegin(index),
                                  first_slice.cend(index)),
              std::vector<double>(const_first_slice.cbegin(index),
                                  const_first_slice.cend(index)));
        };
        for (std::size_t row_idx = 0UL; row_idx < n_rows; ++row_idx)
          checker(row_idx);
      }

      {
        // chekc slice( data const &)
        auto const const_slice = Traits::slice(
            static_cast<decltype(data) const&>(data), col_slice_idx, 2UL);
        REQUIRE(slice.pages() == 3UL);
        REQUIRE(slice.rows() == 3UL);
        REQUIRE(slice.columns() == 2UL);

        //        CHECK_ITERABLE_APPROX(ghost_type(const_slice),
        //        ghost_type(slice));
        for (std::size_t page_idx = 0UL; page_idx < n_pages; ++page_idx)
          for (std::size_t row_idx = 0UL; row_idx < n_rows; ++row_idx)
            for (std::size_t col_idx = 0UL; col_idx < 2UL; ++col_idx)
              CHECK(slice(page_idx, row_idx, col_idx) ==
                    approx(const_slice(page_idx, row_idx, col_idx)));

        // check slice(const_slice_type &)
        auto const first_const_slice = Traits::slice(const_slice, 0UL);
        REQUIRE(first_const_slice.rows() == 3UL);
        REQUIRE(first_const_slice.columns() == 3UL);

        auto first_slice = Traits::slice(slice, 0UL);

        auto checker = [&first_slice, &first_const_slice](std::size_t index) {
          CHECK_ITERABLE_APPROX(
              std::vector<double>(first_slice.cbegin(index),
                                  first_slice.cend(index)),
              std::vector<double>(first_const_slice.cbegin(index),
                                  first_const_slice.cend(index)));
        };
        for (std::size_t row_idx = 0UL; row_idx < n_rows; ++row_idx)
          checker(row_idx);
      }

      // check resizing
      {
        Traits::resize(data, n_cols + 5UL);
        CHECK(data.pages() == 3UL);
        CHECK(data.rows() == 3UL);
        CHECK(data.columns() == 10UL);
      }
    }
  }

}  // namespace