//==============================================================================
/*!
//  \file
//  \brief A half-space primitive
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

    //**************************************************************************
    /*!\brief HalfSpace geometrical primitive.
     * \ingroup primitives
     *
     * \details
     * The HalfSpacePrimitive class represents a geometric primitive plane which
     * divides a space into two halves. For more information on the definition
     * consult https://en.wikipedia.org/wiki/Half-space_(geometry)
     * is one of the basic geometric primitives of \
     *
     * The half-space geometry in \elastica is an infinite rigid body spanning a
     * 3D affine space. Hence the user does not provide it with any span---it is
     * automatically considered to be infinite. The half-space does not allow
     * any bodies to enter into its occupied region (i.e. the region of -n, n
     * being the normal to the plane defining the half-space)
     */
    class HalfSpacePrimitive   /*final*/
        : public GeomPrimitive /* is not a convex primitive according to
                                  collision system*/
    {
     public:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      explicit HalfSpacePrimitive(id_t sid, id_t uid, bool finite,
                                  MaterialID material);
      explicit HalfSpacePrimitive(id_t sid, id_t uid, const Vec3& anchor_pos,
                                  const Vec3& normal, MaterialID material);
      explicit HalfSpacePrimitive(id_t sid, id_t uid, bool finite,
                                  const Vec3& anchor_pos, const Rot3& rot,
                                  MaterialID material);
      //! Move constructor (defaulted).
      HalfSpacePrimitive(HalfSpacePrimitive&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~HalfSpacePrimitive() noexcept /*final*/;
      //@}
      //************************************************************************

     public:
      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{
      inline decltype(auto) get_normal() const noexcept;
      static real_t get_span() noexcept;

      // TODO : https://github.com/tp5uiuc/elasticapp/issues/423
      // Come up with a better way/convention : this is somewhat arbitrary
      //************************************************************************
      /*!\brief Displaces an anchor to COM positively
       */
      template <typename F>
      static auto anchor_to_com_p(F f) noexcept {
        return [f = std::move(f)](real_t t) -> real_t {
          return f(t) + 0.5 * get_span();
        };
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Displaces an anchor to COM negatively
       */
      template <typename F>
      static auto anchor_to_com_n(F f) noexcept {
        return [f = std::move(f)](real_t t) -> real_t {
          return f(t) - 0.5 * get_span();
        };
      }
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
    };

    //==========================================================================
    //
    //  GET FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Returns the normal of the half-space plane in reference to the
     * global world frame.
     *
     * \return The normal of the plane.
     */
    inline decltype(auto) HalfSpacePrimitive::get_normal() const noexcept {
      return trans(blaze::row<RotationConvention::Normal>(Q_));
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
