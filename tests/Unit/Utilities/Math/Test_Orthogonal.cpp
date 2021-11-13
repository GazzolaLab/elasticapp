//
// Created by Tejaswin Parthasarathy on 9/23/21.
//

#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Orthogonal.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.Math.is_orthogonal", "[Unit][Utilities]") {
    using namespace elastica;

    SECTION("Test identity matrices") {
      Rot3 R(identity3());
      CHECK(is_orthogonal(R));
    }

    SECTION("Test 2D matrices") {
      Rot3 R{{static_cast<real_t>(0.6), static_cast<real_t>(0.8),
              static_cast<real_t>(0.0)},
             {static_cast<real_t>(0.8), static_cast<real_t>(-0.6),
              static_cast<real_t>(0.0)},
             {static_cast<real_t>(0.0), static_cast<real_t>(0.0),
              static_cast<real_t>(1.0)}};

      CHECK(is_orthogonal(R));

      Rot3 another_R{{static_cast<real_t>(0.0), static_cast<real_t>(0.8),
                      static_cast<real_t>(0.6)},
                     {static_cast<real_t>(0.0), static_cast<real_t>(-0.6),
                      static_cast<real_t>(0.8)},
                     {static_cast<real_t>(1.0), static_cast<real_t>(0.0),
                      static_cast<real_t>(0.0)}};
      CHECK(is_orthogonal(another_R));

      Rot3 yet_another_R{{static_cast<real_t>(0.8), static_cast<real_t>(0.0),
                          static_cast<real_t>(0.6)},
                         {static_cast<real_t>(0.6), static_cast<real_t>(0.0),
                          static_cast<real_t>(-0.8)},
                         {static_cast<real_t>(0.0), static_cast<real_t>(1.0),
                          static_cast<real_t>(0.0)}};
      CHECK(is_orthogonal(yet_another_R));
    }

    SECTION("Test 3D matrices") {
      // this should also be refactored into its own function
      auto generator = [](real_t deg_alpha, real_t deg_beta, real_t deg_gamma) {
        // should be using angles.hpp when its committed
        const real_t alpha = ::elastica::geometry::deg2rad(deg_alpha);
        const real_t beta = ::elastica::geometry::deg2rad(deg_beta);
        const real_t gamma = ::elastica::geometry::deg2rad(deg_gamma);

        const real_t ca = std::cos(alpha);
        const real_t sa = std::sin(alpha);

        const real_t cb = std::cos(beta);
        const real_t sb = std::sin(beta);

        const real_t cg = std::cos(gamma);
        const real_t sg = std::sin(gamma);

        return Rot3{
            {(ca * cb), (ca * sb * sg - sa * cg), (ca * sb * cg + sa * sg)},
            {(sa * cb), (sa * sb * sg + ca * cg), (sa * sb * cg - ca * sg)},
            {(-sb), (cb * sg), (cb * cg)}};
      };
      CHECK(is_orthogonal(generator(20.0, 45.0, 70.0)));
      CHECK(is_orthogonal(generator(80.0, 15.0, 90.0)));
      CHECK(is_orthogonal(generator(60.0, 30.0, 45.0)));
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.make_orthogonal_bases",
                     "[Unit][Utilities]") {
    using namespace elastica;
    SECTION("Works for common vectors") {
      auto R = make_orthogonal_bases_from_normal(Vec3{1.0, 0.0, 0.0});

      // ( normal | binormal | tangent)^T
      Vec3 predicted_normal = {1.0, 0.0, 0.0};
      Vec3 predicted_binormal = {0.0, 1.0, 0.0};
      Vec3 predicted_tangent = {0.0, 0.0, 1.0};

      CHECK(blaze::row<0UL>(R) == predicted_normal);
      CHECK_NOFAIL(blaze::row<1UL>(R) == predicted_binormal);
      CHECK_NOFAIL(blaze::row<2UL>(R) == predicted_tangent);

      CHECK(is_orthogonal(R));

      predicted_normal = {0.0, 0.0, 1.0};
      predicted_binormal = {1.0, 0.0, 0.0};
      predicted_tangent = {0.0, 1.0, 0.0};

      R = make_orthogonal_bases_from_binormal(Vec3{1.0, 0.0, 0.0});
      CHECK_NOFAIL(blaze::row<0UL>(R) == predicted_normal);
      CHECK(blaze::row<1UL>(R) == predicted_binormal);
      CHECK_NOFAIL(blaze::row<2UL>(R) == predicted_tangent);

      CHECK(is_orthogonal(R));

      predicted_normal = {0.0, 1.0, 0.0};
      predicted_binormal = {0.0, 0.0, 1.0};
      predicted_tangent = {1.0, 0.0, 0.0};

      R = make_orthogonal_bases_from_tangent(Vec3{1.0, 0.0, 0.0});
      CHECK_NOFAIL(blaze::row<0UL>(R) == predicted_normal);
      CHECK_NOFAIL(blaze::row<1UL>(R) == predicted_binormal);
      CHECK(blaze::row<2UL>(R) == predicted_tangent);

      CHECK(is_orthogonal(R));
    }

    SECTION("Works even if the vectors are not scaled") {
      auto R = make_orthogonal_bases_from_normal(5.0 * Vec3{1.0, 0.0, 0.0});

      // ( normal | binormal | tangent)^T
      Vec3 predicted_normal = {1.0, 0.0, 0.0};
      Vec3 predicted_binormal = {0.0, 1.0, 0.0};
      Vec3 predicted_tangent = {0.0, 0.0, 1.0};

      CHECK(blaze::row<0UL>(R) == predicted_normal);
      CHECK_NOFAIL(blaze::row<1UL>(R) == predicted_binormal);
      CHECK_NOFAIL(blaze::row<2UL>(R) == predicted_tangent);

      CHECK(is_orthogonal(R));
    }

    SECTION(
        "Works for the edge case where the vector passed in is the internal "
        "seed vector") {
      // edge case
      auto R = make_orthogonal_bases_from_normal(Vec3{0.8, 0.6, 0.0});

      // ( binormal | normal | tangent)^T
      Vec3 predicted_normal = {0.8, 0.6, 0.0};
      Vec3 predicted_binormal = {0.6, -0.8, 0.0};
      Vec3 predicted_tangent = {0.0, 0.0, 1.0};

      CHECK(blaze::row<0UL>(R) == predicted_normal);
      CHECK_NOFAIL(blaze::row<1UL>(R) == predicted_binormal);
      CHECK_NOFAIL(blaze::row<2UL>(R) == predicted_tangent);

      CHECK(is_orthogonal(R));

      predicted_normal = {0.0, 0.0, 1.0};
      predicted_binormal = {0.8, 0.6, 0.0};
      predicted_tangent = {0.6, -0.8, 0.0};

      R = make_orthogonal_bases_from_binormal(Vec3{0.8, 0.6, 0.0});
      CHECK_NOFAIL(blaze::row<0UL>(R) == predicted_normal);
      CHECK(blaze::row<1UL>(R) == predicted_binormal);
      CHECK_NOFAIL(blaze::row<2UL>(R) == predicted_tangent);

      CHECK(is_orthogonal(R));

      predicted_normal = {0.6, -0.8, 0.0};
      predicted_binormal = {0.0, 0.0, 1.0};
      predicted_tangent = {0.8, 0.6, 0.0};

      R = make_orthogonal_bases_from_tangent(Vec3{0.8, 0.6, 0.0});
      CHECK_NOFAIL(blaze::row<0UL>(R) == predicted_normal);
      CHECK_NOFAIL(blaze::row<1UL>(R) == predicted_binormal);
      CHECK(blaze::row<2UL>(R) == predicted_tangent);

      CHECK(is_orthogonal(R));
    }

    SECTION("Works for pairs of normals and tangents") {
      SECTION("Aligned") {
        auto R = make_orthogonal_bases_from_normal_and_tangent(
            Vec3{0.0, 1.0, 0.0}, Vec3{0.0, 0.0, 1.0});

        // ( normal | binormal | tangent)^T
        Vec3 const predicted_normal = {0.0, 1.0, 0.0};
        Vec3 const predicted_binormal = {-1.0, 0.0, 0.0};
        Vec3 const predicted_tangent = {0.0, 0.0, 1.0};

        CHECK(blaze::row<0UL>(R) == predicted_normal);
        // ensure that it follows the right hand rule
        CHECK(blaze::row<1UL>(R) == predicted_binormal);
        CHECK(blaze::row<2UL>(R) == predicted_tangent);

        CHECK(is_orthogonal(R));
      }
      SECTION("Not aligned with axes") {
        // ( normal | binormal | tangent)^T
        Vec3 const predicted_normal = {0.6, -0.8, 0.0};
        Vec3 const predicted_binormal = {0.0, 0.0, -1.0};
        Vec3 const predicted_tangent = {0.8, 0.6, 0.0};

        const Rot3 R = make_orthogonal_bases_from_normal_and_tangent(
            3 * Vec3{0.6, -0.8, 0.0}, 3 * Vec3{0.8, 0.6, 0.0});
        CHECK(blaze::row<0UL>(R) == predicted_normal);
        CHECK(blaze::row<1UL>(R) == predicted_binormal);
        CHECK(blaze::row<2UL>(R) == predicted_tangent);

        CHECK(is_orthogonal(R));
      }
      SECTION("Not orthogonal to one another") {
        Vec3 const predicted_normal = {0.6, -0.8, 0.0};
        Vec3 const predicted_tangent = {7.0 / 25.0, 24.0 / 25.0, 0.0};

        CHECK_THROWS_WITH(make_orthogonal_bases_from_normal_and_tangent(
                              predicted_normal, predicted_tangent),
                          Catch::Contains("non-orthogonal"));
      }
    }
  }

}  // namespace