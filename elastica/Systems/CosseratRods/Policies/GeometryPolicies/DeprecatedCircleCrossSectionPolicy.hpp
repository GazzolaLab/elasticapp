#pragma once

#include <utility>

///// Types always first
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
/////
#include "Systems/CosseratRods/Policies/GeometryPolicies/DeprecatedRodGeometry.hpp"
#include "Systems/CosseratRods/Policies/GeometryPolicies/DeprecatedRodKinematics.hpp"
#include "Utilities/Requires.hpp"

namespace elastica {
  //  public RodKinematics<RodTraits>
  // Sparse_strategy
  // Dense strategy
  namespace detail_cosserat_rod {

    template <typename RodTraits>
    struct CircleCrossSectionPolicy : public RodGeometry<RodTraits> {
     protected:
      using Traits = RodTraits;
      using This = CircleCrossSectionPolicy<RodTraits>;
      using GeometryParent = RodGeometry<RodTraits>;
      //      using KinematicsParent = RodKinematics<RodTraits>;
      using Volumes = typename Traits::ElementVector;
      using Dimensions = typename Traits::ElementVector;
      using Inertias = typename Traits::ElementMatrix;

      // Store only off-diagonals elements in J and inv_J
      //      using StorageStrategy = typename RodTraits::

      // Memebrs
      // radius collection
      // inertia collection
      // lrngth
     protected:
      inline constexpr auto get_dimension() noexcept -> Dimensions& {
        return radii_;
      }

      inline constexpr auto get_dimension(std::size_t i) const noexcept
          -> real_t {
        return radii_[i];
      }

     private:
      template <typename Radius>
      inline static constexpr auto area_impl(Radius& r) noexcept {
        return M_PI * r * r;
      }

      template <typename Radius>
      inline static constexpr auto second_moment_of_area_impl(
          Radius& r) noexcept {
        return area_impl(r) * area_impl(r) * (0.25 * M_1_PI);
      }

     protected:
      inline constexpr auto get_area(std::size_t i) const noexcept -> real_t {
        return This::get_area_impl(radii_[i]);
      }

      inline constexpr auto get_area() const noexcept {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return This::get_area_impl(radii_);
      }

      template <unsigned char Dim, Requires<(Dim < This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area(
          std::size_t i) const noexcept -> real_t {
        return This::second_moment_of_area_impl(radii_[i]);
      }

      template <unsigned char Dim, Requires<(Dim == This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area(
          std::size_t i) const noexcept -> real_t {
        // Peprendicular axis theorem
        return real_t(2.0) * This::second_moment_of_area_impl(radii_[i]);
      }

      template <unsigned char Dim, Requires<(Dim < This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area() const noexcept {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return This::second_moment_of_area_impl(radii_);
      }

      template <unsigned char Dim, Requires<(Dim == This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area() const noexcept {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return real_t(2.0) * This::second_moment_of_area_impl(radii_);
      }

      // Bad design : which comes first?
      template <typename PosGen, typename DirGen, typename RestLengthGen,
                typename RestCurvatureGen, typename RadiusGen>
      CircleCrossSectionPolicy(
          std::size_t n_elems, PositionProfile<PosGen>&& position_profile,
          DirectorProfile<DirGen>&& director_profile,
          RestLengthProfile<RestLengthGen>&& rest_length_profile,
          RestCurvatureProfile<RestCurvatureGen>&& rest_curvature_profile,
          RadiusProfile<RadiusGen>&& radius_profile)
          : GeometryParent(
                n_elems,
                std::forward<PositionProfile<PosGen>>(position_profile),
                std::forward<DirectorProfile<DirGen>>(director_profile),
                std::forward<RestLengthProfile<RestLengthGen>>(
                    rest_length_profile),
                std::forward<RestCurvatureProfile<RestCurvatureGen>>(
                    rest_curvature_profile)),
            radii_(n_elems),
            volumes_(n_elems) {
        {
          RadiusGen& f = radius_profile.get();
          for (std::size_t i = 0; i < n_elems; ++i) {
            radii_[i] = f(i);
          }
        }

        volumes_ = get_area() * GeometryParent::lengths_;

        // J = rho I ds
        /*
        {
          DP& f = density_profile.get();
          for (std::size_t i = 0UL; i < n_elems; ++i)
            rest_J_[i] = (0.25 / M_PI) * density_profile(i) * cs_area[i] *
                         cs_area[i] * GeometryParent::lengths_[i];
        }
         */
      }

     protected:
      // Some static typedefs for ease of use in the Cosserat Rod class
      static constexpr unsigned char D1 = 0;
      static constexpr unsigned char D2 = 1;
      static constexpr unsigned char D3 = 2;

     private:
      Dimensions radii_;
      Volumes volumes_;
      //      Inertias rest_J_;

      //    rest_J_(3UL, n_elems, real_t(0.0)) {
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
