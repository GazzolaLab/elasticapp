//
// Created by Tejaswin Parthasarathy on 5/11/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Traits/DataType/Protocols.hpp"
#include "Systems/CosseratRods/Traits/DataType/BlazeBackend/VectorTag.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  using namespace elastica::cosserat_rod;
  using real_type = double;
  using Traits = VectorTag<real_type>;

  static_assert(tt::assert_conforms_to<
                    Traits, elastica::cosserat_rod::protocols::DataTrait>,
                "Protocol Failure!");

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.blazeVectorTag",
                     "[Systems][Unit]") {
    using ghost_type = Traits::ghost_type;
    // check ghost
    {
      auto ghost = Traits::ghost_value();
      CHECK_ITERABLE_APPROX(ghost, (ghost_type{0.0}));
    }

    // check data type
    {
      // 10 matrix
      const std::size_t n_cols(10UL);
      Traits::data_type data(n_cols);

      // Initialization
      {
        for (std::size_t col_idx = 0UL; col_idx < n_cols; ++col_idx)
          data[col_idx] = static_cast<real_type>(col_idx);
      }

      const std::size_t col_slice_idx(3UL);
      const std::size_t n_cols_in_slice(2UL);
      auto slice = Traits::slice(data, col_slice_idx, n_cols_in_slice);
      {
        // slice of slice
        // check using slice of slice
        for (std::size_t idx = 0UL; idx < n_cols_in_slice; ++idx) {
          // slice at 0 and 1
          auto& first_slice = Traits::slice(slice, idx);
          CHECK(first_slice == approx(col_slice_idx + idx));
        }
      }

      {
        CHECK(data[col_slice_idx] == approx(3.0));
        CHECK(data[col_slice_idx + 1UL] == approx(4.0));

        for (std::size_t idx = 0UL; idx < n_cols_in_slice; ++idx)
          // this [] should be forbidden!
          slice[idx] += 1.0;

        CHECK(data[col_slice_idx] == approx(4.0));
        CHECK(data[col_slice_idx + 1UL] == approx(5.0));

        // update the slice and see if it reflects in the original data
        // check using slice of slice
        for (std::size_t idx = 0UL; idx < n_cols_in_slice; ++idx) {
          // need a ref here
          Traits::slice(slice, idx) += 1.0;
        }
        // this [] should be forbidden!

        CHECK(data[col_slice_idx] == approx(5.0));
        CHECK(data[col_slice_idx + 1UL] == approx(6.0));

        // use a & to const slice
        auto first_slice = Traits::slice(slice, 0UL);
        auto const const_first_slice =
            Traits::slice(static_cast<decltype(slice) const&>(slice), 0UL);
        // for iterable to work convert to a static vector
        CHECK(first_slice == approx(const_first_slice));
      }

      {
        // chekc slice( data const &)
        auto const const_slice =
            Traits::slice(static_cast<decltype(data) const&>(data),
                          col_slice_idx, n_cols_in_slice);
        // CHECK_ITERABLE_APPROX(ghost_type(const_slice), ghost_type(slice));
        CHECK(slice[0UL] == approx(const_slice[0UL]));
        CHECK(slice[1UL] == approx(const_slice[1UL]));

        // check slice(const_slice_type &)
        auto const first_const_slice = Traits::slice(const_slice, 0UL);
        auto first_slice = Traits::slice(slice, 0UL);
        CHECK((first_slice) == approx(first_const_slice));
      }

      // check resizing
      {
        Traits::resize(data, n_cols + 5UL);
        CHECK(data.size() == 15UL);
      }
    }
  }

}  // namespace
