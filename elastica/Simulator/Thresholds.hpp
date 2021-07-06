//==============================================================================
/*!
 *  \file pe/core/Thresholds.h
 *  \brief Numerical thresholds for the physics engine
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Collection of numerical threshold values.
   * \ingroup core
   *
   * The Thresholds class defines numerical floating point thresholds for the
   elastica physics engine.
   * The following thresholds can be used:
   *
   * - \b collision_threshold: This threshold is used for the contact
   classification. It is used to
   *      separate between separating, resting and colliding contacts.
   * - \b contact_threshold: This threshold is used to tell whether two rigid
   bodies are in contact or
   *      not. If the distance between two bodies is smaller than this
   threshold, they are considered
   *      to be in contact with each other.
   * - \b restitution_threshold: In case the relative velocity between two
   colliding rigid bodies is
   *      smaller than this threshold, a coefficient of restitution of 0 is used
   to avoid an infinite
   *      number of collisions during a single time step.
   * - \b friction_threshold: This threshold represents the boundary between
   static and dynamic
   *      friction. In case the relative tangential velocity of two contacting
   rigid bodies is
   *      smaller than this threshold, static friction is applied, else dynamic
   friction is used.
   * - \b surface_threshold: This threshold is used for surface checks. Only
   points with a distance
   *      to the surface smaller than this threshold are considered surface
   point.
   * - \b parallel_threshold: This threshold is used for parallelism checks. If
   the scalar product
   *      of two vectors is smaller than this threshold the vectors are
   considered to be parallel.
   *
   * The Thresholds class is used in the following manner:

     \code
     const double acc = Thresholds<double>::accuracy();
     if( dist < Thresholds<double>::surface_threshold ) {...}
     \endcode

   * \b Note: The Thresholds class is not defined for integral data types.
   */
  template <typename Float>
  struct Thresholds {};
  //****************************************************************************

  //============================================================================
  //
  //  FLOAT SPECIALIZATION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Thresholds<float> specialization.
   * \ingroup core
   */
  template <>
  struct Thresholds<float> {
   public:
    //! _threshold for the contact classification.
    /*! This threshold separates between separating, resting and colliding
     * contacts. */
    static inline constexpr float collision_threshold() { return 1E-8F; }

    //! _threshold for the distance between two rigid bodies.
    /*! Rigid bodies with a distance smaller than this threshold are in contact.
     */
    static inline constexpr float contact_threshold() { return 5E-7F; }

    //! _threshold for the restriction of the coefficient of restitution.
    /*! In case the relative velocity between two colliding rigid bodies is
       smaller than this threshold, a coefficient of restitution of 0 is used to
       avoid an infinite number of collisions during a single time step. */
    static inline constexpr float restitution_threshold() { return 1E-8F; }

    //! _threshold for the separation between static and dynamic friction.
    /*! This threshold represents the boundary between static and dynamic
     * friction. */
    static inline constexpr float friction_threshold() { return 1E-8F; }

    //! _threshold for surface points/checks.
    /*! Only points with a distance to the surface smaller than this threshold
       are considered surface point. */
    static inline constexpr float surface_threshold() { return 5E-7F; }

    //! _threshold for parallelism checks.
    /*! Scalar products smaller than this threshold value indicate parallel
     * vectors. */
    static inline constexpr float parallel_threshold() { return 1E-8F; }
  };
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  DOUBLE SPECIALIZATION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Thresholds<double> specialization.
   * \ingroup core
   */
  template <>
  struct Thresholds<double> {
   public:
    //! _threshold for the contact classification.
    /*! This threshold separates between separating, resting and colliding
     * contacts. */
    static inline constexpr double collision_threshold() { return 1E-8; }

    //! _threshold for the distance between two rigid bodies.
    /*! Rigid bodies with a distance smaller than this threshold are in contact.
     */
    static inline constexpr double contact_threshold() { return 1E-8; }

    //! _threshold for the restriction of the coefficient of restitution.
    /*! In case the relative velocity between two colliding rigid bodies is
       smaller than this threshold, a coefficient of restitution of 0 is used to
       avoid an infinite number of collisions during a single time step. */
    static inline constexpr double restitution_threshold() { return 1E-8; }

    //! _threshold for the separation between static and dynamic friction.
    /*! This threshold represents the boundary between static and dynamic
     * friction. */
    static inline constexpr double friction_threshold() { return 1E-8; }

    //! _threshold for surface points/checks.
    /*! Only points with a distance to the surface smaller than this threshold
       are considered surface point. */
    static inline constexpr double surface_threshold() { return 5E-7; }

    //! _threshold for parallelism checks.
    /*! Scalar products smaller than this threshold value indicate parallel
     * vectors. */
    static inline constexpr double parallel_threshold() { return 1E-8; }
  };
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  LONG DOUBLE SPECIALIZATION
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Thresholds<long double> specialization.
   * \ingroup core
   */
  template <>
  struct Thresholds<long double> {
   public:
    //! _threshold for the contact classification.
    /*! This threshold separates between separating, resting and colliding
     * contacts. */
    static inline constexpr long double collision_threshold() { return 1E-10L; }

    //! _threshold for the distance between two rigid bodies.
    /*! Rigid bodies with a distance smaller than this threshold are in contact.
     */
    static inline constexpr long double contact_threshold() { return 5E-7L; }

    //! _threshold for the restriction of the coefficient of restitution.
    /*! In case the relative velocity between two colliding rigid bodies is
       smaller than this threshold, a coefficient of restitution of 0 is used to
       avoid an infinite number of collisions during a single time step. */
    static inline constexpr long double restitution_threshold() {
      return 1E-8L;
    }

    //! _threshold for the separation between static and dynamic friction.
    /*! This threshold represents the boundary between static and dynamic
     * friction. */
    static inline constexpr long double friction_threshold() { return 1E-8L; }

    //! _threshold for surface points/checks.
    /*! Only points with a distance to the surface smaller than this threshold
       are considered surface point. */
    static inline constexpr long double surface_threshold() { return 5E-7L; }

    //! _threshold for parallelism checks.
    /*! Scalar products smaller than this threshold value indicate parallel
     * vectors. */
    static inline constexpr long double parallel_threshold() { return 1E-8L; }
  };
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  GLOBAL THRESHOLD VALUES
  //
  //============================================================================

  //****************************************************************************
  //! _threshold for the contact classification.
  /*! This threshold separates between separating, resting and colliding
   * contacts. */
  constexpr real_t collision_threshold =
      Thresholds<real_t>::collision_threshold();
  //****************************************************************************

  //****************************************************************************
  //! _threshold for the distance between two rigid bodies.
  /*! Rigid bodies with a distance smaller than this threshold are in contact.
   */
  constexpr real_t contact_threshold = Thresholds<real_t>::contact_threshold();
  //****************************************************************************

  //****************************************************************************
  //! _threshold for the restriction of the coefficient of restitution.
  /*! In case the relative velocity between two colliding rigid bodies is
     smaller than this threshold, a coefficient of restitution of 0 is used to
     avoid an infinite number of collisions during a single time step. */
  constexpr real_t restitution_threshold =
      Thresholds<real_t>::restitution_threshold();
  //****************************************************************************

  //****************************************************************************
  //! _threshold for the separation between static and dynamic friction.
  /*! This threshold represents the boundary between static and dynamic
   * friction. */
  constexpr real_t friction_threshold =
      Thresholds<real_t>::friction_threshold();
  //****************************************************************************

  //****************************************************************************
  //! _threshold for surface points/checks.
  /*! Only points with a distance to the surface smaller than this threshold are
     considered surface point. */
  constexpr real_t surface_threshold = Thresholds<real_t>::surface_threshold();
  //****************************************************************************

  //****************************************************************************
  //! _threshold for parallelism checks.
  /*! Scalar products smaller than this threshold value indicate parallel
   * vectors. */
  constexpr real_t parallel_threshold =
      Thresholds<real_t>::parallel_threshold();
  //****************************************************************************

}  // namespace elastica
