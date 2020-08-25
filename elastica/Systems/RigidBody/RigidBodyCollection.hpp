#pragma once

#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/BoundingBox.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"
#include "elastica/Simulator/Materials.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/NonCopyable.hpp"

namespace elastica {

  class RigidBodyCollection : private NonCopyable {
   protected:
    using AABB = ::elastica::collision::detection::coarse::BoundingBox<real_t>;
    using AABBCollection = std::vector<AABB>;
    using MaterialIDs = std::vector<MaterialID>;
    using VectorCollection = blaze::DynamicVector<real_t, blaze::rowVector>;
    using MatrixCollection = blaze::DynamicMatrix<real_t, blaze::rowMajor>;

   protected:
    RigidBodyCollection(const VectorCollection& mc, const MatrixCollection& pc,
                        const MatrixCollection& vc);
    ~RigidBodyCollection();

    // Virtual functions are not ideal since dynamic dispatch is involved
    // hence we don't use this. Rather we have a protocol to check if
    // the object derives from RigidBodyCollection and provides a calculate
    // bounding box method...
    //    virtual inline void calculate_bounding_box() = 0;
    RigidBodyCollection(RigidBodyCollection&& other);

   public:
    inline std::size_t size() const noexcept { return mass_collection_.size(); }

    // Useful getter methods for visualization
    // TODO blaze::unchecked and blaze::checked for columns
    inline auto get_position(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return blaze::column(position_collection_, idx);
    }

    inline auto get_velocity(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return blaze::column(velocity_collection_, idx);
    }

    inline auto get_angular_velocity(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return blaze::column(angular_velocity_collection_, idx);
    }

    // Used in visualizations
    inline auto get_force(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return blaze::column(force_collection_, idx);
    }

    // Not tagged const for environments
    inline decltype(auto) get_force(std::size_t idx) {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return blaze::column(force_collection_, idx);
    }

    // Used in visualizations
    inline auto get_torque(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return blaze::column(torque_collection_, idx);
    }

    // Not tagged const for environments
    inline decltype(auto) get_torque(std::size_t idx) {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return blaze::column(torque_collection_, idx);
    }

    inline real_t get_mass(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return mass_collection_[idx];
    }

    inline MaterialID get_material(std::size_t idx) const {
      ELASTICA_ASSERT(idx < size(), "Not enough spheres");
      return materials_[idx];
    }

   public:
    VectorCollection mass_collection_;
    MatrixCollection position_collection_;
    MatrixCollection velocity_collection_;
    MatrixCollection angular_velocity_collection_;
    MatrixCollection force_collection_;
    // Temporarily have another buffer :(
    MatrixCollection acceleration_collection_;
    MatrixCollection angular_acceleration_collection_;
    MatrixCollection torque_collection_;
    AABBCollection AABB_collection_;
    MaterialIDs materials_;
  };

}  // namespace elastica