#include "RigidBodyCollection.hpp"

#include <utility>

namespace elastica {

  //============================================================================
  //
  //  CONSTRUCTORS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Constructor for the RigidBodyCollection class.
   *
   * \param body The body ID of this rigid body.
   */
  RigidBodyCollection::RigidBodyCollection(const VectorCollection& mc,
                                           const MatrixCollection& pc,
                                           const MatrixCollection& vc)
      : mass_collection_(mc),
        position_collection_(pc),
        velocity_collection_(vc),
        angular_velocity_collection_(pc.rows(), pc.columns(), real_t(0.0)),
        force_collection_(pc.rows(), pc.columns(), real_t(0.0)),
        acceleration_collection_(pc.rows(), pc.columns(), real_t(0.0)),
        angular_acceleration_collection_(pc.rows(), pc.columns(), real_t(0.0)),
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
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Move constructor for the RigidBodyCollection class.
   *
   * \param body The body ID of this rigid body.
   */
  RigidBodyCollection::RigidBodyCollection(RigidBodyCollection&& other)
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
  //****************************************************************************

  RigidBodyCollection::~RigidBodyCollection() = default;

}  // namespace elastica
