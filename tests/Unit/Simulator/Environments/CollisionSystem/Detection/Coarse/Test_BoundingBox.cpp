//
// Created by Tejaswin Parthasarathy on 9/25/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/BoundingBox.hpp"
#include "Simulator/Frames.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/DefineTypes.h"

namespace {

  using namespace elastica::collision::detection::coarse;

  using AABB = BoundingBox<::elastica::real_t>;

  struct Dimensions : std::integral_constant<std::size_t, 3> {};

  auto get_aabb() { return AABB(0.2, 0.3, 0.4, 0.6, 0.7, 0.8); }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox",
                     "[Unit][Environments]") {
    // empty
    AABB nouse_aabb;

    // test named and copy constructor
    AABB test_aabb(get_aabb());
    CHECK(test_aabb[0] == approx(0.2));
    CHECK(test_aabb[1] == approx(0.3));
    CHECK(test_aabb[2] == approx(0.4));
    CHECK(test_aabb[3] == approx(0.6));
    CHECK(test_aabb[4] == approx(0.7));
    CHECK(test_aabb[5] == approx(0.8));

    // assignable
    test_aabb[1] = 0.5;
    CHECK(test_aabb[1] == approx(0.5));

    // not assignable
    // cpp17::as_const(test_aabb)[1] = 0.5;

    // explicit copy constructor
    AABB another_aabb(test_aabb);
    CHECK(another_aabb[0] == approx(0.2));
    CHECK(another_aabb[1] == approx(0.5));
    CHECK(another_aabb[2] == approx(0.4));
    CHECK(another_aabb[3] == approx(0.6));
    CHECK(another_aabb[4] == approx(0.7));
    CHECK(another_aabb[5] == approx(0.8));

    // explicit copy assignment
    AABB yet_another_aabb = test_aabb;
    CHECK(yet_another_aabb[0] == approx(0.2));
    CHECK(yet_another_aabb[1] == approx(0.5));
    CHECK(yet_another_aabb[2] == approx(0.4));
    CHECK(yet_another_aabb[3] == approx(0.6));
    CHECK(yet_another_aabb[4] == approx(0.7));
    CHECK(yet_another_aabb[5] == approx(0.8));
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.is_valid",
                     "[Unit][Environments]") {
    AABB aabb(get_aabb());
    CHECK(aabb.is_valid());

    AABB invalid_aabb(get_aabb());
    for (std::size_t i = 0UL; i < Dimensions(); ++i) {
      auto temp = cpp17::as_const(invalid_aabb)[i];
      invalid_aabb[i] = 0.9;
      CHECK(not invalid_aabb.is_valid());
      invalid_aabb[i] = temp;
    }

    invalid_aabb.reset();
    CHECK(invalid_aabb.is_valid());
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.equality",
                     "[Unit][Environments]") {
    AABB first(get_aabb()), second(get_aabb());

    CHECK(first.equal(second));
    CHECK(first == second);
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.center",
                     "[Unit][Environments]") {
    AABB first(0.2, 0.3, 0.4, 0.6, 0.7, 0.8);

    CHECK(first.center<::elastica::EulerianFrame::X>() == approx(0.4));
    CHECK(first.center<::elastica::EulerianFrame::Y>() == approx(0.5));
    CHECK(first.center<::elastica::EulerianFrame::Z>() == approx(0.6));
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.length",
                     "[Unit][Environments]") {
    AABB first(0.2, 0.3, 0.4, 0.6, 0.7, 0.8);

    CHECK(first.length<::elastica::EulerianFrame::X>() == approx(0.4));
    CHECK(first.length<::elastica::EulerianFrame::Y>() == approx(0.4));
    CHECK(first.length<::elastica::EulerianFrame::Z>() == approx(0.4));
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.CoarseDetection.BoundingBox.length_along",
      "[Unit][Environments]") {
    AABB first(0.2, 0.3, 0.4, 0.6, 0.7, 0.8);

    CHECK(length_along<::elastica::EulerianFrame::X>(first) == approx(0.4));
    CHECK(length_along<::elastica::EulerianFrame::Y>(first) == approx(0.4));
    CHECK(length_along<::elastica::EulerianFrame::Z>(first) == approx(0.4));
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.overlaps",
                     "[Unit][Environments]") {
    AABB first(0.2, 0.3, 0.4, 0.6, 0.7, 0.8);
    AABB should_overlap(first);
    ::elastica::real_t shift[Dimensions()] = {0.2, 0.2, 0.2};

    {
      // bit flip for shifts
      for (int z : {-1, 1})
        for (int y : {-1, 1})
          for (int x : {-1, 1}) {
            // actually overlaps
            should_overlap[0] += x * shift[0];
            should_overlap[1] += y * shift[1];
            should_overlap[2] += z * shift[2];
            should_overlap[3] += x * shift[0];
            should_overlap[4] += y * shift[1];
            should_overlap[5] += z * shift[2];

            CHECK(first.overlaps(should_overlap));

            // is slightly beyond the border,
            should_overlap[0] += 1.1 * x * shift[0];
            should_overlap[1] += 1.1 * y * shift[1];
            should_overlap[2] += 1.1 * z * shift[2];
            should_overlap[3] += 1.1 * x * shift[0];
            should_overlap[4] += 1.1 * y * shift[1];
            should_overlap[5] += 1.1 * z * shift[2];

            CHECK(not first.overlaps(should_overlap));
            CHECK(first.overlaps(should_overlap, ::elastica::real_t(0.2)));

            should_overlap = first;
          }
    }

    AABB should_not_overlap(-0.3, -0.2, -0.1, 0.1, 0.2, 0.4);
    CHECK(not first.overlaps(should_not_overlap));
    CHECK(first.overlaps(should_not_overlap, ::elastica::real_t(0.1)));
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.contains",
                     "[Unit][Environments]") {
    AABB first(0.2, 0.3, 0.4, 0.6, 0.7, 0.8);

    // contains the center and the near-edge points
    ::elastica::real_t shift = 0.08;
    ::elastica::real_t center[3] = {0.4, 0.5, 0.6};
    int shift_coeffs[3] = {0, 1, 2};

    for (int z : shift_coeffs)
      for (int y : shift_coeffs)
        for (int x : shift_coeffs) {
          CHECK(first.contains(center[0] + x * shift, center[1] + y * shift,
                               center[2] + z * shift));
          CHECK(first.contains(center[0] - x * shift, center[1] - y * shift,
                               center[2] - z * shift));
        }

    for (int z : shift_coeffs)
      for (int y : shift_coeffs)
        for (int x : shift_coeffs) {
          if (x == 0 and y == 0 and z == 0)
            continue;
          CHECK(not first.contains(first[0] - x * shift, first[1] - y * shift,
                                   first[2] - z * shift));
          CHECK(not first.contains(first[3] + x * shift, first[4] + y * shift,
                                   first[5] + z * shift));
        }

    for (int z : {1, 2})
      for (int y : {1, 2})
        for (int x : {1, 2}) {
          CHECK(first.contains(
              AABB(center[0] - x * shift, center[1] - y * shift,
                   center[2] - z * shift, center[0] + x * shift,
                   center[1] + y * shift, center[2] + z * shift)));
        }
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.merge",
                     "[Unit][Environments]") {
    // Two bounding
    AABB first(0.0, 0.1, 0.5, 0.3, 0.4, 0.8);
    AABB second(0.2, 0.3, 0.4, 0.6, 0.7, 0.6);

    // Free form merge function
    AABB result = first + second;
    CHECK(result[0] == approx(0.0));
    CHECK(result[1] == approx(0.1));
    CHECK(result[2] == approx(0.4));
    CHECK(result[3] == approx(0.6));
    CHECK(result[4] == approx(0.7));
    CHECK(result[5] == approx(0.8));

    AABB another_result = merge(first, second);
    CHECK(result == another_result);

    // in-place merge
    AABB yet_another_result = first;
    yet_another_result += second;
    CHECK(yet_another_result == result);
  }

  ELASTICA_TEST_CASE("Unit.Environments.CoarseDetection.BoundingBox.intersect",
                     "[Unit][Environments]") {
    // Two bounding
    AABB first(0.0, 0.1, 0.5, 0.3, 0.4, 0.8);
    AABB second(0.2, 0.3, 0.4, 0.6, 0.7, 0.6);

    // Free form intersect function
    AABB result = intersect(first, second);
    CHECK(result[0] == approx(0.2));
    CHECK(result[1] == approx(0.3));
    CHECK(result[2] == approx(0.5));
    CHECK(result[3] == approx(0.3));
    CHECK(result[4] == approx(0.4));
    CHECK(result[5] == approx(0.6));

    // in-place interscet
    AABB yet_another_result = first;
    CHECK(yet_another_result.intersect(second) == result);

    // see if no intersection returns a still valid bbox
    AABB third(100.0, 200.0, 300.0, 400.0, 500.0, 600.0);
    third.intersect(first);
    CHECK(third.is_valid());
  }

}  // namespace
