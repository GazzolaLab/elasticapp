//==============================================================================
/*!
//  \file
//  \brief Implementation of an axis-aligned bounding box
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
**
**  Project home: https://www.cs10.tf.fau.de/research/software/pe/
**
**  Copyright (C) 2009 Klaus Iglberger
**
**  This file is part of pe.
**
**  pe is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License as published by the Free Software
** Foundation, either version 3 of the License, or (at your option) any later
** version.
**
**  pe is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License along with
** pe. If not, see <http://www.gnu.org/licenses/>.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <algorithm>
#include <cmath>
#include <ostream>
#include <utility>

#include "ErrorHandling/Assert.hpp"
//
#include "Simulator/Frames.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Equal.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Axis-aligned bounding box for rigid bodies.
         * \ingroup coarse_collision_detection
         *
         * An axis-aligned bounding box is a bounding volume for rigid bodies.
         * One basic property of an axis-aligned bounding box is that it is
         * always aligned with the axes of the global world frame.
         * Therefore the bounding box can be represented by two vertices: the
         * first gives the coordinates of the bottom-left-front vertex \f$
         * (x_1,y_1,z_1) \f$, the second is the upper-right-back vertex
         * \f$ (x_2,y_2,z_2) \f$. Due to this arrangement, the x-, y- and
         * z-component of the first vertex have to be smaller than their
         * counterparts in the second vertex, otherwise the bounding box is
         * invalid.
         *
         * Here's an example of using BoundingBox
         * \example
         * \code
         * using namespace elastica;
         * using namespace elastica::collision::detection::coarse;
         *
         * using AABB = BoundingBox<real_t>;
         *
         * // Creating a default bounding box with the vertices (-1,-1,-1) and
         * // (1,1,1)
         * AABB aabb1;
         *
         * // Creating a bounding box with the vertices (1,1,1) and (2,2,2)
         * AABB aabb2( 1, 1, 1, 2, 2, 2 );
         *
         * // Merging the two bounding boxes results in a bounding box from
         * // (-1,-1,-1) to (2,2,2)
         * AABB aabb3 = aabb1 + aabb2;
         * \endcode
         *
         * \tparam Type Element type of the axis-aligned bounding box
         */
        template <typename Type>
        class BoundingBox {
         public:
          //**Type definitions**************************************************
          //! Type of the bounding box elements.
          using ElementType = Type;
          //********************************************************************

          //**Constructors******************************************************
          /*!\name Constructors */
          //@{
          explicit inline BoundingBox();
          explicit inline BoundingBox(Type const& x1, Type const& y1,
                                      Type const& z1, Type const& x2,
                                      Type const& y2, Type const& z2);

          /*
          template <typename Other>
          explicit inline BoundingBox(const Vector3<Other>& a,
                                      const Vector3<Other>& b);
                                      */

          inline BoundingBox(BoundingBox const& aabb);
          //          inline BoundingBox(BoundingBox&& aabb) noexcept;

          template <typename Other>
          inline BoundingBox(BoundingBox<Other> const& aabb);
          //@}
          //********************************************************************

          //**Destructor********************************************************
          // No explicitly declared destructor.
          //********************************************************************

          //**Bounding box operators********************************************
          /*!\name Bounding box operators */
          //@{
          inline BoundingBox& operator=(BoundingBox rhs) noexcept;
          template <typename Other>
          inline BoundingBox& operator=(BoundingBox<Other> rhs) noexcept;
          inline BoundingBox& operator+=(BoundingBox const& rhs) noexcept;
          inline Type& operator[](size_t index) & noexcept;
          inline const Type& operator[](size_t index) const& noexcept;
          //@}
          //********************************************************************

          //**Utility functions*************************************************
          /*!\name Utility functions */
          //@{
          inline bool is_valid() const noexcept;
          inline void reset() noexcept;
          template <typename Other>
          inline bool equal(BoundingBox<Other> const& rhs) const noexcept;

          template <::elastica::EulerianFrame::DirectionType Dir>
          inline auto center() const noexcept -> Type;
          template <::elastica::EulerianFrame::DirectionType Dir>
          inline auto length() const noexcept -> Type;
          //        inline bool is_colliding() const;

          template <typename Other>
          inline bool overlaps(BoundingBox<Other> const& aabb) const noexcept;
          template <typename Other>
          inline bool overlaps(BoundingBox<Other> const& aabb,
                               real_t dx) const noexcept;
          inline bool contains(real_t px, real_t py, real_t pz) const noexcept;
          inline bool contains(Vec3 const& gpos) const noexcept;

          template <typename Other>
          inline bool contains(BoundingBox<Other> const& aabb) const noexcept;
          template <typename Other>
          inline bool contains(BoundingBox<Other> const& aabb,
                               real_t dx) const noexcept;
          inline BoundingBox& intersect(const BoundingBox& rhs) noexcept;
          //@}
          //********************************************************************

         private:
          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          Type v_[6];  //!< Bottom-left-front and upper-right-back vertex.
          /*!< The first three values are used for the Bottom-left-front vertex
             <x1,y1,z1>
               and the last three values for the upper-right-back vertex
             <x2,y2,z2>. */
          //@}
          //********************************************************************

          //**Friend declarations***********************************************
          /*! \cond ELASTICA_INTERNAL */
          template <typename Other>
          friend class BoundingBox;
          /*! \endcond */
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  CONSTRUCTORS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Default constructor for the axis-aligned bounding box.
         *
         * This constructor initializes the axis-aligned bounding box to the
         * default values provided by #reset
         */
        template <typename Type>
        inline BoundingBox<Type>::BoundingBox() {
          reset();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constructor for the direct initialization of the bounding box.
         *
         * \param x1 x-component of the bottom-left-front vertex.
         * \param y1 y-component of the bottom-left-front vertex.
         * \param z1 z-component of the bottom-left-front vertex.
         * \param x2 x-component of the upper-right-back vertex.
         * \param y2 y-component of the upper-right-back vertex.
         * \param z2 z-component of the upper-right-back vertex.
         *
         * Direct initialization of the bounding box. The first vertex is
         * initialized as (x1,y1,z1), whereas the second bounding box is set to
         * (x2,y2,z2).
         */
        template <typename Type>
        inline BoundingBox<Type>::BoundingBox(Type const& x1, Type const& y1,
                                              Type const& z1, Type const& x2,
                                              Type const& y2, Type const& z2) {
          ELASTICA_ASSERT((x1 < x2 and y1 < y2 and z1 < z2),
                          "Invalid bounding box values");

          v_[0] = x1;
          v_[1] = y1;
          v_[2] = z1;
          v_[3] = x2;
          v_[4] = y2;
          v_[5] = z2;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constructor for the direct initialization of the bounding box.
         *
         * \param a The bottom-left-front vertex.
         * \param b The upper-right-back vertex.
         *
         * Direct initialization of the bounding box. The first vertex is
         * initialized with \a a, whereas the second bounding box is set to \a
         * b.
         */
        /*
         template <typename Type>  // Element type of the axis-aligned bounding
         box template <typename Other>  // Element type of the vectors inline
         BoundingBox<Type>::BoundingBox(const Vector3<Other>& a, const
         Vector3<Other>& b) { v_[0] = a[0]; v_[1] = a[1]; v_[2] = a[2]; v_[3] =
         b[0]; v_[4] = b[1]; v_[5] = b[2];
         }
        */
        //**********************************************************************

        //**********************************************************************
        /*!\brief Copy constructor for axis-aligned bounding boxes.
         *
         * \param aabb Bounding box to be copied.
         *
         * Explicit definition of a copy constructor in order to
         * enable/facilitate NRV optimization.
         */
        template <typename Type>
        inline BoundingBox<Type>::BoundingBox(BoundingBox const& aabb) {
          // std::copy(std::begin(v_), std::end(v_), std::begin(aabb.v_));
          v_[0] = aabb.v_[0];
          v_[1] = aabb.v_[1];
          v_[2] = aabb.v_[2];
          v_[3] = aabb.v_[3];
          v_[4] = aabb.v_[4];
          v_[5] = aabb.v_[5];
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Conversion constructor from different BoundingBox instances.
         *
         * \param aabb Bounding box to be copied.
         */
        template <typename Type>   // Element type of the axis-aligned
                                   // bounding box
        template <typename Other>  // Element type of the foreign axis-aligned
                                   // bounding box
        inline BoundingBox<Type>::BoundingBox(BoundingBox<Other> const& aabb) {
          v_[0] = aabb.v_[0];
          v_[1] = aabb.v_[1];
          v_[2] = aabb.v_[2];
          v_[3] = aabb.v_[3];
          v_[4] = aabb.v_[4];
          v_[5] = aabb.v_[5];
        }
        //**********************************************************************

        //        //**********************************************************************
        //        /*!\brief Move constructor for axis-aligned bounding boxes.
        //         *
        //         * \param aabb Bounding box to be moved.
        //         *
        //         * Explicit definition of a copy constructor in order to
        //         * enable/facilitate NRV optimization.
        //         */
        //        template <typename Type>
        //        inline BoundingBox<Type>::BoundingBox(BoundingBox&& aabb)
        //        noexcept
        //            : v_(std::move(aabb.v_)) {
        //          // really no benefit here
        //        }
        //        //**********************************************************************
        //

        //======================================================================
        //
        //  OPERATORS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Copy assignment operator for axis-aligned bounding boxes.
         *
         * \param rhs Bounding box to be copied.
         * \return Reference to the assigned bounding box.
         *
         * Explicit definition of a copy assignment operator for performance
         * reasons.
         */
        template <typename Type>
        inline BoundingBox<Type>& BoundingBox<Type>::operator=(
            BoundingBox rhs) noexcept {
          // Note : this claim is questionable
          // This implementation is faster than the synthesized default copy
          // assignment operator and faster than an implementation with the C
          // library function 'memcpy' in combination with a protection against
          // self-assignment. Additionally, this version goes without a
          // protection against self-assignment.
          using std::swap;
          swap(v_, rhs.v_);  // array swap from C++11 onwards
          // v_ = std::move(rhs.v_);
          return *this;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Copy assignment operator for different axis-aligned bounding
         * box instances.
         *
         * \param rhs Bounding box to be copied.
         * \return Reference to the assigned bounding box.
         */
        template <typename Type>   // Element type of the axis-aligned bounding
                                   // box
        template <typename Other>  // Element type of the foreign axis-aligned
                                   // bounding box
        inline BoundingBox<Type>& BoundingBox<Type>::operator=(
            BoundingBox<Other> rhs) noexcept {
          using std::swap;
          swap(v_, rhs.v_);  // array swap from C++11 onwards
          // v_ = std::move(rhs.v_);
          return *this;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Merge operation for two axis-aligned bounding boxes.
         *
         * \param rhs Right-hand side bounding box to be merged into the
         * left-hand side bounding box.
         * \return Reference to the (adjusted) left-hand side bounding box.
         *
         * Merging of a second axis-aligned bounding box into the left-hand side
         * bounding box to form a combined bounding box that contains both
         * original bounding boxes.
         *
         * \b Note: The result of the merging process is undefined for invalid
         * bounding boxes!
         */
        template <typename Type>
        inline BoundingBox<Type>& BoundingBox<Type>::operator+=(
            BoundingBox const& rhs) noexcept {
          using std::max;
          using std::min;
          v_[0] = min(v_[0], rhs.v_[0]);
          v_[1] = min(v_[1], rhs.v_[1]);
          v_[2] = min(v_[2], rhs.v_[2]);
          v_[3] = max(v_[3], rhs.v_[3]);
          v_[4] = max(v_[4], rhs.v_[4]);
          v_[5] = max(v_[5], rhs.v_[5]);
          return *this;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Direct access to the bounding box vertices/values.
         *
         * \param index Index of the accessed bounding box value.
         * \return Reference to the accessed value.
         *
         * Direct access to the bounding box vertices. The first vertex is
         * accessed with the indices [0..2], whereas the second vertex is
         * accessed with the indices [3..5].
         */
        template <typename Type>
        inline auto BoundingBox<Type>::operator[](size_t index) & noexcept
            -> Type& {
          ELASTICA_ASSERT(index < 6, "Invalid bounding box access index");
          return v_[index];
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Direct access to the bounding box vertices/values.
         *
         * \param index Index of the accessed bounding box value.
         * \return Reference-to-const to the accessed value.
         *
         * Direct access to the bounding box vertices. The first vertex is
         * accessed with the indices [0..2], whereas the second vertex is
         * accessed with the indices [3..5].
         */
        template <typename Type>
        inline auto BoundingBox<Type>::operator[](size_t index) const& noexcept
            -> Type const& {
          ELASTICA_ASSERT(index < 6, "Invalid bounding box access index");
          return v_[index];
        }
        //**********************************************************************

        //======================================================================
        //
        //  UTILITY FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Validity check on a axis-aligned bounding box.
         *
         * \return \a true if the bounding box is valid, \a false if it is not.
         *
         * Checking the validity of the axis-aligned bounding box: all
         * components of the first bottom- left-front vertex have to be smaller
         * than their counterparts in the second upper-right-back vertex.
         */
        template <typename Type>
        inline bool BoundingBox<Type>::is_valid() const noexcept {
          return !(v_[0] >= v_[3] or v_[1] >= v_[4] or v_[2] >= v_[5]);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Resetting the axis-aligned bounding box to the default values.
         *
         * \return void
         *
         * This function resets the axis-aligned bounding box to its default
         * values: the bottom-left-front vertex is reset to (-1,-1,-1), the
         * upper-right-back vertex is reset to (1,1,1).
         */
        template <typename Type>
        inline void BoundingBox<Type>::reset() noexcept {
          v_[0] = -1;
          v_[1] = -1;
          v_[2] = -1;
          v_[3] = 1;
          v_[4] = 1;
          v_[5] = 1;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Equality comparison of two axis-aligned bounding boxes.
         *
         * \param rhs Right-hand side bounding box to be compared.
         * \return \a true if the two bounding boxes are equal, \a false if not.
         */
        template <typename Type>   // Element type of the axis-aligned bounding
                                   // box
        template <typename Other>  // Element type of the foreign axis-aligned
                                   // bounding box
        inline bool BoundingBox<Type>::equal(
            BoundingBox<Other> const& rhs) const noexcept {
          return not(not(elastica::equal(v_[0], rhs.v_[0])) or
                     not(elastica::equal(v_[1], rhs.v_[1])) or
                     not(elastica::equal(v_[2], rhs.v_[2])) or
                     not(elastica::equal(v_[3], rhs.v_[3])) or
                     not(elastica::equal(v_[4], rhs.v_[4])) or
                     not(elastica::equal(v_[5], rhs.v_[5])));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets coordinates of the center of the axis-aligned bounding
         * boxes in a given Direction.
         *
         * \tparam Dir An ::elastica::EulerianFrame::DirectionType along which
         * we need to get the center()
         */
        template <typename Type>
        template <::elastica::EulerianFrame::DirectionType Dir>
        inline auto BoundingBox<Type>::center() const noexcept -> Type {
          // note : in this case we static cast the 0.5 directly
          // as we intend to use only floating point numbers here
          // unlike index-based numbers
          constexpr auto idx = ::elastica::cast_along(Dir);
          return (static_cast<Type>(0.5) * (v_[idx] + v_[idx + 3]));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets length of the axis-aligned bounding boxes in a given
         * Direction.
         *
         * \tparam Dir An ::elastica::EulerianFrame::DirectionType along which
         * we need to get the length()
         */
        template <typename Type>
        template <::elastica::EulerianFrame::DirectionType Dir>
        inline auto BoundingBox<Type>::length() const noexcept -> Type {
          constexpr auto idx = ::elastica::cast_along(Dir);
          return (v_[idx + 3] - v_[idx]);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Overlap test between two axis-aligned bounding boxes.
         *
         * \param aabb The second axis-aligned bounding box.
         * \return \a true if the two bounding boxes overlap, \a false if not.
         *
         * This function tests, whether the bounding box overlaps with the
         * second bounding box \a aabb. In case the bounding box overlaps with
         * \a aabb, the function returns \a true, otherwise the function returns
         * \a false.
         */
        template <typename Type>   // Element type of the axis-aligned bounding
                                   // box
        template <typename Other>  // Element type of the foreign axis-aligned
                                   // bounding box
        inline bool BoundingBox<Type>::overlaps(
            BoundingBox<Other> const& aabb) const noexcept {
          ELASTICA_ASSERT(is_valid(), "Invalid bounding box");
          ELASTICA_ASSERT(aabb.is_valid(), "Invalid bounding box");

          return !(aabb.v_[0] > v_[3] or aabb.v_[1] > v_[4] or
                   aabb.v_[2] > v_[5] or aabb.v_[3] < v_[0] or
                   aabb.v_[4] < v_[1] or aabb.v_[5] < v_[2]);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Overlap test between two axis-aligned bounding boxes.
         *
         * \param aabb The second axis-aligned bounding box.
         * \param dx Spacing between the two bounding boxes.
         * \return \a true if the two bounding boxes overlap, \a false if not.
         *
         * In this overlap test, the first bounding box is increased by \a dx in
         * all directions. The function detects an overlap, if the second
         * bounding box overlaps the first, increased bounding box. In this
         * case, the function returns \a true, otherwise it returns \a false.
         */
        template <typename Type>   // Element type of the axis-aligned bounding
                                   // box
        template <typename Other>  // Element type of the foreign axis-aligned
                                   // bounding box
        inline bool BoundingBox<Type>::overlaps(BoundingBox<Other> const& aabb,
                                                real_t dx) const noexcept {
          ELASTICA_ASSERT(is_valid(), "Invalid bounding box");
          ELASTICA_ASSERT(aabb.is_valid(), "Invalid bounding box");

          return !(aabb.v_[0] > v_[3] + dx or aabb.v_[1] > v_[4] + dx or
                   aabb.v_[2] > v_[5] + dx or aabb.v_[3] < v_[0] - dx or
                   aabb.v_[4] < v_[1] - dx or aabb.v_[5] < v_[2] - dx);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Intersection test for global coordinates.
         *
         * \param px The x-component of the global coordinate.
         * \param py The y-component of the global coordinate.
         * \param pz The z-component of the global coordinate.
         * \return \a true if the coordinate is contained in the bounding box,
         * \a false if not.
         *
         * This function tests, whether the given global coordinate is contained
         * in or on the surface of the bounding box. In case the point is
         * contained in the bounding box, the function returns \a true.
         * Otherwise \a false is returned.
         */
        template <typename Type>
        inline bool BoundingBox<Type>::contains(real_t px, real_t py,
                                                real_t pz) const noexcept {
          return !(px < v_[0] or py < v_[1] or pz < v_[2] or px > v_[3] or
                   py > v_[4] or pz > v_[5]);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Intersection test for global coordinates.
         *
         * \param gpos The global coordinate.
         * \return \a true if the coordinate is contained in the bounding box,
         * \a false if not.
         *
         * This function tests, whether the given global coordinate is contained
         * in or on the surface of the bounding box. In case \a gpos is
         * contained in the bounding box, the function returns \a true.
         * Otherwise \a false is returned.
         */
        template <typename Type>
        inline bool BoundingBox<Type>::contains(
            Vec3 const& gpos) const noexcept {
          return !(gpos[0] < v_[0] or gpos[1] < v_[1] or gpos[2] < v_[2] or
                   gpos[0] > v_[3] or gpos[1] > v_[4] or gpos[2] > v_[5]);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Intersection test for axis-aligned bounding boxes.
         *
         * \param aabb The second axis-aligned bounding box.
         * \return \a true if the bounding box contains the other bounding box,
         * \a false if not.
         *
         * This function tests, whether the bounding box contains the second
         * bounding box \a aabb. In case \a aabb is completely contained in the
         * bounding box, the function returns \a true. Otherwise \a false is
         * returned.
         */
        template <typename Type>   // Element type of the axis-aligned bounding
                                   // box
        template <typename Other>  // Element type of the foreign axis-aligned
                                   // bounding box
        inline bool BoundingBox<Type>::contains(
            BoundingBox<Other> const& aabb) const noexcept {
          ELASTICA_ASSERT(is_valid(), "Invalid bounding box");
          ELASTICA_ASSERT(aabb.is_valid(), "Invalid bounding box");

          return !(aabb.v_[0] < v_[0] or aabb.v_[1] < v_[1] or
                   aabb.v_[2] < v_[2] or aabb.v_[3] > v_[3] or
                   aabb.v_[4] > v_[4] or aabb.v_[5] > v_[5]);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Intersection test for axis-aligned bounding boxes.
         *
         * \param aabb The second axis-aligned bounding box.
         * \param dx Spacing between the two bounding boxes.
         * \return \a true if the bounding box contains the other bounding box,
         * \a false if not.
         *
         * In this intersection test, the first bounding box is increased by \a
         * dx in all directions. The function tests, if the increased
         * axis-aligned bounding box contains the second bounding box. In case
         * \a aabb is completely contained in the bounding box, the function
         * returns \a true. Otherwise \a false is returned.
         */
        template <typename Type>   // Element type of the axis-aligned bounding
                                   // box
        template <typename Other>  // Element type of the foreign axis-aligned
                                   // bounding box
        inline bool BoundingBox<Type>::contains(BoundingBox<Other> const& aabb,
                                                real_t dx) const noexcept {
          ELASTICA_ASSERT(is_valid(), "Invalid bounding box");
          ELASTICA_ASSERT(aabb.is_valid(), "Invalid bounding box");

          return !(aabb.v_[0] < v_[0] - dx or aabb.v_[1] < v_[1] - dx or
                   aabb.v_[2] < v_[2] - dx or aabb.v_[3] > v_[3] + dx or
                   aabb.v_[4] > v_[4] + dx or aabb.v_[5] > v_[5] + dx);
        }
        //**********************************************************************

        //***********************************************************************
        /*!\brief Intersect operation for two axis-aligned bounding boxes.
         *
         * \param rhs Right-hand side bounding box to be merged into the
         * left-hand side bounding box.
         * \return Reference to the (adjusted) left-hand side bounding box.
         *
         * Intersection of a second axis-aligned bounding box into the left-hand
         * side bounding box to form a combined bounding box that contains both
         * original bounding boxes.
         *
         * \b Note: The result of the intersection process is undefined for
         * invalid bounding boxes!
         */
        template <typename Type>  // Element type of the axis-aligned bounding
        inline BoundingBox<Type>& BoundingBox<Type>::intersect(
            const BoundingBox& rhs) noexcept {
          if (this->overlaps(rhs)) {
            using std::max;
            using std::min;
            v_[0] = max(v_[0], rhs.v_[0]);
            v_[1] = max(v_[1], rhs.v_[1]);
            v_[2] = max(v_[2], rhs.v_[2]);
            v_[3] = min(v_[3], rhs.v_[3]);
            v_[4] = min(v_[4], rhs.v_[4]);
            v_[5] = min(v_[5], rhs.v_[5]);
          } else {
            reset();
          }
          return *this;
        }
        //**********************************************************************

        //======================================================================
        //
        //  GLOBAL OPERATORS
        //
        //======================================================================

        //**********************************************************************
        /*!\name Axis-aligned bounding box operators */
        //@{

        //**********************************************************************
        /*!\brief Merge operation for two axis-aligned bounding boxes.
         *
         * \param lhs Left-hand side bounding box to be merged
         * \param rhs Right-hand side bounding box to be merged
         * \return The new combined bounding box.
         *
         * Merging of two axis-aligned bounding boxes into a new combined
         * bounding box that contains both original bounding boxes.
         */
        template <typename Type>
        inline auto operator+(BoundingBox<Type> /*const*/ lhs,
                              BoundingBox<Type> const& rhs) noexcept
            -> BoundingBox<Type> {
          lhs += rhs;
          return lhs;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Merge operation for two axis-aligned bounding boxes.
         *
         * \param lhs Left-hand side bounding box to be merged
         * \param rhs Right-hand side bounding box to be merged
         * \return The new combined bounding box.
         *
         * Merging of two axis-aligned bounding boxes into a new combined
         * bounding box that contains both original bounding boxes.
         */
        template <typename Type>  // Element type of the bounding box
        inline auto merge(BoundingBox<Type> const& lhs,
                          BoundingBox<Type> const& rhs) noexcept {
          return lhs + rhs;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Equality comparison between two axis-aligned bounding boxes.
         * \ingroup coarse_collision_detection
         *
         * \param lhs Left-hand side bounding box to be compared.
         * \param rhs Right-hand side bounding box to be compared.
         * \return \a true if the two bounding boxes are equal, \a false if not.
         */
        template <typename T1, typename T2>
        inline bool operator==(BoundingBox<T1> const& lhs,
                               BoundingBox<T2> const& rhs) noexcept {
          return lhs.equal(rhs);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Inequality comparison between two axis-aligned bounding boxes.
         * \ingroup coarse_collision_detection
         *
         * \param lhs Left-hand side bounding box to be compared.
         * \param rhs Right-hand side bounding box to be compared.
         * \return \a true if the two bounding boxes are not equal, \a false if
         * they are equal.
         */
        template <typename T1, typename T2>
        inline bool operator!=(BoundingBox<T1> const& lhs,
                               BoundingBox<T2> const& rhs) noexcept {
          return !lhs.equal(rhs);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if two two axis-aligned bounding boxes overlap
         *
         * \param lhs Left-hand side bounding box to be compared for overlap.
         * \param rhs Right-hand side bounding box to be compared for overlap.
         * \return \a true if the two bounding boxes overlap, \a false if not.
         */
        template <typename T1, typename T2>
        inline auto overlaps(BoundingBox<T1> const& lhs,
                             BoundingBox<T2> const& rhs) noexcept -> bool {
          return lhs.overlaps(rhs);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets length of the bounding box along axes
         * \ingroup coarse_collision_detection
         *
         * \tparam Dir An ::elastica::EulerianFrame::DirectionType along which
         * we need to get the length()
         * \param aabb Reference to an axis-aligned bounding box.
         * \return Length of the bounding box along the given axis Dir
         */
        template <::elastica::EulerianFrame::DirectionType Dir, typename Type>
        inline Type length_along(BoundingBox<Type> const& aabb) noexcept {
          return aabb.template length<Dir>();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Returns the length of the longest side of the AABB of the
         * rigid body.
         * \ingroup coarse_collision_detection
         *
         * \param aabb Reference to an axis-aligned bounding box.
         * \return The length of the longest side of the AABB of the rigid body.
         */
        template <typename Type>
        inline Type length(BoundingBox<Type> const& aabb) noexcept {
          using std::max;
          const auto x_size = length_along<::elastica::EulerianFrame::X>(aabb);
          const auto y_size = length_along<::elastica::EulerianFrame::Y>(aabb);
          const auto z_size = length_along<::elastica::EulerianFrame::Z>(aabb);
          return max(x_size, max(y_size, z_size));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Intersect operation for two axis-aligned bounding boxes.
         *
         * \param lhs Left-hand side bounding box to be intersected
         * \param rhs Right-hand side bounding box to be intersected
         * \return The new combined bounding box.
         *
         * Intersection of two axis-aligned bounding boxes into a new combined
         * bounding box that contains both original bounding boxes.
         */
        template <typename Type>  // Element type of the bounding box
        inline auto intersect(BoundingBox<Type> lhs,
                              BoundingBox<Type> const& rhs) noexcept
            -> BoundingBox<Type> {
          lhs.intersect(rhs);
          return lhs;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Global output operator for axis-aligned bounding boxes.
         * \ingroup coarse_collision_detection
         *
         * \param os Reference to the output stream.
         * \param aabb Reference to an axis-aligned bounding box.
         * \return Reference to the output stream.
         */
        template <typename Type>
        inline std::ostream& operator<<(std::ostream& os,
                                        BoundingBox<Type> const& aabb) {
          return os << "<" << aabb[0] << "," << aabb[1] << "," << aabb[2]
                    << "> , "
                    << "<" << aabb[3] << "," << aabb[4] << "," << aabb[5]
                    << ">";
        }
        //**********************************************************************

        //@}
        //**********************************************************************

      }  // namespace coarse

    }  // namespace detection

    //  using AABB = ::elastica::detection::coarse::BoundingBox<real_t>;

  }  // namespace collision

}  // namespace elastica
