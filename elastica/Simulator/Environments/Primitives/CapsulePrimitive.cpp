#include "CapsulePrimitive.hpp"

#include "elastica/Utilities/MakeSignalingNan.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CONSTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor for the CapsulePrimitive class.
     *
     * \param sid Unique system-specific ID for the sphere.
     * \param uid User-specific ID for the sphere.
     * \param gpos Global geometric center of the sphere.
     * \param radius The radius of the sphere \f$ (0..\infty) \f$.
     * \param material The material of the sphere.
     */
    CapsulePrimitive::CapsulePrimitive(id_t sid, id_t uid, bool finite,
                                         MaterialID material)
        : ApproximateCylinderPrimitive(finite, sid, uid, material) {}
    //**************************************************************************

    CapsulePrimitive::CapsulePrimitive(id_t sid, id_t uid, bool finite,
                                         const Vec3& gpos, real_t radius,
                                         real_t length, MaterialID material)
        : ApproximateCylinderPrimitive(sid, uid, finite, gpos, radius, length,
                                       material) {}

    CapsulePrimitive::CapsulePrimitive(CapsulePrimitive&& other) noexcept
        : ApproximateCylinderPrimitive(std::move(other)) {}

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the CapsulePrimitive class.
     */
    CapsulePrimitive::~CapsulePrimitive() noexcept = default;
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
