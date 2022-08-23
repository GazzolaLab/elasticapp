#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
//
// needs to be included first
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/BaseTemplate.hpp"
//#include
//#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/SIMD.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/Scalar.hpp"
//
#include "Systems/CosseratRods/Traits/DataType/BlazeBackend/DataTypeTraits.hpp"
//
#include "Systems/States/Expressions/SO3.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Vec3.hpp"

#if defined(ELASTICA_USE_BLAZE)

namespace {

  using ::elastica::real_t;
  using ::elastica::Vec3;
  using Scalar = ::elastica::cosserat_rod::VectorTag<real_t>;
  using Vector = ::elastica::cosserat_rod::MatrixTag<real_t>;
  using Matrix = ::elastica::cosserat_rod::TensorTag<real_t>;

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3_add_assign.scalar.basis_rotate",
                     "[Systems][Unit]") {
    // https://github.com/GazzolaLab/PyElastica/blob/583c4aec28573d1c88442812a6cea92416d38939/tests/test_math/test_rotations.py#L184
    using namespace elastica::states;

    Approx approx = Approx::custom()
                        .epsilon(1.2e-12)
                        .margin(1e-17);

    const std::size_t n_elem(1UL);
    const std::size_t dimension(3UL);

    real_t test_theta;
    Vector::data_type vector_collection(dimension, n_elem);
    Matrix::data_type correct_rot_mat(dimension, dimension, n_elem);

    auto test_theta_deg = GENERATE(range(-89.0, 89.0, 0.5));
    test_theta = test_theta_deg * M_PI / 180.0;
    const real_t cost = std::cos(test_theta);
    const real_t sint = std::sin(test_theta);

    SECTION("z-axis") {
      vector_collection = 0.0;
      blaze::row(vector_collection, 2UL) = test_theta;
      correct_rot_mat = {{{cost}, {sint}, {0.0}},
                         {{-sint}, {cost}, {0.0}},
                         {{0.0}, {0.0}, {1.0}}};
    }
    SECTION("y-axis") {
      vector_collection = 0.0;
      blaze::row(vector_collection, 1UL) = test_theta;
      correct_rot_mat = {{{cost}, {0.0}, {-sint}},
                         {{0.0}, {1.0}, {0.0}},
                         {{sint}, {0.0}, {cost}}};
    }
    SECTION("x-axis") {
      vector_collection = 0.0;
      blaze::row(vector_collection, 0UL) = test_theta;
      correct_rot_mat = {{{1.0}, {0.0}, {0.0}},
                         {{0.0}, {cost}, {sint}},
                         {{0.0}, {-sint}, {cost}}};
    }

    Matrix::data_type test_rot_mat(dimension, dimension, n_elem);
    Matrix::slice(test_rot_mat, 0UL) = {
        {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

    // Operation
    constexpr auto backend = detail::SO3AddAssignKind::scalar;
    detail::SO3AddAssign<backend>::apply(test_rot_mat,
                                         vector_collection);

    for (std::size_t i(0UL); i < dimension; ++i) {
      auto test_row = blaze::row(blaze::columnslice(test_rot_mat, 0UL), i);
      auto correct_row =
          blaze::row(blaze::columnslice(correct_rot_mat, 0UL), i);

      CHECK_ITERABLE_CUSTOM_APPROX(test_row, correct_row, approx);
    }

  }  // namespace
}

#endif  // ELASTICA_USE_BLAZE

