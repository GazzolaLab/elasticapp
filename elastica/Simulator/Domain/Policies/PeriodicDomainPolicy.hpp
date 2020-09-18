//==============================================================================
/*!
//  \file
//  \brief Periodic domain policy class
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
#include <utility>

#include "ErrorHandling/Assert.hpp"
#include "Simulator/Domain/Policies/Directions.hpp"
#include "Simulator/Domain/Span.hpp"
#include "Simulator/Domain/Types.hpp"
#include "Utilities/MakeSignalingNan.hpp"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/NonCopyable.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  /*!\brief Class template implementing a periodic domain policy
  // \ingroup domain
  //
  // The PeriodicDomainPolicy class template is a policy used by Elastica++'s
  // Domain class. It implements the logic for implementing periodic domains
  // in the context of Elastica++ simulations.
  */
  template <typename Real>  // Any real type
  class PeriodicDomainPolicy : public NonCopyable {
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
    PeriodicDomainPolicy();
    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~PeriodicDomainPolicy() = default;
    //@}
    //**************************************************************************

   public:
    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation.
    //
    // \param x_span The span in the X direction
    // \param y_span The span in the Y direction
    // \param z_span The span in the Z direction
    //
    // \return void
    */
    void set(SpanType&& x_span, SpanType&& y_span, SpanType&& z_span) noexcept {
      spans_[X] = std::move(x_span);
      spans_[Y] = std::move(y_span);
      spans_[Z] = std::move(z_span);
    }
    //**************************************************************************

    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation.
    //
    // \param min_x The minimum coordinate in the X direction
    // \param max_x The maximum coordinate in the X direction
    // \param min_y The minimum coordinate in the Y direction
    // \param max_y The maximum coordinate in the Y direction
    // \param min_z The minimum coordinate in the Z direction
    // \param max_z The maximum coordinate in the Z direction
    // \return void
    */
    void set(Real min_x, Real max_x, Real min_y, Real max_y, Real min_z,
             Real max_z) noexcept {
      set(SpanType(min_x, max_x), SpanType(min_y, max_y),
          SpanType(min_z, max_z));
    }
    //**************************************************************************

    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation.
    //
    // \param span A span of Vector like elements
    // \return void
    */
    template <typename VecLike>  // Vec3 like
    void set(Span<VecLike> span) noexcept {
      VecLike& mins = span.span_[span.min_idx];
      VecLike& maxs = span.span_[span.max_idx];
      set(SpanType(mins[X], maxs[X]), SpanType(mins[Y], maxs[Y]),
          SpanType(mins[Z], maxs[Z]));
    }
    //**************************************************************************

    //**Set function************************************************************
    /*!\brief Set span of domain.
    //
    // Sets the simulation domain after creation.
    //
    // \param mins The minimum coordinates in all directions
    // \param maxs The maximum coordinates in all directions
    */
    void set(const Vec3& mins, const Vec3& maxs) noexcept {
      set(Span<Vec3>(mins, maxs));
    }
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
      return spans_[Direction][SpanType::min_idx];
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
      return spans_[Direction][SpanType::max_idx];
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
      ELASTICA_ASSERT(
          get_upper_bound<Direction>() > get_lower_bound<Direction>(),
          "Wrong span in simulator!");
      return (get_upper_bound<Direction>() - get_lower_bound<Direction>());
    }
    //**************************************************************************

   private:
    //**Member variables********************************************************
    SpanStorage spans_;  //!< Storage of spans in the domain
    //**************************************************************************
  };
  //****************************************************************************

  //============================================================================
  //
  //  CONSTRUCTORS
  //
  //============================================================================

  //**Constructor***************************************************************
  /*!\brief Constructor for the PeriodicDomainPolicy class.
  //
  */
  template <typename Real>  // Any real type
  PeriodicDomainPolicy<Real>::PeriodicDomainPolicy() {
#ifdef ELASTICA_DEBUG
    auto infty = make_signaling_NaN<Real>();
#else
    auto infty = inf;
#endif
    spans_[X] = SpanType(-infty, infty);
    spans_[Y] = SpanType(-infty, infty);
    spans_[Z] = SpanType(-infty, infty);
  }
  //****************************************************************************

}  // namespace elastica