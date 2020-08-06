#pragma once

#include <vector>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/BoundingBox.hpp"
#include "elastica/Simulator/Trigger.hpp"
#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  namespace rigidbody {
    class Sphere;
  }

  class Visualization : public Trigger {
   protected:
    //**Type definitions********************************************************
    using Viewer =
        std::vector<Visualization*>;  //!< Container for active viewers.
    using Spheres = std::vector<rigidbody::Sphere const*>;  //!< Container for
                                                            //!< active spheres.
    using AABB = ::elastica::collision::detection::coarse::BoundingBox<real_t>;
    using BoundingBoxes =
        std::vector<AABB const*>;  //!< Container for active spheres.

   public:
    //**Registration functions**************************************************
    /*!\name Registration functions */
    //@{
    static void add(rigidbody::Sphere const* sphere);
    static void add(AABB const* bounding_box);
    //@}
    //**************************************************************************

    //**Deregistration functions************************************************
    /*!\name Deregistration functions */
    //@{
    static void remove(rigidbody::Sphere const* sphere);
    static void remove(AABB const* bounding_box);
    //@}

   protected:
    //**Constructor*************************************************************
    /*!\name Constructor */
    //@{
    explicit Visualization();
    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    virtual ~Visualization();
    //@}
    //**************************************************************************

    //**Add functions***********************************************************
    /*!\name Add functions */
    //@{
    virtual void add_sphere(rigidbody::Sphere const* sphere) = 0;
    virtual void add_bounding_box(AABB const* bounding_box) = 0;
    //@}

    //**Remove functions********************************************************
    /*!\name Remove functions */
    //@{
    virtual void remove_sphere(rigidbody::Sphere const* sphere) = 0;
    virtual void remove_bounding_box(AABB const* bounding_box) = 0;
    //@}
    //@}

    //**Trigger functions*******************************************************
    /*!\name Trigger functions */
    //@{
    void trigger(double time_v, std::size_t simulation_step) override = 0;
    //@}
    //**************************************************************************
   protected:
    //**Member
    // variables*********************************************************
    /*!\name Member variables */
    //@{
    //!< The currently active viewers/visualization objects.
    static Viewer viewer_;
    //!< The registered visible spheres.
    static Spheres spheres_;
    //!< The registered visible boxes.
    static BoundingBoxes bounding_boxes_;
    //@}
  };

}  // namespace elastica
