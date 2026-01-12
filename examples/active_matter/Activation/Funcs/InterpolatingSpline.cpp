//
// Created by Tejaswin Parthasarathy on 10/7/22.
//

#include "InterpolatingSpline.hpp"
//
#include "Utilities/Generators.hpp"
//
#include <algorithm>

namespace elastica {

  namespace forcing {

    namespace activation {

      auto get_interpolating_points(std::vector<real_t> spline_y)
          -> std::tuple<std::vector<real_t>, std::vector<real_t>> {
        auto const syze = std::size(spline_y);
        Expects(syze > 2UL);
        auto const nsyze = syze;

        auto gen = linspace_generator(real_t(0.0), real_t(1.0), nsyze + 1);

        std::vector<real_t> xs;
        xs.resize(nsyze);

        // Do not generate value at 0.0, but generate up to 1.0
        std::generate(xs.begin(), xs.end(),
                      [&gen, n = 1UL]() mutable { return gen(n++); });

        Ensures(spline_y.size() == xs.size());
        return std::make_tuple(std::move(xs), std::move(spline_y));
      }

    }  // namespace activation

  }  // namespace forcing

}  // namespace elastica