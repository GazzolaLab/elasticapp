#pragma once

#include <array>

#include "GeomPrimitive.hpp"
#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Math/Equal.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    // TODO Consider a specialization for an infinite plane primitive

    /*
     * The plane equation is: \f$ ax + by + cz = d \f$.\n
     * \a a, \a b and \a c are the x, y and z coordinate of the normal vector
     * and \a d is the distance from the origin to the plane.
     */

    class PlanePrimitive final : public GeomPrimitive {
     public:
      using Spans = std::array<real_t, 2UL>;

     public:
      explicit PlanePrimitive(id_t sid, id_t uid, bool finite,
                              MaterialID material);
      explicit PlanePrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                              const Rot3& rot, const Spans& spans,
                              MaterialID material);
      explicit PlanePrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                              const Vec3& normal, const Spans& spans,
                              MaterialID material);
      ~PlanePrimitive() noexcept final;
      PlanePrimitive(PlanePrimitive&&) noexcept;

     public:
      inline auto get_normal() const noexcept -> decltype(blaze::row<1UL>(rot_mat_));
      inline auto get_normal() noexcept -> decltype(blaze::row<1UL>(rot_mat_));
      //      inline real_t get_displacement() const noexcept;
      //      inline real_t& get_displacement() noexcept;

      void
      calculate_bounding_box();  // Calculation of the axis-aligned bounding box

     private:
      //**Member
      // variables****************************************************************************
      /*!\name Member variables */
      //@{
      //      Vec3 normal_;  //!< Normal of the plane in reference to the global
      //      world
      //                     //!< frame.
      //      /*!< The normal of the plane is always pointing towards the
      //      halfspace
      //           outside the plane. */
      //      real_t d_;  //!< Plane displacement from the origin.
      //      /*!< The displacement can be categorized in the following way:\n
      //            - > 0: The global origin is inside the plane\n
      //            - < 0: The global origin is outside the plane\n
      //            - = 0: The global origin is on the surface of the plane */
      //      Tangents tangents_;
      Spans spans_;  //!< Span of plane
      //@}
      //**********************************************************************************************
    };

    //=================================================================================================
    //
    //  GET FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Returns the normal of the plane in reference to the global world
     * frame.
     *
     * \return The normal of the plane.
     */
    inline auto PlanePrimitive::get_normal() const noexcept
        -> decltype(blaze::row<1UL>(rot_mat_)) {
      return blaze::row<1UL>(rot_mat_);
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Returns the normal of the plane in reference to the global world
     * frame.
     *
     * \return The normal of the plane.
     */
    inline auto PlanePrimitive::get_normal() noexcept
        -> decltype(blaze::row<1UL>(rot_mat_)) {
      return blaze::row<1UL>(rot_mat_);
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Returns the displacement/distance from the origin of the global
     * world frame.
     *
     * \return The displacement of the plane.
     *
     * A positive displacement value indicates that the origin of the global
     * world frame is contained in the plane, whereas a negative value indicates
     * that the origin is not contained in the plane.
     */
    //    inline real_t PlanePrimitive::get_displacement() const noexcept {
    //      return d_;
    //    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Returns the displacement/distance from the origin of the global
     * world frame.
     *
     * \return The displacement of the plane.
     *
     * A positive displacement value indicates that the origin of the global
     * world frame is contained in the plane, whereas a negative value indicates
     * that the origin is not contained in the plane.
     */
    //    inline real_t& PlanePrimitive::get_displacement() noexcept { return
    //    d_; }
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
