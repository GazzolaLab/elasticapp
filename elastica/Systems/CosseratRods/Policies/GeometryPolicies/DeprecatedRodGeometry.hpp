#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/Policies/GeometryPolicies/Types.hpp"
#include "Utilities/DefineTypes.h"

// smells, why here and not a static function?
#include <blaze/math/Subvector.h>

namespace elastica {

  namespace detail_cosserat_rod {

    template <typename RodTraits>
    struct RodGeometryBase {
     protected:
      using Self = RodGeometryBase<RodTraits>;
      using Traits = RodTraits;
      using Positions = typename Traits::NodeMatrix;
      using Directors = typename Traits::ElementTensor;

      // Should take in a polymorphic allocator as argument and
      // only allocate memory from that resource
      template <typename PosGen, typename DirGen>
      RodGeometryBase(std::size_t n_elems,
                      PositionProfile<PosGen>&& position_profile,
                      DirectorProfile<DirGen>&& director_profile)
          : positions_(Traits::dimensions(), Traits::n_nodes(n_elems)),
            directors_(Traits::dimensions(), Traits::dimensions(), n_elems) {
        {
          auto& f = position_profile.get();
          for (std::size_t i = 0; i < Traits::n_nodes(n_elems); ++i) {
            positions_[i] = f(i);
          }
        }

        {
          auto& f = director_profile.get();
          for (std::size_t i = 0; i < n_elems; ++i) {
            directors_[i] = f(i);
          }
        }
      }

     protected:
      // TODO : Think about aligning them together
      Positions positions_;
      Directors directors_;
    };

    // Uniform length constructor
    // Variable length constructor
    template <typename RodTraits>
    struct RodGeometry : RodGeometryBase<RodTraits> {
     protected:
      using Parent = RodGeometryBase<RodTraits>;
      using Self = RodGeometry<RodTraits>;
      using Traits = RodTraits;
      using ElementLengths = typename Traits::ElementVector;
      using VoronoiLengths = typename Traits::VoronoiVector;
      using Curvatures = typename Traits::VoronoiMatrix;
      using Tangents = typename Traits::ElementMatrix;

     protected:
      struct UniformInitializer {
        real_t operator()(std::size_t /*i*/) const noexcept {
          return l_ / static_cast<real_t>(n_);
        }
        real_t l_;
        std::size_t n_;
      };

      /*
      RodGeometry(std::size_t n_elems, real_t length) {
        rest_lengths_.resize(n_elems);
        real_t dl(length / real_t(n_elems));
        std::fill(rest_lengths_.begin(), rest_lengths_.end(), dl);

        lengths_.resize(n_elems);
        std::copy(rest_lengths_.begin(), rest_lengths_.end(), lengths_.begin());
      }
      */

      // At the call site, initialize as so:
      //      UniformInitializer{length_, n_elems};
      template <typename PosGen, typename DirGen,
                typename RestLengthGen = EmptyFunc,
                typename RestCurvatureGen = EmptyFunc>
      RodGeometry(
          std::size_t n_elems, PositionProfile<PosGen>&& position_profile,
          DirectorProfile<DirGen>&& director_profile,
          RestLengthProfile<RestLengthGen>&& rest_length_profile =
              RestLengthProfile<RestLengthGen>{RestLengthGen()},
          RestCurvatureProfile<RestCurvatureGen>&& rest_curvature_profile =
              RestCurvatureProfile<RestCurvatureGen>{RestCurvatureGen()})
          : Parent(n_elems,
                   std::forward<PositionProfile<PosGen>>(position_profile),
                   std::forward<DirectorProfile<DirGen>>(director_profile)),
            reference_lengths_(n_elems),
            reference_voronoi_lengths_(Traits::n_voronoi(n_elems)),
            reference_curvature_(Traits::dimensions(),
                                 Traits::n_voronoi(n_elems)),
            lengths_(n_elems),
            dilatations_(n_elems),
            voronoi_lengths_(Traits::n_voronoi(n_elems)),
            voronoi_dilatations_(Traits::n_voronoi(n_elems)),
            curvature_(Traits::dimensions(), Traits::n_voronoi(n_elems)),
            tangents_(Traits::dimensions(), n_elems) {
        ////////////////////////////////////////////////////////////////////////
        /// LENGTH INITIALIZATION
        ////////////////////////////////////////////////////////////////////////
        {
          RestLengthGen& f = rest_length_profile.get();
          for (std::size_t i = 0; i < n_elems; ++i) {
            reference_lengths_[i] = f(i);
          }
        }

        // Calculate lengths from positions
        //        std::copy(rest_lengths_.begin(), rest_lengths_.end(),
        //        lengths_.begin());

        // Dilatation
        dilatations_ = lengths_ / reference_lengths_;

        reference_voronoi_lengths_ =
            calculate_voronoi_length(reference_lengths_);

        // Calculate voronoi lengths from lengths_
        voronoi_lengths_ = calculate_voronoi_length(lengths_);

        voronoi_dilatations_ = voronoi_lengths_ / reference_voronoi_lengths_;

        ////////////////////////////////////////////////////////////////////////
        /// CURVATURE INITIALIZATION
        ////////////////////////////////////////////////////////////////////////
        // TODO : SFINAE for function of length or index...
        {
          RestCurvatureGen& f = rest_curvature_profile.get();
          for (std::size_t i = 0; i < n_elems; ++i) {
            reference_curvature_[i] = f(i);
          }
        }

        // With the lengths we can calculate curvature
        // TODO

        ////////////////////////////////////////////////////////////////////////
        /// TANGENT INITIALIZATION
        ////////////////////////////////////////////////////////////////////////
        // TODO
      }

      inline auto calculate_voronoi_length(ElementLengths const& phys) {
        // convolve with weights 0.5, 0.5
        auto n_voronoi(Traits::n_voronoi(phys.size()));
        return 0.5 * (blaze::subvector(phys, 1UL, n_voronoi) +
                      blaze::subvector(phys, 0UL, n_voronoi));
      }

     protected:
      // Reference configuration
      ElementLengths reference_lengths_;
      VoronoiLengths reference_voronoi_lengths_;
      Curvatures reference_curvature_;

      // Current configuration
      ElementLengths lengths_, dilatations_;
      VoronoiLengths voronoi_lengths_, voronoi_dilatations_;
      Curvatures curvature_;
      Tangents tangents_;
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica