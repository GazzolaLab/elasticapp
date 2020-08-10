#include "Visualization.hpp"

namespace elastica {

  //=================================================================================================
  //
  //  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
  //
  //=================================================================================================
  Visualization::Viewer Visualization::viewer_;
  Visualization::Spheres Visualization::spheres_;
  Visualization::BoundingBoxes Visualization::bounding_boxes_;

  //=================================================================================================
  //
  //  CONSTRUCTOR
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Constructor for the Visualization class.
   */
  Visualization::Visualization()
      : Trigger()  // Initialization of the Trigger base class
  {
    viewer_.push_back(this);
  }
  //*************************************************************************************************

  //=================================================================================================
  //
  //  DESTRUCTOR
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Destructor for the Visualization class.
   */
  Visualization::~Visualization() {
    auto pos(std::find(viewer_.begin(), viewer_.end(), this));
    ELASTICA_ASSERT(pos != viewer_.end(),
                    "Visualization object is not registered!");
    viewer_.erase(pos);
  }
  //*************************************************************************************************

  //=================================================================================================
  //
  //  REGISTRATION FUNCTIONS
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Registration function for a sphere.
   *
   * \param sphere The sphere to be registered for visualization.
   * \return void
   */
  void Visualization::add(SphereCollection const* const sphere) {
    spheres_.push_back(sphere);

    for (auto v : viewer_) {
      v->add_sphere(sphere);
    }
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Registration function for a bounding box.
   *
   * \param bounding_box The bounding box to be registered for visualization.
   * \return void
   */
  void Visualization::add(AABB const* const bounding_box) {
    bounding_boxes_.push_back(bounding_box);

    for (auto v : viewer_) {
      v->add_bounding_box(bounding_box);
    }
  }
  //*************************************************************************************************

  //=================================================================================================
  //
  //  DEREGISTRATION FUNCTIONS
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Deregistration function for a sphere.
   *
   * \param sphere The sphere to be deregistered from visualization.
   * \return void
   */
  void Visualization::remove(SphereCollection const* const sphere) {
    auto pos(std::find(spheres_.begin(), spheres_.end(), sphere));
    ELASTICA_ASSERT(pos != spheres_.end(),
                    "Sphere is not registered for visualization");
    spheres_.erase(pos);

    for (auto v : viewer_) {
      v->remove_sphere(sphere);
    }
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Deregistration function for a bounding box.
   *
   * \param bounding_box The bounding box to be deregistered from visualization.
   * \return void
   */
  void Visualization::remove(AABB const* const bounding_box) {
    auto pos(std::find(bounding_boxes_.begin(), bounding_boxes_.end(),
                       bounding_box));
    ELASTICA_ASSERT(pos != bounding_boxes_.end(),
                    "Bounding box is not registered for visualization");
    bounding_boxes_.erase(pos);

    for (auto v : viewer_) {
      v->remove_bounding_box(bounding_box);
    }
  }
  //*************************************************************************************************

}  // namespace elastica
