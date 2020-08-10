#include "ApproximateCylinderPrimitive.hpp"

#include "elastica/Utilities/MakeSignalingNan.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CONSTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor for the ApproximateCylinderPrimitive class.
     *
     * \param sid Unique system-specific ID for the sphere.
     * \param uid User-specific ID for the sphere.
     * \param gpos Global geometric center of the sphere.
     * \param radius The radius of the sphere \f$ (0..\infty) \f$.
     * \param material The material of the sphere.
     */
    ApproximateCylinderPrimitive::ApproximateCylinderPrimitive(
        id_t sid, id_t uid, bool finite,
        MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
#ifdef ELASTICA_DEBUG
          radius_(make_signaling_NaN<real_t>()),  // Radius of the cylinder
          length_(make_signaling_NaN<real_t>())   // Length of the cylinder
#else
          radius_(0.0),  // Radius of the cylinder
          length_(0.0)   // Length of the cylinder
#endif
    {
    }
    //**************************************************************************

    ApproximateCylinderPrimitive::ApproximateCylinderPrimitive(
        id_t sid, id_t uid, bool finite, const Vec3& gpos, real_t radius,
        real_t length,
        MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
          //
          radius_(radius),
          length_(length) {
      ELASTICA_ASSERT(radius > real_t(0), "Invalid cylinder radius");
      ELASTICA_ASSERT(length > real_t(0), "Invalid cylinder radius");

      // Setting the center of the cylinder
      gpos_ = gpos;

      // Setting the axis-aligned bounding box
      ApproximateCylinderPrimitive::calculate_bounding_box();
    }

    ApproximateCylinderPrimitive::ApproximateCylinderPrimitive(
        ApproximateCylinderPrimitive&& other) noexcept
        : GeomPrimitive(std::move(other)),
#ifdef ELASTICA_DEBUG
          radius_(make_signaling_NaN<real_t>()),  // Radius of the cylinder
          length_(make_signaling_NaN<real_t>())   // Length of the cylinder
#else
          radius_(0.0),  // Radius of the cylinder
          length_(0.0)   // Length of the cylinder
#endif
    {
    }

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the ApproximateCylinderPrimitive class.
     */
    ApproximateCylinderPrimitive::~ApproximateCylinderPrimitive() noexcept =
        default;
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
