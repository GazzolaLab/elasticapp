#include "CylinderPrimitive.hpp"

#include "elastica/Utilities/MakeSignalingNan.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CONSTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor for the CylinderPrimitive class.
     *
     * \param sid Unique system-specific ID for the sphere.
     * \param uid User-specific ID for the sphere.
     * \param gpos Global geometric center of the sphere.
     * \param radius The radius of the sphere \f$ (0..\infty) \f$.
     * \param material The material of the sphere.
     */
    CylinderPrimitive::CylinderPrimitive(id_t sid, id_t uid, bool finite,
                                         MaterialID material)
        : ApproximateCylinderPrimitive(finite, sid, uid, material) {}
    //**************************************************************************

    CylinderPrimitive::CylinderPrimitive(id_t sid, id_t uid, bool finite,
                                         const Vec3& gpos, real_t radius,
                                         real_t length, MaterialID material)
        : ApproximateCylinderPrimitive(sid, uid, finite, gpos, radius, length,
                                       material) {}

    CylinderPrimitive::CylinderPrimitive(CylinderPrimitive&& other) noexcept
        : ApproximateCylinderPrimitive(std::move(other)) {}

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the CylinderPrimitive class.
     */
    CylinderPrimitive::~CylinderPrimitive() noexcept = default;
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
