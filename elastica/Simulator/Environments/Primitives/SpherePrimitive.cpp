#include "SpherePrimitive.hpp"

#include "elastica/Utilities/MakeSignalingNan.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CONSTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor for the SpherePrimitive class.
     *
     * \param sid Unique system-specific ID for the sphere.
     * \param uid User-specific ID for the sphere.
     * \param gpos Global geometric center of the sphere.
     * \param radius The radius of the sphere \f$ (0..\infty) \f$.
     * \param material The material of the sphere.
     */
    SpherePrimitive::SpherePrimitive(id_t sid, id_t uid, bool finite,
                                     MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
#ifdef ELASTICA_DEBUG
          radius_(make_signaling_NaN<real_t>())  // Radius of the sphere
#else
          radius_(0.0)  // Radius of the sphere
#endif
    {
    }
    //**************************************************************************

    SpherePrimitive::SpherePrimitive(id_t sid, id_t uid, bool finite,
                                     const Vec3& gpos, real_t radius,
                                     MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
          //
          radius_(radius) {
      ELASTICA_ASSERT(radius > real_t(0), "Invalid sphere radius");

      // Setting the center of the sphere
      gpos_ = gpos;

      // Setting the axis-aligned bounding box
      SpherePrimitive::calculate_bounding_box();
    }

    SpherePrimitive::SpherePrimitive(SpherePrimitive&& other) noexcept
        : GeomPrimitive(std::move(other)),
#ifdef ELASTICA_DEBUG
          radius_(make_signaling_NaN<real_t>())  // Radius of the sphere
#else
          radius_(0.0)  // Radius of the sphere
#endif
    {
    }

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the SpherePrimitive class.
     */
    SpherePrimitive::~SpherePrimitive() noexcept {}
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
