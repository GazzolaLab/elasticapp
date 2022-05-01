//=================================================================================================
/*!
 *  \file pe/core/detection/fine/GJK.h
 *  \brief The Gilbert-Johnson-Keerthi Algorithm
 *
 *  Copyright (C) 2013-2014 Tobias Scharpff
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
//=================================================================================================

#pragma once

//*************************************************************************************************
// Includes
//*************************************************************************************************

//#include <pe/math/Vector3.h>
//#include <pe/util/Limits.h>
#include <iostream>  //for error message
#include <vector>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Math/Accuracy.hpp"
#include "elastica/Utilities/Math/Dot.hpp"
#include "elastica/Utilities/Math/Normalize.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //=================================================================================================
        //
        //  CLASS DEFINITION
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief The Gilbert-Johnson-Keerthi Algorithm.
         * \ingroup fine_collision_detection
         */
        class GJK {
         public:
          //**Constructor*********************************************************************************
          /*!\name Constructor */
          //@{
          explicit inline GJK();
          //@}
          //**********************************************************************************************

          //**Query
          // functions*****************************************************************************
          /*!\name Query functions */
          //@{
          template <typename FirstPrimitive, typename SecondPrimitive>
          inline real_t doGJK(FirstPrimitive& first_primitive,
                              SecondPrimitive& second_primitive, Vec3& normal,
                              Vec3& contact_point);

          template <typename FirstPrimitive, typename SecondPrimitive>
          inline bool do_contact_threshold(FirstPrimitive& first_primitive,
                                           SecondPrimitive& second_primitive);

          //@}
          //**********************************************************************************************

          //**Get
          // functions*******************************************************************************
          /*!\name Get functions */
          //@{
          inline const std::vector<Vec3>& getSimplex() const noexcept;
          inline constexpr std::size_t getSimplexSize() const noexcept;
          inline constexpr const std::vector<Vec3>& getSupportA()
              const noexcept;
          inline constexpr const std::vector<Vec3>& getSupportB()
              const noexcept;
          //@}
          //**********************************************************************************************

         private:
          //**Utility
          // functions***************************************************************************
          /*!\name Utility functions */
          //@{
          inline bool simplex2(
              Vec3& d);  // TODO rename 0-Simplex is a line not 1-Simplex
          inline bool simplex3(Vec3& d);
          inline bool simplex4(Vec3& d);

          template <typename Vec3T>
          inline bool sameDirection(const Vec3T& vec1,
                                    const Vec3& vec2) const noexcept;
          inline bool zeroLengthVector(const Vec3& vec) const noexcept;
          inline real_t calcDistance(Vec3& normal, Vec3& contact_point);
          //@}
          //**********************************************************************************************

          //**Member
          // variables****************************************************************************
          /*!\name Member variables */
          //@{
          std::vector<Vec3> simplex_;   //<! Container to hold the simplex.
          std::vector<Vec3> supportA_;  //<! Container to hold the support
                                        // points generated in triangle mesh mA
          std::vector<Vec3> supportB_;  //<! Container to hold the support
                                        // points generated in triangle mesh mB
          unsigned char
              numPoints_;  //<! Current number of points in the simplex.
          Vec3 d_;         //<! The next search direction.
                           //@}
          //**********************************************************************************************
        };
        //*************************************************************************************************

        //=================================================================================================
        //
        //  CONSTRUCTOR
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline GJK::GJK()
            : simplex_(4), supportA_(4), supportB_(4), numPoints_(0) {
          d_ = Vec3{0.0, 0.6, 0.8};  // Vec3(0.0, 3.0, 4.0)/5.0 just start with
                                     // any vector of length 1
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  QUERY FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         *
         * returns distance between first_primitive and second_primitive or 0.0
         * if they are intersecting
         */
        template <typename FirstPrimitive,   // Type of the first primitive
                  typename SecondPrimitive>  // Type of the second primitive
        inline real_t GJK::doGJK(FirstPrimitive& first_primitive,
                                 SecondPrimitive& second_primitive,
                                 Vec3& normal, Vec3& contact_point) {
          // Variables
          Vec3 support;  // the current support point
          real_t ret;  // return value aka distance between first_primitive and
                       // second_primitive

          ////////////////////////////////////////////////////////////////////////
          // Initial initialisation step
          ret = 0.0;
          supportA_.resize(4);
          supportB_.resize(4);
          simplex_.resize(4);

          // get any first support point
          if (numPoints_ != 0) {
            d_ = normalize(d_);
          }
          supportA_[0] = first_primitive.support(d_);
          supportB_[0] = second_primitive.support(-d_);
          support = supportA_[0] - supportB_[0];

          // add this point to the simplex_
          simplex_[0] = support;
          numPoints_ = 1;

          // first real_t search direction is in the opposite direction of the
          // first support point
          d_ = -support;

          /*
             if(trans(support) * d_ < 0.0){
                //we went as far as we could in direction 'd' but not passed the
             origin
                //this means the triangle mashes don't overlap
                //and as the support()-function extends the support point by
             contactThreshold
                //the mashes are not even close enough to be considered in
             contact. return false;
             }
             */
          ////////////////////////////////////////////////////////////////////////
          // GJK main loop
          while (true) {
            // get the support point in the current search direction
            d_ = normalize(d_);
            supportA_[numPoints_] = first_primitive.support(d_);
            supportB_[numPoints_] = second_primitive.support(-d_);
            support = supportA_[numPoints_] - supportB_[numPoints_];

            // check if "support" is passed the origin in search direction
            if (dot(support, d_) < real_t(0.0)) {
              // we went as far as we could in direction 'd' but not passed the
              // origin this means the triangle mashes don't overlap

              // calc distance simplex to Origin
              ret = calcDistance(normal, contact_point);

              return ret;
            }

            // add the new support point into the simplex
            simplex_[numPoints_] = support;
            numPoints_++;

            ////////////////////////////////////////////////////////////////
            // check if the origin is in the simplex
            // if it is the triangle mashes are overlapping
            switch (numPoints_) {
              case 2: {
                if (simplex2(d_)) {
                  simplex_.pop_back();
                  simplex_.pop_back();
                  supportA_.pop_back();
                  supportA_.pop_back();
                  supportB_.pop_back();
                  supportB_.pop_back();
                  return ret;
                }
              } break;

              case 3: {
                if (simplex3(d_)) {
                  simplex_.pop_back();
                  supportA_.pop_back();
                  supportB_.pop_back();
                  return ret;
                }
              } break;

              case 4: {
                if (simplex4(d_)) {
                  return ret;
                }
              } break;

              default: {
                // std::cerr << "numPoints_="<< numPoints_ <<std::endl;
                ELASTICA_ASSERT(
                    false, "Number of points in the simplex is not 1<=n<=4");
              } break;
            }
          }

          return ret;  // never reach this point
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        template <typename FirstPrimitive,   // Type of the first primitive
                  typename SecondPrimitive>  // Type of the second primitive
        inline bool GJK::do_contact_threshold(
            FirstPrimitive& first_primitive,
            SecondPrimitive& second_primitive) {
          // Variables
          Vec3 support;  // the current support point

          ////////////////////////////////////////////////////////////////////////
          // Initial initialisation step
          supportA_.resize(4);
          supportB_.resize(4);
          simplex_.resize(4);

          // get any first support point
          if (numPoints_ != 0) {
            d_ = normalize(d_);
          }
          supportA_[0] = first_primitive.support_contact_threshold(d_);
          supportB_[0] = second_primitive.support_contact_threshold(-d_);
          support = supportA_[0] - supportB_[0];

          // add this point to the simplex_
          simplex_[0] = support;
          numPoints_ = 1;

          // first real_t search direction is in the opposite direction of the
          // first support point
          d_ = -support;

          /*
             if(trans(support) * d_ < 0.0){
                   //we went as far as we could in direction 'd' but not passed
             the origin
                   //this means the triangle mashes don't overlap
                   //and as the support()-function extends the support point by
             contactThreshold
                   //the mashes are not even close enough to be considered in
             contact. return false;
             }
             */
          ////////////////////////////////////////////////////////////////////////
          // GJK main loop
          while (true) {
            // get the support point in the current search direction
            d_ = normalize(d_);
            supportA_[numPoints_] =
                first_primitive.support_contact_threshold(d_);
            supportB_[numPoints_] =
                second_primitive.support_contact_threshold(-d_);
            support = supportA_[numPoints_] - supportB_[numPoints_];

            // check if "support" is passed the origin in search direction
            if (dot(support, d_) < real_t(0.0)) {
              // we went as far as we could in direction 'd' but not passed the
              // origin this means the triangle mashes don't overlap and as the
              // support()-function extends the support point by
              // contactThreshold the mashes are not even close enough to be
              // considered in contact.
              return false;
            }

            // add the new support point into the simplex
            simplex_[numPoints_] = support;
            numPoints_++;

            ////////////////////////////////////////////////////////////////
            // check if the origin is in the simplex
            // if it is the triangle mashes are overlapping
            switch (numPoints_) {
              case 2: {
                if (simplex2(d_)) {
                  simplex_.pop_back();
                  simplex_.pop_back();
                  supportA_.pop_back();
                  supportA_.pop_back();
                  supportB_.pop_back();
                  supportB_.pop_back();
                  return true;
                }
              } break;

              case 3: {
                if (simplex3(d_)) {
                  simplex_.pop_back();
                  supportA_.pop_back();
                  supportB_.pop_back();
                  return true;
                }
              } break;

              case 4: {
                if (simplex4(d_)) {
                  return true;
                }
              } break;

              default: {
                // std::cerr << "numPoints_="<< numPoints_ <<std::endl;
                ELASTICA_ASSERT(
                    false, "Number of points in the simplex is not 1<=n<=4");
              } break;
            }
          }

          return false;  // never reach this point
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  GET FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline const std::vector<Vec3>& GJK::getSimplex() const noexcept {
          return simplex_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline constexpr std::size_t GJK::getSimplexSize() const noexcept {
          return numPoints_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline constexpr const std::vector<Vec3>& GJK::getSupportA()
            const noexcept {
          return supportA_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline constexpr const std::vector<Vec3>& GJK::getSupportB()
            const noexcept {
          return supportB_;
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  UTILITY FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline bool GJK::simplex2(Vec3& d) {
          // The simplex is a line

          // The Point last added to the simplex
          const Vec3& A = simplex_[1];
          // The Point that was already in the simplex
          const Vec3& B = simplex_[0];
          const Vec3 AO = -A;  // The vector A->O with 0 the origin
          // The transposed vector A->O with O the origin
          const auto AOt = trans(AO);
          const Vec3 AB = B - A;  // The vector A->B

          if (sameDirection(AOt, AB)) {
            // The origin O is in the same direction as B is so the line AB is
            // closest to the origin
            // aka O lies in the voronoi region of edge AB
            //=> keep A and B in the simplex

            // In general (a x b) x c \neq a x (b x c) as seen below
            // https://wikimedia.org/api/rest_v1/media/math/render/svg/021dc51c36f9318dbc7e248194f939addecc6d75
            // in this special case however both are the same and we leave it
            // to the compiler to decide the more optimal one for speed
            d = AB % AO % AB;
          } else {
            // The origin is not in the direction of B seen from A.
            // So O lies in the voronoi region of A
            //=> simplex is just A
            simplex_[0] = A;  // aka simplex_[1]
            supportA_[0] = supportA_[1];
            supportB_[0] = supportB_[1];
            numPoints_ = 1;
            d = AO;
          }

          // if the new search direction has zero length
          // than the origin is on the simplex
          if (zeroLengthVector(d)) {
            d_ = Vec3{0.0, 0.6, 0.8};  // give the GJK a chance to rerun
            return true;
          }
          return false;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline bool GJK::simplex3(Vec3& d) {
          // The simplex is a triangle

          // The Point last added to the simplex
          const Vec3& A = simplex_[2];
          // One Point that was already in the simplex
          const Vec3& B = simplex_[1];
          // One Point that was already in the simplex
          const Vec3& C = simplex_[0];

          // ABC is a conter-clockwise triangle

          // The vector A->O with 0 the origin
          const Vec3 AO = -A;
          // The transposed vector A->O with O the origin
          const auto AOt = trans(AO);
          const Vec3 AB = B - A;     // The vector A->B
          const Vec3 AC = C - A;     // The vector A->C
          const Vec3 ABC = AB % AC;  // The the normal pointing towards the
                                     // viewer if he sees a CCW triangle ABC

          if (sameDirection(AOt, (AB % ABC))) {
            // Origin is on the outside of the triangle of the line AB
            if (dot(AOt, AB) > 0.0) {
              // Origin in the voronoi region of AB outside the triangle
              //=> AB is the new simplex
              simplex_[0] = B;  // aka simplex_[1]
              simplex_[1] = A;  // aka simplex_[2]
              supportA_[0] = supportA_[1];
              supportA_[1] = supportA_[2];
              supportB_[0] = supportB_[1];
              supportB_[1] = supportB_[2];
              numPoints_ = 2;
              d = AB % AO % AB;

            } else {
              // STAR case in Casey's videos
              if (sameDirection(AOt, AC)) {
                // Origin is on a subspace of the voronio region of AC
                //=> AC is the new simplex
                // simplex_[0] = C; //aka simplex_[0] already there
                simplex_[1] = A;  // aka simplex_[2]
                supportA_[1] = supportA_[2];
                supportB_[1] = supportB_[2];
                numPoints_ = 2;
                d = AC % AO % AC;
              } else {
                // Origin is in the voronoi region of A
                //=> A is the new simplex
                simplex_[0] = A;  // aka simplex_[2]
                supportA_[0] = supportA_[2];
                supportB_[0] = supportB_[2];
                numPoints_ = 1;
                d = AO;
              }
            }
          } else {
            if (sameDirection(AOt, (ABC % AC))) {
              // Origin is on the outside of the triangle of the line AC
              // STAR case in Casey's videos
              if (dot(AOt, AC) > 0.0) {
                // Origin is on a subspace of the voronio region of AC
                //=> AC is the new simplex
                // simplex_[0] = C; //aka simplex_[0] already there
                simplex_[1] = A;  // aka simplex_[2]
                supportA_[1] = supportA_[2];
                supportB_[1] = supportB_[2];
                numPoints_ = 2;
                d = AC % AO % AC;
              } else {
                // Origin is in the voronio region of A
                //=> A is the new simplex
                simplex_[0] = A;  // aka simplex_[2]
                supportA_[0] = supportA_[2];
                supportB_[0] = supportB_[2];
                numPoints_ = 1;
                d = AO;
              }
            } else {
              // origin is above or below the triangle ABC but its mapping on
              // the plane ABC lies within ABC
              if (sameDirection(AOt, ABC)) {
                // Origin is above the triangle
                //=>Keep triangle as simplex seen from the origin it is already
                // CCW
                d = ABC;
              } else {
                if (sameDirection(AOt, -ABC)) {
                  // Origin is below the triangle
                  //=>Keep triangle as simplex.
                  // seen from the origin ABC is CW so change the winding
                  // From ABC -> ACB, swap C and B across data structures

                  /*
                  Vec3 temp = B;    // aka simplex_[1]
                  simplex_[1] = C;  // aka simplex_[0]
                  simplex_[0] = temp;

                  // simplex_[2] = A; //aka simplex_[2] already there
                  // old simplex 2:A 1:B 0:C
                  // simplex now contains 2:A 1:C 0:B

                  temp = supportA_[1];
                  supportA_[1] = supportA_[0];
                  supportA_[0] = temp;
                  temp = supportB_[1];
                  supportB_[1] = supportB_[0];
                  supportB_[0] = temp;
                   */

                  using std::swap;
                  swap(simplex_[1], simplex_[0]);
                  swap(supportA_[1], supportA_[0]);
                  swap(supportB_[1], supportB_[0]);

                  d = -ABC;
                } else {
                  // Origin lies in the triangle plane
                  return true;
                }
              }
            }
          }

          // if the new search direction has zero length
          // than the origin is on the boundary of the simplex
          if (zeroLengthVector(d)) {
            d_ = Vec3{0.0, 0.6, 0.8};  // give the GJK a chance to rerun
            return true;
          }
          return false;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline bool GJK::simplex4(Vec3& d) {
          // The simplex is a tetrahedron
          // Three new edges + Three new faces + inside

          /*
           * 1. Check if in front of planes to reject max number of
           * possibilities
           * 2. If behind all faces, only possibility is to lie inside the tet,
           * we're done
           * 3. If in front of one plane, consider only it and its edges =>
           * Triangle case!
           * 4. If in front of two planes, consider both the planes and all its
           * edges (ie. 2 planes, 3 edges => 5 possibilities)
           * 5. If in front of all planes, simplex is very degenerate (probably
           * along the line joining the center of BCD and the line A, in which
           * case we check only the edges and point A
           */

          // The Point last added to the tetrahedron
          const Vec3& A = simplex_[3];
          // t in front mens just a temp varialble
          // One Point that was already in the simplex
          const Vec3& B = simplex_[2];
          // One Point that was already in the simplex
          const Vec3& C = simplex_[1];
          const Vec3& D = simplex_[0];

          // BCD is a clockwise triangle when seen from A

          // The vector A->O with 0 the origin
          const Vec3 AO = -A;
          // The transposed vector A->O with O the origin
          const auto AOt = trans(AO);
          // The vector A->B
          const Vec3 AB = B - A;
          // The vector A->C
          const Vec3 AC = C - A;
          // The vector A-D
          const Vec3 AD = D - A;

          // https://mollyrocket.com/forums/viewtopic.php?p=1829#1829
          unsigned char testWhere = 0;

          // The the normal pointing out of the tetrahedron
          // towards the viewer if he sees a CCW triangle ABC
          const Vec3 ABC = AB % AC;

          // The the normal pointing out of the tetrahedron
          // towards the viewer if he sees a CCW triangle ACD
          const Vec3 ACD = AC % AD;

          // The the normal pointing out of the tetrahedron
          // towards the viewer if he sees a CCW triangle ADB
          const Vec3 ADB = AD % AB;

          constexpr unsigned char overABC = 0x1;
          constexpr unsigned char overACD = 0x2;
          constexpr unsigned char overADB = 0x4;

          if (sameDirection(AOt, ABC)) {
            // set the first bit of testWhere to 1:
            testWhere |= overABC;
          }

          if (sameDirection(AOt, ACD)) {
            // set the second bit of testWhere to 1:
            testWhere |= overACD;
          }

          if (sameDirection(AOt, ADB)) {
            // set the third bit of testWhere to 1:
            testWhere |= overADB;
          }

          switch (testWhere) {
            case 0: {
              // origin is in the tetrahedro
              //=> the two triangle mashes overlap
              // std::cout << "Origin is within the tetrahedron\nA=" << A << "
              // B=" << B << " C="<< C << " D="<< D << std::endl;
              return true;
            } break;

            case overABC: {
              // In front of ABC only
              // Origin is outside the tetrahedron above ABC
              //=> rearrange simplex to use the triangle case
              // while chucking D out
              simplex_[0] = C;  // aka simplex_[1] 0:C
              simplex_[1] = B;  // aka simplex_[2] 1:B
              simplex_[2] = A;  // aka simplex_[3] 2:A

              supportA_[0] = supportA_[1];
              supportA_[1] = supportA_[2];
              supportA_[2] = supportA_[3];
              supportB_[0] = supportB_[1];
              supportB_[1] = supportB_[2];
              supportB_[2] = supportB_[3];

              numPoints_ = 3;

              return simplex3(d);
            } break;

            case overACD: {
              // In front of ACD only
              // Origin is outside the tetrahedron above ACD
              //=> rearrange simplex to use the triangle case
              // while chucking B out

              // simplex_[0] = D; //aka simplex_[0] 0:D already the case
              // simplex_[1] = C; //aka simplex_[1] 1:C already the case
              simplex_[2] = A;  // aka simplex_[3] 2:A

              supportA_[2] = supportA_[3];
              supportB_[2] = supportB_[3];

              numPoints_ = 3;

              return simplex3(d);
            } break;

            case overADB: {
              // In front of ADB only
              // Origin is outside the tetrahedron above ADB
              //=> rearrange simplex to use the triangle case
              // while chucking C out
              // The origin however sees a CW winded triangle
              // ABD, so we change the winding to make it ADB
              // by swapping D and B
              simplex_[1] = D;  // aka simplex_[0] 1:D
              simplex_[0] = B;  // aka simplex_[2] 0:B already there
              simplex_[2] = A;  // aka simplex_[3] 2:A

              supportA_[1] = supportA_[0];
              supportA_[0] = supportA_[2];
              supportA_[2] = supportA_[3];
              supportB_[1] = supportB_[0];
              supportB_[0] = supportB_[2];
              supportB_[2] = supportB_[3];

              numPoints_ = 3;

              return simplex3(d);
            } break;

            case (overABC | overACD): {
              // In front of ABC and ACD

              // Can be in the voronoi regino of the edge AC
              // point

              if (sameDirection(AOt, ABC % AC)) {
                // Origin is beyond AC from ABC's persepective
                // That means it cannot lie in the voronoi
                // region corresponding to face ABC
                // hence check only for face ACD, edge AC,
                // edge AD or point A

                if (sameDirection(AOt, AC % ACD)) {
                  // Origin is beyond AC from ACD's perspective
                  // so it cannot lie in the voronoi regino of
                  // ACD, so check only edge AC or point A,

                  if (sameDirection(AOt, AC)) {
                    // AddEdgeSimplex(A, C);
                    simplex_[0] = C;  // aka simplex_[1] 0:C
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[0] = supportA_[1];
                    supportA_[1] = supportA_[3];
                    supportB_[0] = supportB_[1];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AC % AO % AC;
                  } else {
                    // AddPointSimplex;
                    simplex_[0] = A;  // aka simplex_[3] 0:A

                    supportA_[0] = supportA_[3];
                    supportB_[0] = supportB_[3];

                    numPoints_ = 1;
                    d = AO;
                  }
                } else {
                  // Origin is not beyond AC from ACD's perspective
                  // so it can lie in the voronoi region of ACD or
                  // in the edge AD
                  // so check edge AD or face ACD,

                  if (sameDirection(AOt, ACD % AD)) {
                    // AddEdgeSimplex(A, D);
                    // simplex_[0] = D; //aka simplex_[0] 0:D already there
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[1] = supportA_[3];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AD % AO % AD;
                  } else {
                    // AddTriangleSimplex(A, C, D);
                    // simplex_[0] = D; //aka simplex_[0] 0:D already there
                    // simplex_[1] = C; //aka simplex_[1] 1:C already there
                    simplex_[2] = A;  // aka simplex_[3] 2:A

                    supportA_[2] = supportA_[3];
                    supportB_[2] = supportB_[3];

                    numPoints_ = 3;
                    d = ACD;
                  }
                }
              } else {
                // Origin is not beyond AC from ABC's persepective
                // That means it can lie in the voronoi
                // region corresponding to face ABC or even edge AB
                // or point A

                if (sameDirection(AOt, AB % ABC)) {
                  // Origin beyond AB from ABC's perspective
                  // the only possible cases are edge AB and point A

                  if (sameDirection(AOt, AB)) {
                    // AddEdgeSimplex(A, B);
                    simplex_[0] = B;  // aka simplex_[2] 0:B
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[0] = supportA_[2];
                    supportA_[1] = supportA_[3];
                    supportB_[0] = supportB_[2];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AB % AO % AB;
                  } else {
                    // AddPointSimplex;
                    simplex_[0] = A;  // aka simplex_[3] 0:A

                    supportA_[0] = supportA_[3];
                    supportB_[0] = supportB_[3];

                    numPoints_ = 1;
                    d = AO;
                  }
                } else {
                  // Origin not beyond AB from ABC's perspective
                  // so add face ABC

                  // AddTriangleSimplex(A, B, C);
                  simplex_[0] = C;  // aka simplex_[1] 0:C
                  simplex_[1] = B;  // aka simplex_[2] 1:B
                  simplex_[2] = A;  // aka simplex_[3] 2:A

                  supportA_[0] = supportA_[1];
                  supportA_[1] = supportA_[2];
                  supportA_[2] = supportA_[3];
                  supportB_[0] = supportB_[1];
                  supportB_[1] = supportB_[2];
                  supportB_[2] = supportB_[3];

                  numPoints_ = 3;
                  d = ABC;
                }
              }
            } break;

              // If one understands the case above, this is very similar
            case (overABC | overADB): {
              // In front of ADB and ABC
              if (sameDirection(AOt, ADB % AB)) {
                if (sameDirection(AOt, AB % ABC)) {
                  if (sameDirection(AOt, AB)) {
                    // AddEdgeSimplex(A, B);
                    simplex_[0] = B;  // aka simplex_[2] 0:B
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[0] = supportA_[2];
                    supportA_[1] = supportA_[3];
                    supportB_[0] = supportB_[2];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AB % AO % AB;
                  } else {
                    // AddPointSimplex;
                    simplex_[0] = A;  // aka simplex_[3] 0:A

                    supportA_[0] = supportA_[3];
                    supportB_[0] = supportB_[3];

                    numPoints_ = 1;
                    d = AO;
                  }
                } else {
                  if (sameDirection(AOt, ABC % AC)) {
                    // AddEdgeSimplex(A, C);
                    simplex_[0] = C;  // aka simplex_[1] 0:C
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[0] = supportA_[1];
                    supportA_[1] = supportA_[3];
                    supportB_[0] = supportB_[1];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AC % AO % AC;
                  } else {
                    // AddTriangleSimplex(A, B, C);
                    simplex_[0] = C;  // aka simplex_[1] 0:C
                    simplex_[1] = B;  // aka simplex_[2] 1:B
                    simplex_[2] = A;  // aka simplex_[3] 2:A

                    supportA_[0] = supportA_[1];
                    supportA_[1] = supportA_[2];
                    supportA_[2] = supportA_[3];
                    supportB_[0] = supportB_[1];
                    supportB_[1] = supportB_[2];
                    supportB_[2] = supportB_[3];

                    numPoints_ = 3;
                    d = ABC;
                  }
                }
              } else {
                if (sameDirection(AOt, AD % ADB)) {
                  if (sameDirection(AOt, AD)) {
                    // AddEdgeSimplex(A, D);
                    // simplex_[0] = D; //aka simplex_[0] 0:D already there
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[1] = supportA_[3];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AD % AO % AD;
                  } else {
                    // AddPointSimplex;
                    simplex_[0] = A;  // aka simplex_[3] 0:A

                    supportA_[0] = supportA_[3];
                    supportB_[0] = supportB_[3];

                    numPoints_ = 1;
                    d = AO;
                  }
                } else {
                  // AddTriangleSimplex(A, D, B);
                  // Rewind as seen from origin => swap D and B position
                  simplex_[1] = D;  // aka simplex[0] 1:D
                  simplex_[0] = B;  // aka simplex[2] 0:B
                  simplex_[2] = A;  // aka simplex[3] 2:A

                  supportA_[1] = supportA_[0];
                  supportA_[0] = supportA_[2];
                  supportA_[2] = supportA_[3];
                  supportB_[1] = supportB_[0];
                  supportB_[0] = supportB_[2];
                  supportB_[2] = supportB_[3];

                  numPoints_ = 3;

                  numPoints_ = 3;
                  d = ADB;
                }
              }
            } break;

            case (overACD | overADB): {
              // In front of ACD and ADB
              if (sameDirection(AOt, ACD % AD)) {
                if (sameDirection(AOt, AD % ADB)) {
                  if (sameDirection(AOt, AD)) {
                    // AddEdgeSimplex(A, D);
                    // simplex_[0] = D; //aka simplex_[0] 0:D already there
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[1] = supportA_[3];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AD % AO % AD;
                  } else {
                    // AddPointSimplex;
                    simplex_[0] = A;  // aka simplex_[3] 0:A

                    supportA_[0] = supportA_[3];
                    supportB_[0] = supportB_[3];

                    numPoints_ = 1;
                    d = AO;
                  }
                } else {
                  if (sameDirection(AOt, ADB % AB)) {
                    // AddEdgeSimplex(A, B);
                    simplex_[0] = B;  // aka simplex_[2] 0:B
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[0] = supportA_[2];
                    supportA_[1] = supportA_[3];
                    supportB_[0] = supportB_[2];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AB % AO % AB;
                  } else {
                    // AddTriangleSimplex(A, D, B);
                    simplex_[1] = D;  // aka simplex[0] 1:D
                    simplex_[0] = B;  // aka simplex[2] 0:B
                    simplex_[2] = A;  // aka simplex[3] 2:A

                    supportA_[1] = supportA_[0];
                    supportA_[0] = supportA_[2];
                    supportA_[2] = supportA_[3];
                    supportB_[1] = supportB_[0];
                    supportB_[0] = supportB_[2];
                    supportB_[2] = supportB_[3];

                    numPoints_ = 3;

                    // numPoints_ = 3;
                    d = ADB;
                  }
                }
              } else {
                if (sameDirection(AOt, AC % ACD)) {
                  if (sameDirection(AOt, AC)) {
                    // AddEdgeSimplex(A, C);
                    simplex_[0] = C;  // aka simplex_[1] 0:C
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[0] = supportA_[1];
                    supportA_[1] = supportA_[3];
                    supportB_[0] = supportB_[1];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AC % AO % AC;
                  } else {
                    // AddPointSimplex;
                    simplex_[0] = A;  // aka simplex_[3] 0:A

                    supportA_[0] = supportA_[3];
                    supportB_[0] = supportB_[3];

                    numPoints_ = 1;
                    d = AO;
                  }
                } else {
                  // AddTriangleSimplex(A, C, D);
                  // simplex_[0] = D; //aka simplex_[0] 0:D already there
                  // simplex_[1] = C; //aka simplex_[1] 1:C already there
                  simplex_[2] = A;  // aka simplex_[3] 2:A

                  supportA_[2] = supportA_[3];
                  supportB_[2] = supportB_[3];

                  numPoints_ = 3;
                  d = ACD;
                }
              }
            } break;

            case (overABC | overACD | overADB): {
              // In front of ABC, ACD and ADB
              if (sameDirection(AOt, AB)) {
                simplex_[0] = B;  // aka simplex_[2] 0:B
                simplex_[1] = A;  // aka simplex_[3] 1:A

                supportA_[0] = supportA_[2];
                supportA_[1] = supportA_[3];
                supportB_[0] = supportB_[2];
                supportB_[1] = supportB_[3];

                numPoints_ = 2;
                d = AB % AO % AB;
              } else {
                if (sameDirection(AOt, AC)) {
                  simplex_[0] = C;  // aka simplex_[1] 0:C
                  simplex_[1] = A;  // aka simplex_[3] 1:A

                  supportA_[0] = supportA_[1];
                  supportA_[1] = supportA_[3];
                  supportB_[0] = supportB_[1];
                  supportB_[1] = supportB_[3];

                  numPoints_ = 2;
                  d = AC % AO % AC;

                } else {
                  if (sameDirection(AOt, AD)) {
                    // simplex_[0] = D; //aka simplex_[1] 0:D already there
                    simplex_[1] = A;  // aka simplex_[3] 1:A

                    supportA_[1] = supportA_[3];
                    supportB_[1] = supportB_[3];

                    numPoints_ = 2;
                    d = AD % AO % AD;
                  } else {
                    simplex_[0] = A;  // aka simplex_[3] 0:A

                    supportA_[0] = supportA_[3];
                    supportB_[0] = supportB_[3];

                    numPoints_ = 1;
                    d = AO;
                  }
                }
              }
            } break;
            default: {
              // all 8 cases 0-7 are covered
            } break;
          }

          return false;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         *
         * Checks if two vectors roughly point in the same direction
         */
        template <typename Vec3T>
        inline bool GJK::sameDirection(const Vec3T& vec1,
                                       const Vec3& vec2) const noexcept {
          return dot(vec1, vec2) > real_t(0.0);
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         *
         * Checks if the length of a vector is zero or as close to zero that it
         * can not be distinguished form zero
         */
        inline bool GJK::zeroLengthVector(const Vec3& vec) const noexcept {
          //          return vec.sqrLength() < Limits<real_t>::fpuAccuracy();
          return sqrLength(vec) < accuracy;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline real_t GJK::calcDistance(Vec3& normal, Vec3& contact_point) {
          // find the point in the simplex closest to the origin#
          // its distance to the origin is the distance of the two objects
          real_t dist_squared = 0.0;

          real_t barCoords[3] = {0.0, 0.0, 0.0};
          real_t& u = barCoords[0];
          real_t& v = barCoords[1];
          real_t& w = barCoords[2];

          Vec3& A = simplex_[0];
          Vec3& B = simplex_[1];
          Vec3& C = simplex_[2];

          switch (numPoints_) {
            case 1: {
              // the only point in simplex is closest to Origin
              dist_squared = sqrLength(A);
              u = 1.0;
              break;
            }
            case 2: {
              // calc distance Origin to line segment
              // it is definitively closest do the segment not to one of the end
              // points as the voronoi regions of the points also consist of the
              // border borderline between point region and segment region
              // compare "Real-Time Collision Detection" by Christer Ericson
              // page 129ff
              Vec3 ab = B - A;
              // Vec3 ac = -A;
              // Vec3 bc = -simplex[1];

              // calc baryzenctric coordinats
              // compare "Real-Time Collision Detection" by Christer Ericson
              // page 129
              // double t = trans(ac)*ab;
              real_t t = -dot(A, ab);
              real_t denom = sqrLength(ab);
              u = t / denom;
              v = 1.0 - u;

              // compare "Real-Time Collision Detection" by Christer Ericson
              // page 130
              // double& e = t;
              // double& f = denom;
              // dist = ac.sqrLength() -  e*e/f;
              dist_squared = sqrLength(A) - t * u;

              break;
            }
            case 3: {
              // origin is surly in the voronoi region of the face itself
              // not the bordering lines or one of the 3 points
              // to be more precise it is also in normal direction.
              // compare "Real-Time Collision Detection" by Christer Ericson
              // page 139

              // Vec3 ab= B - A;
              // Vec3 ac= C - A;
              // Vec3 bc= C - B;
              Vec3& n = d_;  // we already know the normal
              auto nT = trans(n);

              real_t vc = dot(nT, (A % B));
              real_t va = dot(nT, (B % C));
              real_t vb = dot(nT, (C % A));
              real_t denom = 1.0 / (va + vb + vc);
              u = va * denom;
              v = vb * denom;
              w = 1.0 - u - v;

              Vec3 closestPoint = u * A + v * B + w * C;
              dist_squared = sqrLength(closestPoint);

              break;
            }
            default: {
              // Source to source translation!
              std::cout << "wrong number of points in the simplex" << std::endl;
              break;
            }
          }

          Vec3 pointOnA(u * supportA_[0]);
          Vec3 pointOnB(u * supportB_[0]);
          for (unsigned short i = 1; i < numPoints_; ++i) {
            pointOnA += barCoords[i] * supportA_[i];
            pointOnB += barCoords[i] * supportB_[i];
          }

          normal = normalize(pointOnA - pointOnB);
          contact_point = (pointOnA + pointOnB) * 0.5;

          // Note : while one needs to return the std::sqrt() of this guy,
          // in practice we use EPA to find the exact distance and hence
          // this is usually a upper bound (?!) to the actual distance,
          // which means we can ignore the sqrt here for performance
          return (dist_squared);
        }
        //*************************************************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
