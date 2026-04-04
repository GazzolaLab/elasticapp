//==============================================================================
/*!
//  \file
//  \brief A plane primitive
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <array>

#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "Simulator/Environments/Primitives/Protocols.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Equal.hpp"
#include "Utilities/Math/RotationConvention.hpp"
#include "Utilities/Math/SqrLength.hpp"
#include "Utilities/Math/Trans.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace collision {

    // TODO Consider a specialization for an infinite plane primitive

    /*
     * The plane equation is: \f$ ax + by + cz = d \f$.\n
     * \a a, \a b and \a c are the x, y and z coordinate of the normal vector
     * and \a d is the distance from the origin to the plane.
     */

    //**************************************************************************
    /*!\brief Plane geometrical primitive.
     * \ingroup primitives
     *
     * \details
     * The PlanePrimitive class represents the geometric primitive plane, which
     * is one of the basic geometric primitives of \elastica.
     *
     * The plane geometry in \elastica is still a finite rigid body spanning a
     * 2D subspace. Hence its constructed with a spans---its two lengths in the
     * subspace axis, first along the tangent direction (third row in the
     * rotation matrix, and the second along the binormal direction (first row
     * in the rotation matrix).
     */
    class PlanePrimitive /*final*/
        : public GeomPrimitive /* is not a convex primitive*/ {
     public:
      //**Type definitions******************************************************
      using Spans = std::array<real_t, 2UL>;
      //************************************************************************

     public:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      explicit PlanePrimitive(id_t sid, id_t uid, bool finite,
                              MaterialID material);
      explicit PlanePrimitive(id_t sid, id_t uid, const Vec3& gpos,
                              const Vec3& normal, MaterialID material);
      explicit PlanePrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                              const Vec3& normal, const Spans& spans,
                              MaterialID material);
      explicit PlanePrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                              const Rot3& rot, const Spans& spans,
                              MaterialID material);
      //! Move constructor (defaulted).
      PlanePrimitive(PlanePrimitive&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~PlanePrimitive() noexcept /*final*/;
      //@}
      //************************************************************************

     public:
      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{
      inline decltype(auto) get_normal() const noexcept;
      //          ->
      //          decltype(trans(blaze::row<RotationConvention::Normal>(Q_)));
      // inline decltype(auto) get_normal() noexcept;
      //@}
      //************************************************************************

     public:
      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
      //!< Calculation of the axis-aligned bounding box
      void calculate_bounding_box();
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //!< Span of plane
      Spans spans_{};
      //@}
      //************************************************************************
    };

    //==========================================================================
    //
    //  GET FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Returns the normal of the plane in reference to the global world
     * frame.
     *
     * \return The normal of the plane.
     */
    inline decltype(auto) PlanePrimitive::get_normal() const noexcept {
      return trans(blaze::row<RotationConvention::Normal>(Q_));
    }
    //**************************************************************************

    //    //**************************************************************************
    //    /*!\brief Returns the normal of the plane in reference to the global
    //    world
    //     * frame.
    //     *
    //     * \return The normal of the plane.
    //     */
    //    inline decltype(auto) PlanePrimitive::get_normal() noexcept {
    //      return trans(blaze::row<1UL>(Q_));
    //    }
    //    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
