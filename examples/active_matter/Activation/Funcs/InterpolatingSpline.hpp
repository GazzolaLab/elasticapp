//
// Created by Tejaswin Parthasarathy on 9/30/22.
//

#pragma once

#include "ErrorHandling/ExpectsAndEnsures.hpp"
//
#include "Utilities/DefineTypes.h"
//
#include <cmath>
#include <utility>  // move
#include <vector>
#include <tuple>
//
#include <spline.h>  // external

namespace elastica {

  namespace forcing {

    namespace activation {

      auto get_interpolating_points(std::vector<real_t> spline_y)
          -> std::tuple<std::vector<real_t>, std::vector<real_t>>;

      template <typename... SplineArgs>
      tk::spline generate_spline(std::vector<real_t> spline_x,
                                 std::vector<real_t> spline_y,
                                 SplineArgs&&... spline_args) {
        Expects(std::size(spline_x) > 2UL);
        Expects(std::size(spline_y) > 2UL);
        Expects(spline_x[0UL] > real_t(0.0));
        Expects(spline_x[std::size(spline_x) - 1UL] <= real_t(1.0));

        // Second order derivative boundary condition. Okay for now.
        return tk::spline(std::move(spline_x), std::move(spline_y),
                          std::forward<SplineArgs>(spline_args)...);
      }

      template <typename... SplineArgs>
      tk::spline generate_spline(std::vector<real_t> spline_y,
                                 SplineArgs&&... spline_args) {
        auto vals = get_interpolating_points(std::move(spline_y));
        return generate_spline(std::move(std::get<0UL>(vals)),
                               std::move(std::get<1UL>(vals)),
                               std::forward<SplineArgs>(spline_args)...);
      }

      struct InterpolatingSpline {
        // If data-sparse, use linear, else use not a knot
        explicit InterpolatingSpline(std::vector<real_t> spline_x,
                                     std::vector<real_t> spline_y)
            : interp_(generate_spline(std::move(spline_x), std::move(spline_y),
                                      tk::spline::cspline,  // cubic non-hermite
                                      false,                // monotonic
                                      tk::spline::not_a_knot,  // left bc
                                      0.0,                     // left value
                                      tk::spline::not_a_knot,  // right bc
                                      0.0                      // right value
                                      )) {}

        template <typename... SplineArgs>
        explicit InterpolatingSpline(std::vector<real_t> spline_y,
                                     SplineArgs&&... spline_args)
            : interp_(
                  generate_spline(std::move(spline_y),
                                  std::forward<SplineArgs>(spline_args)...)) {}

        template <typename... SplineArgs>
        InterpolatingSpline(std::vector<real_t> spline_x,
                            std::vector<real_t> spline_y,
                            SplineArgs&&... spline_args)
            : interp_(
                  generate_spline(std::move(spline_x), std::move(spline_y),
                                  std::forward<SplineArgs>(spline_args)...)) {}

        inline real_t operator()(real_t s) const { return interp_(s); }

        tk::spline interp_;
      };

    }  // namespace activation

  }  // namespace forcing

}  // namespace elastica
