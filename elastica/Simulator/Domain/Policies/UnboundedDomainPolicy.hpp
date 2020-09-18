//==============================================================================
/*!
//  \file
//  \brief Unbounded domain policy class
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <array>

#include "Simulator/Domain/Policies/Directions.hpp"
#include "Simulator/Domain/Span.hpp"
#include "Simulator/Domain/Types.hpp"
#include "Utilities/MakeSignalingNan.hpp"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/Requires.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  /*!\brief Class template implementing a unbounded domain policy
  // \ingroup domain
  //
  // The UnboundedDomainPolicy class template is a policy used by Elastica++'s
  // Domain class. It implements the logic for implementing unbounded domains
  // in the context of Elastica++ simulations.
  */
  template <typename Real>  // Any real type
  class UnboundedDomainPolicy : public NonCopyable {
   public:
    //**Type definitions********************************************************
    using SpanType = Span<Real>;  //!< The type of spans in each direction

    //! The storage type of all spans
    using SpanStorage = std::array<SpanType, 3UL>;

    //! The type of direction ID
    using DirectionType = typename detail::Directions::DirectionType;
    //**************************************************************************

    //**Static members**********************************************************
    //! unique ID for X direction
    static constexpr DirectionType X = detail::Directions::X;

    //! unique ID for Y direction
    static constexpr DirectionType Y = detail::Directions::Y;

    //! unique ID for Z direction
    static constexpr DirectionType Z = detail::Directions::Z;

    //! Dimension of simulations
    static constexpr auto Dimension = detail::Directions::Dimension;
    //**************************************************************************

   protected:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    UnboundedDomainPolicy() = default;
    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~UnboundedDomainPolicy() = default;
    //@}
    //**************************************************************************

   public:
    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation. Unimplemented.
    //
    // \param x_span The span in the X direction
    // \param y_span The span in the Y direction
    // \param z_span The span in the Z direction
    //
    // \return void
    */
    void set(SpanType&&, SpanType&&, SpanType&&) const noexcept {}
    //**************************************************************************

    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation. Unimplemented.
    //
    // \param min_x The minimum coordinate in the X direction
    // \param max_x The maximum coordinate in the X direction
    // \param min_y The minimum coordinate in the Y direction
    // \param max_y The maximum coordinate in the Y direction
    // \param min_z The minimum coordinate in the Z direction
    // \param max_z The maximum coordinate in the Z direction
    // \return void
    */
    void set(Real, Real, Real, Real, Real, Real) const noexcept {}
    //**************************************************************************

    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation. Unimplemented.
    //
    // \param span A span of Vector like elements
    // \return void
    */
    template <typename VecLike>  // Vec3 like
    void set(Span<VecLike>) const noexcept {}
    //**************************************************************************

    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation. Unimplemented.
    //
    // \param mins The minimum coordinates in all directions
    // \param maxs The maximum coordinates in all directions
    */
    void set(const Vec3&, const Vec3&) const noexcept {}
    //**************************************************************************

    //**Get function************************************************************
    /*!\brief Get lower bound of domain.
    //
    // Gets the lower bound of the domain in a particular Direction.
    //
    // \tparam Direction Direction to get lower bound, one of X, Y or Z
    */
    template <DirectionType Direction>
    inline constexpr Real get_lower_bound() const noexcept {
      return -inf;
    }
    //**************************************************************************

    //**Get function************************************************************
    /*!\brief Get upper bound of domain.
    //
    // Gets the upper bound of the domain in a particular Direction.
    //
    // \tparam Direction Direction to get upper bound, one of X, Y or Z
    */
    template <DirectionType Direction>
    inline constexpr Real get_upper_bound() const noexcept {
      return inf;
    }
    //**************************************************************************

    //**Get function************************************************************
    /*!\brief Get span of domain.
    //
    // Gets the span of the domain in a particular Direction.
    //
    // \tparam Direction Direction to get span, one of X, Y or Z
    */
    template <DirectionType Direction>
    inline Real get_span() const noexcept {
      return inf;
    }
    //**************************************************************************
  };

}  // namespace elastica
