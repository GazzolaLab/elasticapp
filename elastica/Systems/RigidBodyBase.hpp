#pragma once

#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>
#include <utility>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/BoundingBox.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"
#include "elastica/Simulator/Materials.hpp"
#include "elastica/Simulator/Visualization.hpp"  // for registering the sphere with viz
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/NonCopyable.hpp"

namespace elastica {

  namespace rigidbody {

    class RigidBodyBase : private NonCopyable {
     protected:
      using AABB =
          ::elastica::collision::detection::coarse::BoundingBox<real_t>;
      using AABBCollection = std::vector<AABB>;
      using MaterialIDs = std::vector<MaterialID>;
      using VectorCollection = blaze::DynamicVector<real_t>;
      using MatrixCollection = blaze::DynamicMatrix<real_t>;

     protected:
      RigidBodyBase(const VectorCollection& mc, const MatrixCollection& pc,
                    const MatrixCollection& vc)
          : mass_collection_(mc),
            position_collection_(pc),
            velocity_collection_(vc),
            angular_velocity_collection_(pc.rows(), pc.columns(),
                                         static_cast<real_t>(0.0)),
            force_collection_(pc.rows(), pc.columns(),
                              static_cast<real_t>(0.0)),
            acceleration_collection_(pc.rows(), pc.columns(),
                                     static_cast<real_t>(0.0)),
            angular_acceleration_collection_(pc.rows(), pc.columns(),
                                             static_cast<real_t>(0.0)),
            torque_collection_(pc.rows(), pc.columns(), real_t(0.0)),
            AABB_collection_(mc.size()),
            materials_(mc.size(), iron) {
        ELASTICA_ASSERT(mc.size() == pc.columns(),
                        "Unequal sizes to masses and positions");
        ELASTICA_ASSERT(mc.size() == vc.columns(),
                        "Unequal sizes to masses and forces");
        //        AABB_collection_.resize(mc.size());
        //        std::size_t n_bodies(mc_.size());
        //        for (std::size_t i_body = 0UL; i_body < n_bodies; ++i_body){
        //          AABB_collection_.emplace_back();
        //        }
      };
      virtual ~RigidBodyBase() = default;
      virtual inline void calculate_bounding_box() = 0;

      RigidBodyBase(RigidBodyBase&& other)
          : mass_collection_(std::move(other.mass_collection_)),
            position_collection_(std::move(other.position_collection_)),
            velocity_collection_(std::move(other.velocity_collection_)),
            angular_velocity_collection_(
                std::move(other.angular_velocity_collection_)),
            force_collection_(std::move(other.force_collection_)),
            acceleration_collection_(std::move(other.acceleration_collection_)),
            angular_acceleration_collection_(
                std::move(other.angular_acceleration_collection_)),
            torque_collection_(std::move(other.torque_collection_)),
            AABB_collection_(std::move(other.AABB_collection_)),
            materials_(std::move(other.materials_)) {
        // Fill other's stuff
      }

     public:
      inline std::size_t size() const noexcept {
        return mass_collection_.size();
      }

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

    class Sphere : public RigidBodyBase {
      using Parent = RigidBodyBase;
      using Parent::AABB_collection_;
      using Parent::MatrixCollection;
      using Parent::VectorCollection;

     public:
      using PrimitiveType = ::elastica::collision::SpherePrimitive;

      Sphere(const VectorCollection& mc, const MatrixCollection& pc,
             const MatrixCollection& vc, VectorCollection rc)
          : Parent(mc, pc, vc), radius_collection_(std::move(rc)) {
        Sphere::calculate_bounding_box();
        std::printf("initalized a sphere from the const");
        // Registering the sphere for visualization
        //        elastica::Visualization::add(this);
      };

      Sphere(Sphere&& other)
          : RigidBodyBase(std::move(other)),
            radius_collection_(std::move(other.radius_collection_)) {
        // Registering the sphere for visualization
        // Hacky, as the other constructor is immediately moved from
        // and the reference is lost?
        elastica::Visualization::add(this);
      }

      Sphere& operator=(Sphere&&) = delete;

      inline MatrixCollection& kinematic_states() {
        return position_collection_;
      }

      inline MatrixCollection& kinematic_rates(double time_v) {
        return velocity_collection_;
      }

      inline MatrixCollection const& kinematic_rates(double time_v) const {
        return velocity_collection_;
      }

      inline MatrixCollection& dynamic_states() { return velocity_collection_; }

      inline MatrixCollection& dynamic_rates(double time_v) {
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
      VectorCollection radius_collection_;
    };

  }  // namespace rigidbody

  using rigidbody::RigidBodyBase;
}  // namespace elastica