#include <vector>
#include <cmath>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
// needs to be included first
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/BaseTemplate.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/Blaze.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/SIMD.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/Scalar.hpp"
//
#include "Systems/common/Traits/DataType/BlazeBackend/DataTypeTraits.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Geometry/AxisAngleRotation.hpp"
#include "Utilities/Math/Sqrt.hpp"
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {

  using ::elastica::real_t;
  using Scalar = ::elastica::cosserat_rod::VectorTag<real_t>;
  using Vector = ::elastica::cosserat_rod::MatrixTag<real_t>;
  using Matrix = ::elastica::cosserat_rod::TensorTag<real_t>;

  ELASTICA_TEST_CASE(
      // clang-format: off
      "Unit.Systems.CosseratRods.Traits.Operations.inv_rotate_divide.scalar.correctness",
      "[Systems][Unit]") {
    // https://github.com/GazzolaLab/PyElastica/blob/583c4aec28573d1c88442812a6cea92416d38939/tests/test_math/test_rotations.py#L226
    // clang-format: on
    using namespace ::elastica::cosserat_rod;
    using Op = detail::InvRotateDivideOp<detail::InvRotateDivideKind::scalar>;

    Approx approx = Approx::custom();
    approx.epsilon(1.3e-10); // From 1e-10 extra in inv_rotate

    constexpr std::size_t dimension(3UL);
    constexpr std::size_t n_elem(2UL);


    Matrix::data_type input_director_collection(dimension, dimension, n_elem);
    input_director_collection = {{{1.,0.}, {0.,1.}, {0.,0.}},
                                 {{0.,0.}, {1.,0.}, {0.,1.}},
                                 {{0.,1.}, {0.,0.}, {1.,0.}}};

    const elastica::real_t ratio = 0.25;
    Scalar::data_type normalize_vector(n_elem-1);
    normalize_vector = ratio;

    Vector::data_type out_rot_matrix_batch(dimension, n_elem-1);
    Op::apply(out_rot_matrix_batch, input_director_collection,
                  normalize_vector);

    const elastica::real_t correct_angle = 120.0 * M_PI / 180.0 / ratio;

    for (std::size_t i(0UL); i < n_elem-1UL; ++i){
      auto out_vector_slice = Vector::slice(out_rot_matrix_batch, i);
      const elastica::real_t out_angle = blaze::l2Norm(out_vector_slice);
      CHECK(out_angle == approx(correct_angle));
      CHECK(out_vector_slice / out_angle == approx(1.0/sqrt(3)));
    }
  }
  ELASTICA_TEST_CASE(
      // clang-format: off
      "Unit.Systems.CosseratRods.Traits.Operations.inv_rotate_divide.scalar.circle_in_two_dimensions",
      "[Systems][Unit]") {
    // https://github.com/GazzolaLab/PyElastica/blob/583c4aec28573d1c88442812a6cea92416d38939/tests/test_math/test_rotations.py#L256
    // clang-format: on
    using namespace ::elastica::cosserat_rod;
    using ::elastica::real_t;
    using ::elastica::Vec3;
    using Op = detail::InvRotateDivideOp<detail::InvRotateDivideKind::scalar>;

    Approx approx = Approx::custom();
    approx.margin(1.0e-16);

    constexpr std::size_t dimension(3UL);
    constexpr std::size_t n_elem(512UL);

    Scalar::data_type theta;
    Vec3 axis_of_rotation;

    SECTION("Anti-Clockwise") {
      // Anti-Clockwise
      theta = blaze::linspace(n_elem, 0.0, 2.0 * M_PI);
      axis_of_rotation = {{0.0}, {0.0}, {-1.0}};
      CHECK(axis_of_rotation[2] == approx(-1.0));
    }
    SECTION("Clockwise") {
      // Anti-Clockwise
      theta = blaze::linspace(n_elem, 2.0 * M_PI, 0.0);
      axis_of_rotation = {{0.0}, {0.0}, {1.0}};
      CHECK(axis_of_rotation[2] == approx(1.0));
    }

    const real_t dtheta_di(
        blaze::abs(theta[1UL] - theta[0UL]));

    Matrix::data_type director(dimension, dimension, n_elem);
    director = 0.0;

    // Normal Direction
    blaze::row(blaze::pageslice(director, 0UL), 0UL) =
        -blaze::trans(blaze::cos(theta));
    blaze::row(blaze::pageslice(director, 0UL), 1UL) =
        -blaze::trans(blaze::sin(theta));

    // Tangent Direction
    blaze::row(blaze::pageslice(director, 1UL), 0UL) =
        -blaze::trans(blaze::sin(theta));
    blaze::row(blaze::pageslice(director, 1UL), 1UL) =
        blaze::trans(blaze::cos(theta));

    // D3 Direction
    blaze::row(blaze::pageslice(director, 2UL), 2UL) = -1.0;

    const elastica::real_t ratio = 1.00;
    Scalar::data_type normalize_vector(n_elem-1);
    normalize_vector = ratio;

    Vector::data_type test_axis_collection(dimension, n_elem-1);
    Op::apply(test_axis_collection, director, normalize_vector);

    CHECK(test_axis_collection.columns() == n_elem-1);
    CHECK(test_axis_collection.rows() == dimension);

    for (std::size_t i(0UL); i < n_elem-1UL; ++i){
      auto test_axis = Vector::slice(test_axis_collection, i);
      const elastica::real_t test_scaling = blaze::l2Norm(test_axis);
      CHECK_ITERABLE_CUSTOM_APPROX(test_axis / test_scaling, axis_of_rotation,
                                   approx);
      CHECK(test_scaling == approx(dtheta_di));
    }
  }
}  // namespace
