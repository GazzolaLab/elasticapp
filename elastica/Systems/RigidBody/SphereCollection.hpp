#pragma once

#include "RigidBodyCollection.hpp"

namespace elastica {

  class SphereCollection : public RigidBodyCollection {
    using Parent = RigidBodyCollection;
    using Parent::AABB_collection_;
    using Parent::MatrixCollection;
    using Parent::VectorCollection;

   public:
    SphereCollection(const VectorCollection& mc, const MatrixCollection& pc,
                     const MatrixCollection& vc, VectorCollection rc);
    SphereCollection(SphereCollection&& other) noexcept;
    ~SphereCollection();

    SphereCollection& operator=(SphereCollection&&) = delete;

    inline MatrixCollection& kinematic_states() { return position_collection_; }

    inline MatrixCollection& kinematic_rates(double /*time_v*/) {
      return velocity_collection_;
    }

    inline MatrixCollection const& kinematic_rates(double /*time_v*/) const {
      return velocity_collection_;
    }

    inline MatrixCollection& dynamic_states() { return velocity_collection_; }

    inline MatrixCollection& dynamic_rates(double /*time_v*/) {
      // To be constexprified
      for (std::size_t dim = 0UL; dim < 3UL; ++dim) {
        //          blaze::row(acceleration_collection_, dim) =
        //          blaze::row(force_collection_, dim) /
        //          blaze::expand<1UL>(mass_collection_);
        blaze::row(acceleration_collection_, dim) =
            blaze::row(force_collection_, dim) * (1.0 / mass_collection_);
      }
      //        acceleration_collection_ = force_collection_ *
      //        blaze::expand<3UL>(1.0 / mass_collection_);
      // reset
      force_collection_ = 0.0;
      return acceleration_collection_;
    }

    //      inline MatrixCollection const& dynamic_rates(double time_v) const
    //      {
    //        return force_collection_ / mass_collection_;
    //      }

    inline real_t get_radius(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return radius_collection_[idx];
    }

    /*
    inline void calculate_bounding_box() override {
      std::size_t n_bodies(AABB_collection_.size());
      for (std::size_t i_body = 0UL; i_body < n_bodies; ++i_body) {
        auto& aabb = AABB_collection_[i_body];
        const real_t aabb_length(radius_collection_[i_body]);
        const auto position = blaze::column(position_collection_, i_body);

        for (std::size_t dim = 0UL; dim < 3UL; ++dim) {
          aabb[dim] = position[dim] - aabb_length;
        }
        for (std::size_t dim = 0UL; dim < 3UL; ++dim) {
          aabb[3UL + dim] = position[dim] + aabb_length;
        }

        ELASTICA_ASSERT(aabb.isValid(), "Invalid bounding box detected");
        ELASTICA_ASSERT(aabb.contains(position[0], position[1], position[2]),
                        "Invalid bounding box detected");
      }
    };
    */

    VectorCollection radius_collection_;
  };

}  // namespace elastica
