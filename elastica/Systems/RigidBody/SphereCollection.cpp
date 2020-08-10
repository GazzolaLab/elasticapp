#include "SphereCollection.hpp"

#include <utility>

#include "elastica/Simulator/Visualization.hpp"  // for registering the sphere with viz

namespace elastica {

  //============================================================================
  //
  //  CONSTRUCTORS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Constructor for the SphereCollection class.
   *
   * \param body The body ID of this rigid body.
   */
  SphereCollection::SphereCollection(const VectorCollection& mc,
                                     const MatrixCollection& pc,
                                     const MatrixCollection& vc,
                                     VectorCollection rc)
      : Parent(mc, pc, vc), radius_collection_(std::move(rc)) {
    //    SphereCollection::calculate_bounding_box();
    //    std::cout << "initalized a sphere from the const" << std::endl;
    // Registering the sphere for visualization
    // Use for emplace back
    elastica::Visualization::add(this);
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Move constructor for the SphereCollection class.
   *
   * \param body The body ID of this rigid body.
   */
  SphereCollection::SphereCollection(SphereCollection&& other) noexcept
      : Parent(std::move(other)),
        radius_collection_(std::move(other.radius_collection_)) {
    // Registering the sphere for visualization
    // Hacky, as the other constructor is immediately moved from
    // and the reference is lost?

    // This seems to be somewhat brittle, as we explicitly need
    // to remove the reference of ther other sphere
    // TODO : New trigger framework without these statics
    elastica::Visualization::remove(&other);
    elastica::Visualization::add(this);
  }
  //****************************************************************************

  //============================================================================
  //
  //  DESTRUCTOR
  //
  //============================================================================
  SphereCollection::~SphereCollection() = default;

}  // namespace elastica
