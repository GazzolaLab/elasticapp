//=================================================================================================
/*!
 *  \file pe/core/detection/fine/EPA.h
 *  \brief The Expanding-Polytope Algorithm
 *
 *  Copyright (C) 2001-2003 Dtecta
 *                2013-2014 Tobias Scharpff
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
 *
 *  DISCLAIMER: The following source file contains modified code from the
 * SOLID-3.5 library for interference detection as it is published in the book
 * "Collision Detection in Interactive 3D Environments" by Gino van den Bergen
 * <info@dtecta.com>. Even though the original source was published under the
 * GPL version 2 not allowing later versions, the original author of the source
 * code permitted the relicensing of the SOLID-3.5 library under the GPL version
 * 3 license.
 */
//=================================================================================================

#pragma once

//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <limits>
#include <vector>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Fine/GJK.hpp"
#include "elastica/Simulator/Thresholds.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Math/Accuracy.hpp"
#include "elastica/Utilities/Math/Dot.hpp"
#include "elastica/Utilities/Math/Length.hpp"
#include "elastica/Utilities/Math/Normalize.hpp"
#include "elastica/Utilities/Math/Rot3.hpp"
#include "elastica/Utilities/Math/Rotations.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Trans.hpp"

//#include <pe/core/Thresholds.h>
//#include <pe/core/detection/fine/GJK.h>
//#include <pe/math/Quaternion.h>
//#include <pe/math/RotationMatrix.h>

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief The Expanding-Polytope Algorithm.
         * \ingroup fine_collision_detection
         */
        class EPA {
         private:
          //**Type definitions**************************************************
          class EPAEdge;
          class EPATriangle;
          class EPATriangleCompare;

          using EPAEntryBuffer = std::vector<EPATriangle>;
          using EPAEntryHeap = std::vector<EPATriangle*>;
          using EPAEdgeBuffer = std::vector<EPAEdge>;
          //********************************************************************

         public:
          //**Query functions***************************************************
          /*!\name Query functions */
          //@{
          template <typename FirstPrimitive, typename SecondPrimitive>
          inline bool do_contact_threshold(FirstPrimitive& first_primitive,
                                           SecondPrimitive& second_primitive,
                                           const GJK& gjk, Vec3& normal,
                                           Vec3& contactPoint,
                                           real_t& penetrationDepth);

         private:
          //**Utility functions*************************************************
          /*!\name Utility functions */
          //@{
          inline bool originInTetrahedron(const Vec3& A, const Vec3& B,
                                          const Vec3& C, const Vec3& D);
          inline bool originInTetrahedronVolumeMethod(const Vec3& A,
                                                      const Vec3& B,
                                                      const Vec3& C,
                                                      const Vec3& D);
          inline bool pointInTetrahedron(const Vec3& A, const Vec3& B,
                                         const Vec3& C, const Vec3& D,
                                         const Vec3& point);
          inline void createInitialTetrahedron(std::size_t top,
                                               std::size_t frontLeft,
                                               std::size_t frontRight,
                                               std::size_t back,
                                               std::vector<Vec3>& epaVolume,
                                               EPAEntryBuffer& entryBuffer);
          template <typename FirstPrimitive, typename SecondPrimitive>
          inline void createInitialSimplex(unsigned char numPoints,
                                           FirstPrimitive& first_primitive,
                                           SecondPrimitive& second_primitive,
                                           std::vector<Vec3>& supportA,
                                           std::vector<Vec3>& supportB,
                                           std::vector<Vec3>& epaVolume,
                                           EPAEntryBuffer& entryBuffer);
          //@}
          //********************************************************************

         private:
          // EPA constants
          static constexpr std::size_t maxSupportPoints_ = 100UL;
          static constexpr std::size_t maxTriangles_ = 200UL;
        };
        //**********************************************************************

        //=================================================================================================
        //
        //  EPA::EPA_EDGE CLASS DEFINITION
        //
        //=================================================================================================

        //**********************************************************************
        /*!\brief TODO
         */
        class EPA::EPAEdge {
         public:
          //**Constructor*******************************************************
          /*!\name Constructor */
          //@{
          EPAEdge(EPATriangle* triangle, unsigned char index);
          //@}
          //********************************************************************

          //**Get functions*****************************************************
          /*!\name Get functions */
          //@{
          EPATriangle* getTriangle() const;
          unsigned char getIndex() const;
          std::size_t getStart() const;
          std::size_t getEnd() const;
          //@}
          //********************************************************************

         private:
          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          //!< the EPA triangle the edge is contained in
          EPATriangle* triangle_;
          unsigned char startIdx_;  //!< the index of the point the edge starts
                                    //!< at (0, 1, 2)
                                    //@}
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  EPA::EPA_TRIANGLE CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief TODO
         *
         * see Collision detection in interactive 3D environments; Gino van den
         * bergen page 155
         */
        class EPA::EPATriangle {
         public:
          //**Constructor*******************************************************
          /*!\name Constructor */
          //@{
          explicit inline EPATriangle(std::size_t a, std::size_t b,
                                      std::size_t c,
                                      const std::vector<Vec3>& points);
          //@}
          //********************************************************************

          //**Get functions*****************************************************
          /*!\name Get functions */
          //@{
          inline std::size_t operator[](unsigned char i) const;
          inline const Vec3& getClosest() const;
          inline const Vec3& getNormal() const;
          inline Vec3 getClosestPoint(const std::vector<Vec3>& points) const;
          inline real_t getSqrDist() const;
          inline bool isObsolete() const;
          inline bool isClosestInternal() const;
          //@}
          //********************************************************************

          //**Utility functions*************************************************
          /*!\name Utility functions */
          //@{
          // TODO : free function
          inline bool link(unsigned char edge0, EPATriangle* tria,
                           unsigned char edge1);
          inline void silhouette(const Vec3& w, EPAEdgeBuffer& edgeBuffer);
          //@}
          //********************************************************************

         private:
          //**Utility functions*************************************************
          /*!\name Utility functions */
          //@{
          inline void silhouette(unsigned char index, const Vec3& w,
                                 EPAEdgeBuffer& edgeBuffer);
          //@}
          //********************************************************************

          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          std::size_t indices_[3];  //!< indices of the vertices of the triangle
          bool obsolete_;  //!< flag to denote whether die triangle is visible
                           //!< from the new support point

          Vec3 closest_;    //!< the point closest to the origin of the affine
                            //!< hull of the triangle
          Vec3 normal_;     //!< normal pointing away from the origin
          real_t bar_[3];   //!< the barycentric coordinate of closest_
          real_t sqrDist_;  //!< =key; square distance of closest_ to the origin

          EPATriangle* adjTriangle_[3];  //!< pointer to the triangle adjacent
                                         //!< to edge i(=0,1,2)
          unsigned char
              adjEdges_[3];  //!< for each adjoining triangle adjTriangle_[i],
                             //!< the index of the adjoining edge
                             //@}
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  EPA::EPA_TRIANGLECOMP CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief TODO
         *
         * Compare class to sort the triangle heap
         */
        class EPA::EPATriangleCompare {
         public:
          //**Binary function call
          // operator***************************************************************
          /*!\name Binary function call operator */
          //@{
          inline bool operator()(const EPATriangle* tria1,
                                 const EPATriangle* tria2);
          //@}
          //**********************************************************************************************
        };
        //*************************************************************************************************

        //=================================================================================================
        //
        //  EPA_EDGE CONSTRUCTOR
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline EPA::EPAEdge::EPAEdge(EPATriangle* triangle, unsigned char index)
            : triangle_(triangle), startIdx_(index) {}
        //*************************************************************************************************

        //=================================================================================================
        //
        //  EPA_EDGE GET FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline EPA::EPATriangle* EPA::EPAEdge::getTriangle() const {
          return triangle_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline unsigned char EPA::EPAEdge::getIndex() const {
          return startIdx_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         * needs to be defined after the EPATriangle class
         */
        inline std::size_t EPA::EPAEdge::getStart() const {
          return (*triangle_)[startIdx_];
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline std::size_t EPA::EPAEdge::getEnd() const {
          return (*triangle_)[(startIdx_ + 1) % 3];
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  EPA::EPA_TRIANGLE CONSTRUCTOR
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline EPA::EPATriangle::EPATriangle(std::size_t a, std::size_t b,
                                             std::size_t c,
                                             const std::vector<Vec3>& points) {
          // TODO No assert in constructor
          ELASTICA_ASSERT(a != b && b != c && c != a,
                          "EPATriangle impossible indices");

          const Vec3& A = points[a];
          const Vec3& B = points[b];
          const Vec3& C = points[c];

          indices_[0] = a;
          indices_[1] = b;
          indices_[2] = c;

          // calculate the closest point to the origin
          // Real-Time Collsion pp 137
          Vec3 AB(B - A);
          Vec3 AC(C - A);
          // Vec3 bc = C-B;

          normal_ = AB % AC;
          auto nT = trans(normal_);

          // We pick the closest face of the polytope to the origin.
          // when constructing this triangle
          // While its not guaranteed that this is the case, while
          // iterating through the EntryBuffer, we check if the closest
          // point is internal or not, so this operation suffices
          real_t vc(dot(nT, A % B));
          real_t va(dot(nT, B % C));
          real_t vb(dot(nT, C % A));
          real_t denom(1.0 / (va + vb + vc));

          bar_[0] = va * denom;
          bar_[1] = vb * denom;
          bar_[2] = 1.0 - bar_[0] - bar_[1];

          closest_ = bar_[0] * A + bar_[1] * B + bar_[2] * C;

          // sqrDist=key is square distance of v to origin
          sqrDist_ = sqrLength(closest_);

          // adjoined triangles not set yet
          adjTriangle_[0] = adjTriangle_[1] = adjTriangle_[2] = nullptr;
          adjEdges_[0] = adjEdges_[1] = adjEdges_[2] = 4;

          obsolete_ = false;
        }

        //=================================================================================================
        //
        //  EPA::EPA_TRIANGLE GET FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*! \brief Returns the index of the internal vertex i(=0,1,2) within the
         * EPA scope.
         */
        inline std::size_t EPA::EPATriangle::operator[](unsigned char i) const {
          return indices_[i];
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! \brief Returns the point closest to the origin of the affine hull of
         * the triangle, which is also the normal.
         */
        inline const Vec3& EPA::EPATriangle::getClosest() const {
          return closest_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! \brief Returns the normal of the triangle. Normal is not normalized!
         */
        inline const Vec3& EPA::EPATriangle::getNormal() const {
          return normal_;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! \brief Calculates the corresponding closest point form the given
         * points, using barycentric coordinates.
         */
        inline Vec3 EPA::EPATriangle::getClosestPoint(
            const std::vector<Vec3>& points) const {
          return bar_[0] * points[indices_[0]] + bar_[1] * points[indices_[1]] +
                 bar_[2] * points[indices_[2]];
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! \brief Returns the squared distance to the closest to the origin of
         * the affine hull of the triangle.
         */
        inline real_t EPA::EPATriangle::getSqrDist() const { return sqrDist_; }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! \brief Returns true if the triangle is no longer part of the EPA
         * polygon.
         */
        inline bool EPA::EPATriangle::isObsolete() const { return obsolete_; }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! Returns true if the point closest to the origin of the affine hull
         * of the triangle, lies inside the triangle.
         */
        inline bool EPA::EPATriangle::isClosestInternal() const {
          return bar_[0] >= real_t(0.0) && bar_[1] >= real_t(0.0) &&
                 bar_[2] >= real_t(0.0);
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  EPA::EPA_TRIANGLE UTILITY FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*! \brief TODO
         */
        inline bool EPA::EPATriangle::link(unsigned char edge0,
                                           EPATriangle* tria,
                                           unsigned char edge1) {
          ELASTICA_ASSERT(edge0 < 3 && edge1 < 3, "link: invalid edge index");

          adjTriangle_[edge0] = tria;
          adjEdges_[edge0] = edge1;
          tria->adjTriangle_[edge1] = this;
          tria->adjEdges_[edge1] = edge0;
          /*
          std::size_t v1 = indices_[edge0] ;
          std::size_t v2 = tria->indices_[(edge1+1)%3];
          std::size_t v3 = indices_[(edge0+1)%3];
          std::size_t v4 = tria->indices_[edge1];

          if( !(v1==v2 && v3==v4)) {
             std::cout << "kaputt" << std::endl;
          }
          */

          bool b = indices_[edge0] == tria->indices_[(edge1 + 1) % 3] &&
                   indices_[(edge0 + 1) % 3] == tria->indices_[edge1];
          return b;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! \brief Fills edgeBuffer with the CCW contour of triangles not seen
         * from point w which is in normal direction of the triangle.
         */
        inline void EPA::EPATriangle::silhouette(const Vec3& w,
                                                 EPAEdgeBuffer& edgeBuffer) {
          edgeBuffer.clear();
          obsolete_ = true;

          adjTriangle_[0]->silhouette(adjEdges_[0], w, edgeBuffer);
          adjTriangle_[1]->silhouette(adjEdges_[1], w, edgeBuffer);
          adjTriangle_[2]->silhouette(adjEdges_[2], w, edgeBuffer);
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*! \brief TODO
         */
        inline void EPA::EPATriangle::silhouette(unsigned char index,
                                                 const Vec3& w,
                                                 EPAEdgeBuffer& edgeBuffer) {
          if (!obsolete_) {
            // This tests if the triangle can be "seen" from the newly computed
            // support point w. aka it checks for
            // (triangle-normal (dot) (support_point –
            // closest_point_on_triangle) > 0)
            //
            // closest_point is closest_point . \hat{n}, i.e. its along normal
            // (Note its along +\hat{n} because the simplex contains the origin)

            // so replace triangle normal with closest point above
            // =>  close (dot) (support - close) > 0
            // =>  close (dot) support > close (dot) close
            // for the triangle to be visible

            real_t test(dot(closest_, w));
            // TODO
            // if ((trans(closest_) * w) < sqrDist_) {
            if (test < sqrDist_) {
              // Facet not visible
              // emplace an EPAEdge
              edgeBuffer.emplace_back(this, index);
            } else {
              // Facet is visible
              obsolete_ = true;
              int next = (index + 1) % 3;
              adjTriangle_[next]->silhouette(adjEdges_[next], w, edgeBuffer);
              next = (next + 1) % 3;
              adjTriangle_[next]->silhouette(adjEdges_[next], w, edgeBuffer);
            }
          }
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  EPA::EPA_TRIANGLECOMP BINARY FUNCTION CALL OPERATOR
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline bool EPA::EPATriangleCompare::operator()(
            const EPATriangle* tria1, const EPATriangle* tria2) {
          return tria1->getSqrDist() > tria2->getSqrDist();
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  EPA QUERY FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */

        template <typename FirstPrimitive,   // Type of the first primitive
                  typename SecondPrimitive>  // Type of the second primitive
        inline bool EPA::do_contact_threshold(FirstPrimitive& first_primitive,
                                              SecondPrimitive& second_primitive,
                                              const GJK& gjk, Vec3& ret_normal,
                                              Vec3& contactPoint,
                                              real_t& penetrationDepth) {
          // have in mind that we use a support mapping which blows up the
          // objects a wee bit so zero penetraion aka toching contact means that
          // the original bodies have a distance of 2*contactTrashold between
          // them

          // Set references to the results of GJK
          unsigned char numPoints(gjk.getSimplexSize());
          std::vector<Vec3> epaVolume(gjk.getSimplex());
          std::vector<Vec3> supportA(gjk.getSupportA());
          std::vector<Vec3> supportB(gjk.getSupportB());

          // Setup all the inicial valus
          Vec3 support;

          static constexpr EPATriangleCompare triangle_comp;

          epaVolume.reserve(maxSupportPoints_);
          supportA.reserve(maxSupportPoints_);
          supportB.reserve(maxSupportPoints_);

          EPAEntryBuffer entryBuffer;
          entryBuffer.reserve(maxTriangles_);

          EPAEntryHeap entryHeap;
          entryHeap.reserve(maxTriangles_);

          EPAEdgeBuffer edgeBuffer;
          edgeBuffer.reserve(20);

          real_t lowerBoundSqr = 0.0;
          // mu in 165, alg 4.10
          real_t upperBoundSqr = std::numeric_limits<real_t>::infinity();

          auto rightDirechtionT = trans(second_primitive.get_position() -
                                        first_primitive.get_position());

          // create an Initial simplex
          if (numPoints == 1) {
            // Bodys are in contact, as they are grown by contactThreshold
            // within the support function they can be considered not
            // intersecting
            return false;
          } else {
            createInitialSimplex<FirstPrimitive, SecondPrimitive>(
                numPoints, first_primitive, second_primitive, supportA,
                supportB, epaVolume, entryBuffer);
          }

          for (auto& entry : entryBuffer) {
            if (entry.isClosestInternal()) {
              // triangle is candidate
              if (entry.getSqrDist() < accuracy) {
                // triangle contains the origin so we have zero penetration and
                // touching contact.
                return false;
              }

              // check if the triangle is in the general right
              // direction
              if (dot(rightDirechtionT, entry.getClosest()) > 0) {
                entryHeap.push_back(&entry);
              }
            }
          }

          if (entryHeap.empty()) {
            // for some reason all triangles have no internal closest point
            // std::cerr << "for some reason all triangles have no internal
            // closest point"<<std::endl;
            return false;
          }

          std::make_heap(entryHeap.begin(), entryHeap.end(), triangle_comp);

          EPATriangle* current = nullptr;
          do {
            current = entryHeap[0];
            std::pop_heap(entryHeap.begin(), entryHeap.end(), triangle_comp);
            entryHeap.pop_back();  // TODO might be better to keep track of the
                                   // end and not pop the last element but just
                                   // overwrite it if needed

            if (!current->isObsolete()) {
              ELASTICA_ASSERT(current->getSqrDist() > real_t(0.0),
                              "EPA_Triangle distance is negative, whereas it "
                              "must be positive");
              lowerBoundSqr = current->getSqrDist();

              if (epaVolume.size() == maxSupportPoints_) {
                ELASTICA_ASSERT(false, "EPA no convergece");
                // TODO wie soll ich das behandeln.
                return false;
              }

              Vec3 normal = normalize(current->getClosest());

              supportA.push_back(
                  first_primitive.supportContactThreshold(normal));
              supportB.push_back(
                  second_primitive.supportContactThreshold(-normal));
              support = supportA.back() - supportB.back();
              epaVolume.push_back(support);

              real_t farDist =
                  dot(support, current->getClosest());  // not yet squared

              ELASTICA_ASSERT(farDist > real_t(0.0),
                              "EPA support mapping gave invalid point in "
                              "expansion direction");

              upperBoundSqr =
                  std::min(upperBoundSqr, farDist * farDist / lowerBoundSqr);

              // terminating criteria's
              //- we found that the two bounds are close enough
              //- the added support point was already in the epaVolumn
              if (upperBoundSqr <=
                      1.0006 * lowerBoundSqr  // TODO remove magic number,
                                              //  eps_rel in Gino's book
                  || support == epaVolume[(*current)[0]] ||
                  support == epaVolume[(*current)[1]] ||
                  support == epaVolume[(*current)[2]]) {
                break;
              }

              // Compute the silhouette cast by the new vertex
              // Note that the new vertex is on the positive side
              // of the current triangle, so the current triangle
              // will not be in the convex hull. Start local search
              // from this facet.
              current->silhouette(support, edgeBuffer);

              // At least one triangle will be removed, so the number
              // of edges have to be at least 3
              if (edgeBuffer.size() < 3) {
                return false;
              }

              // "out of memory" so stop here
              if (entryBuffer.size() == maxSupportPoints_) {
                break;
              }

              EPAEdgeBuffer::const_iterator it = edgeBuffer.begin();
              // emplace an EPA triangle
              entryBuffer.emplace_back(it->getEnd(), it->getStart(),
                                       epaVolume.size() - 1, epaVolume);

              EPATriangle* firstTriangle = &(entryBuffer.back());

              // if it is expanding candidate add to heap
              if (firstTriangle->isClosestInternal() &&
                  firstTriangle->getSqrDist() > lowerBoundSqr &&
                  firstTriangle->getSqrDist() < upperBoundSqr) {
                entryHeap.push_back(firstTriangle);
                std::push_heap(entryHeap.begin(), entryHeap.end(),
                               triangle_comp);
              }

              // WARN : I don't understand this linking business, fuck GINO.
              firstTriangle->link(0, it->getTriangle(), it->getIndex());

              EPATriangle* lastTriangle = firstTriangle;

              ++it;
              for (; it != edgeBuffer.end(); ++it) {
                //"out of memory" so stop here
                if (entryBuffer.size() == maxSupportPoints_) {
                  break;
                }

                // emplace an EPA triangle
                entryBuffer.emplace_back(it->getEnd(), it->getStart(),
                                         epaVolume.size() - 1, epaVolume);

                EPATriangle* newTriangle = &(entryBuffer.back());
                // if it is expanding candidate add to heap
                if (newTriangle->isClosestInternal() &&
                    newTriangle->getSqrDist() > lowerBoundSqr &&
                    newTriangle->getSqrDist() < upperBoundSqr &&
                    rightDirechtionT * newTriangle->getClosest() >
                        0)  // check if the triangle ist in the generll right
                            // direktion
                {
                  entryHeap.push_back(newTriangle);
                  std::push_heap(entryHeap.begin(), entryHeap.end(),
                                 triangle_comp);
                }

                if (!newTriangle->link(0, it->getTriangle(), it->getIndex())) {
                  break;
                }

                if (!newTriangle->link(2, lastTriangle, 1)) {
                  break;
                }

                lastTriangle = newTriangle;
              }

              if (it != edgeBuffer.end()) {
                // For some reason the silhouette couldn't be processed
                // completely so we stop here and take the last result
                break;
              }

              firstTriangle->link(2, lastTriangle, 1);
            }
          } while (!entryHeap.empty() &&
                   entryHeap[0]->getSqrDist() <= upperBoundSqr);
          //} while (entryHeap.size() > 0 && lowerBoundSqr <= upperBoundSqr);

          ret_normal = -normalize(current->getClosest());
          // retNormal   = -current->getNormal().getNormalized();

          const Vec3 witnessA = current->getClosestPoint(supportA);
          const Vec3 witnessB = current->getClosestPoint(supportB);

          if (trans(ret_normal) * (first_primitive.get_position() -
                                   second_primitive.get_position()) <
              0) {
            // std::cerr << "das darf nicht sein" << std::endl;
            return false;  // TODO completly wrong normal, so better not use it
          }

          contactPoint = 0.5 * (witnessA + witnessB);
          penetrationDepth =
              -(length(current->getClosest()) - 2.0 * contact_threshold);
          /*
          std::cerr << "normal=" << normal <<std::endl;
          std::cerr << "close =" << current->getClosest() << std::endl;
          std::cerr << "diff  =" << wittnesA - wittnesB  <<std::endl;
          std::cerr << "wittnesA    =" << wittnesA <<std::endl;
          std::cerr << "wittnesB    =" << wittnesB <<std::endl;
          std::cerr << "contact_point=" << contact_point << std::endl;
          std::cerr << "penDepth=" << penetration_depth  <<std::endl;
          std::cerr << "lowerBound=" << sqrt(lowerBoundSqr) <<std::endl;
          std::cerr << "curreBound=" << current->getClosest().length() <<
          std::endl; std::cerr << "upperBound=" << sqrt(upperBoundSqr)
          <<std::endl
          <<std::endl;
          */
          if (penetrationDepth < contact_threshold) {
            // normal points form object2 (mesh) to object1 (geom)
            // as the wittnes points are placed within the other body the normal
            // must be inverted negative penetraionDepth means penetration

            return true;
          }
          // no intersection found!
          return false;
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  EPA UTILITY FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         *
         * see Book "collision detection in interactive 3D environments" page161
         * ATTENTION seems to have no consistent behavior on the surface and
         * vertices
         */
        inline bool EPA::originInTetrahedron(const Vec3& p0, const Vec3& p1,
                                             const Vec3& p2, const Vec3& p3) {
          auto normal0T = trans((p1 - p0) % (p2 - p0));
          if ((dot(normal0T, p0) > 0.0) == (dot(normal0T, p3) > 0.0)) {
            return false;
          }
          auto normal1T = trans((p2 - p1) % (p3 - p1));
          if ((dot(normal1T, p1) > 0.0) == (dot(normal1T, p0) > 0.0)) {
            return false;
          }
          auto normal2T = trans((p3 - p2) % (p0 - p2));
          if ((dot(normal2T, p2) > 0.0) == (dot(normal2T, p1) > 0.0)) {
            return false;
          }
          auto normal3T = trans((p0 - p3) % (p1 - p3));
          if ((dot(normal3T, p3) > 0.0) == (dot(normal3T, p2) > 0.0)) {
            return false;
          }

          return true;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline bool EPA::originInTetrahedronVolumeMethod(const Vec3& A,
                                                         const Vec3& B,
                                                         const Vec3& C,
                                                         const Vec3& D) {
          auto aoT = trans(A);
          if (dot(aoT, (B % C)) <= real_t(0.0)) {
            // if volume of ABC and Origin <0.0 than the origin is on the wrong
            // side of ABC http://mathworld.wolfram.com/Tetrahedron.html volume
            // formula
            return false;
          }
          if (dot(aoT, (C % D)) <= real_t(0.0)) {
            return false;
          }
          if (dot(aoT, (D % B)) <= real_t(0.0)) {
            return false;
          }
          if (dot(B, (D % C)) <= real_t(0.0)) {
            return false;
          }
          return true;
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        inline bool EPA::pointInTetrahedron(const Vec3& A, const Vec3& B,
                                            const Vec3& C, const Vec3& D,
                                            const Vec3& point) {
          return originInTetrahedronVolumeMethod(A - point, B - point,
                                                 C - point, D - point);
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         * top, frontLeft ... are indices
         */
        inline void EPA::createInitialTetrahedron(std::size_t top,
                                                  std::size_t frontLeft,
                                                  std::size_t frontRight,
                                                  std::size_t back,
                                                  std::vector<Vec3>& epaVolume,
                                                  EPAEntryBuffer& entryBuffer) {
          // insert triangle 1
          entryBuffer.push_back(
              EPATriangle(top, frontLeft, frontRight, epaVolume));  //[0] vorne
          // insert triangle 2
          entryBuffer.push_back(EPATriangle(top, frontRight, back,
                                            epaVolume));  //[1] rechts hinten
          // insert triangle 3
          entryBuffer.push_back(EPATriangle(top, back, frontLeft,
                                            epaVolume));  //[2] links hinten
          // insert triangle 4
          entryBuffer.push_back(EPATriangle(back, frontRight, frontLeft,
                                            epaVolume));  //[3] unten

          // make links between the triangles
          entryBuffer[0].link(0, &(entryBuffer[2]), 2);  // Kante vorne links
          entryBuffer[0].link(2, &(entryBuffer[1]), 0);  // Kante vorne rechts
          entryBuffer[0].link(1, &(entryBuffer[3]), 1);  // kante vorne unten

          entryBuffer[1].link(2, &(entryBuffer[2]), 0);  // Kante hinten
          entryBuffer[1].link(1, &(entryBuffer[3]), 0);  // kante rechts unten

          entryBuffer[2].link(1, &(entryBuffer[3]), 2);  // kante links unten
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        template <typename FirstPrimitive, typename SecondPrimitive>
        inline void EPA::createInitialSimplex(unsigned char numPoints,
                                              FirstPrimitive& first_primitive,
                                              SecondPrimitive& second_primitive,
                                              std::vector<Vec3>& supportA,
                                              std::vector<Vec3>& supportB,
                                              std::vector<Vec3>& epaVolume,
                                              EPAEntryBuffer& entryBuffer) {
          switch (numPoints) {
            case 2: {
              // simplex is a line segement
              // add 3 points around the this segment
              // the COS is konvex so the resultig hexaheadron should be konvex
              // too

              Vec3 d = epaVolume[1] - epaVolume[0];
              // find coordinate axis e_i which is furthest form paralell to d
              // and therefor d hast the smallest abs(d[i])
              real_t abs0 = std::abs(d[0]);
              real_t abs1 = std::abs(d[1]);
              real_t abs2 = std::abs(d[2]);

              Vec3 axis;
              if (abs0 < abs1 && abs0 < abs2) {
                axis = Vec3{1.0, 0.0, 0.0};
              } else if (abs1 < abs0 && abs1 < abs2) {
                axis = Vec3{0.0, 1.0, 0.0};
              } else {
                axis = Vec3{0.0, 0.0, 1.0};
              }

              Vec3 direction1 = normalize(d % axis);

              /*
              Quat q(d, 2.0 / 3.0 * M_PI);
              Rot3 rot = q.toRotationMatrix();
              */
              Rot3 rot(rotation_from_axis_angle(d, 2.0 / 3.0 * M_PI));

              Vec3 direction2 = normalize(rot * direction1);
              Vec3 direction3 = normalize(rot * direction2);

              // add point in positive normal direction1
              supportA.push_back(first_primitive.supportContactThreshold(direction1));
              supportB.push_back(second_primitive.supportContactThreshold(-direction1));
              Vec3 support1 = supportA.back() - supportB.back();
              epaVolume.push_back(support1);  // epaVolume[2]

              // add point in negative normal direction2
              supportA.push_back(first_primitive.supportContactThreshold(direction2));
              supportB.push_back(second_primitive.supportContactThreshold(-direction2));
              Vec3 support2 = supportA.back() - supportB.back();
              epaVolume.push_back(support2);  // epaVolume[3]

              // add point in negative normal direction3
              supportA.push_back(first_primitive.supportContactThreshold(direction3));
              supportB.push_back(second_primitive.supportContactThreshold(-direction3));
              Vec3 support3 = supportA.back() - supportB.back();
              epaVolume.push_back(support3);  // epaVolume[4]

              // TODO check what happens if two of the new support points are
              // identical

              // Build the hexahedron as it is convex
              // epaVolume[1] = up
              // epaVolume[0] = down
              // epaVolume[2] = ccw1
              // epaVolume[3] = ccw2
              // epaVolume[4] = ccw3

              // insert triangle 1
              entryBuffer.push_back(
                  EPATriangle(1, 2, 3, epaVolume));  //[0] up->ccw1->ccw2
              // insert triangle 2
              entryBuffer.push_back(
                  EPATriangle(1, 3, 4, epaVolume));  //[1] up->ccw2->ccw3
              // insert triangle 3
              entryBuffer.push_back(
                  EPATriangle(1, 4, 2, epaVolume));  //[2] up->ccw3->ccw1

              // link these 3 triangles
              entryBuffer[0].link(2, &(entryBuffer[1]), 0);  // edge up->ccw1
              entryBuffer[1].link(2, &(entryBuffer[2]), 0);  // edge up->ccw2
              entryBuffer[2].link(2, &(entryBuffer[0]), 0);  // edge up->ccw3

              // insert triangle 4
              entryBuffer.push_back(
                  EPATriangle(0, 2, 4, epaVolume));  //[3] down->ccw1->ccw3
              // insert triangle 5
              entryBuffer.push_back(
                  EPATriangle(0, 4, 3, epaVolume));  //[4] down->ccw3->ccw2
              // insert triangle 6
              entryBuffer.push_back(
                  EPATriangle(0, 3, 2, epaVolume));  //[5] down->ccw2->ccw1

              // link these 3 triangles
              entryBuffer[3].link(2, &(entryBuffer[4]), 0);  // edge down->ccw3
              entryBuffer[4].link(2, &(entryBuffer[5]), 0);  // edge down->ccw1
              entryBuffer[5].link(2, &(entryBuffer[3]), 0);  // edge down->ccw1

              // link the two pyramids
              entryBuffer[0].link(1, &(entryBuffer[5]), 1);  // edge ccw1->ccw2
              entryBuffer[1].link(1, &(entryBuffer[4]), 1);  // edge ccw2->ccw3
              entryBuffer[2].link(1, &(entryBuffer[3]), 1);  // edge ccw3->ccw1

              break;
            }
            case 3: {
              // simplex is a triangle, add tow points in positive and negative
              // normal direction

              const Vec3& A = epaVolume[2];  // The Point last added to the
                                             // simplex
              const Vec3& B =
                  epaVolume[1];  // One Point that was already in the simplex
              const Vec3& C =
                  epaVolume[0];  // One Point that was already in the simplex
              // ABC is a conterclockwise triangle

              const Vec3 AB = B - A;  // The vector A->B
              const Vec3 AC = C - A;  // The vector A->C

              // The the normal pointing towards the
              // viewer if he sees a CCW triangle ABC
              const Vec3 ABC = normalize(AB % AC);

              // add point in positive normal direction
              supportA.push_back(first_primitive.supportContactThreshold(ABC));
              supportB.push_back(second_primitive.supportContactThreshold(-ABC));
              Vec3 support1 = supportA.back() - supportB.back();
              epaVolume.push_back(support1);  // epaVolume[3]

              // add point in negative normal direction
              supportA.push_back(first_primitive.supportContactThreshold(-ABC));
              supportB.push_back(second_primitive.supportContactThreshold(ABC));
              Vec3 support2 = supportA.back() - supportB.back();
              epaVolume.push_back(support2);  // epaVolume[4]

              // check if the hexahedron is convex aka check if a partial
              // tetrahedron contains the last point
              if (pointInTetrahedron(epaVolume[3], epaVolume[4], epaVolume[0],
                                     epaVolume[2], epaVolume[1])) {
                // epaVolumne[1] is whithin the tetraheadron 3-4-0-2 so this is
                // the epaVolume to take
                createInitialTetrahedron(3, 4, 0, 2, epaVolume, entryBuffer);
              } else if (pointInTetrahedron(epaVolume[3], epaVolume[4],
                                            epaVolume[1], epaVolume[0],
                                            epaVolume[2])) {
                createInitialTetrahedron(3, 4, 1, 0, epaVolume, entryBuffer);
              } else if (pointInTetrahedron(epaVolume[3], epaVolume[4],
                                            epaVolume[2], epaVolume[1],
                                            epaVolume[0])) {
                createInitialTetrahedron(3, 4, 2, 1, epaVolume, entryBuffer);
              } else {
                // Build the hexahedron as it is convex
                // insert triangle 1
                entryBuffer.push_back(
                    EPATriangle(3, 2, 1, epaVolume));  //[0] support1->A->B
                // insert triangle 2
                entryBuffer.push_back(
                    EPATriangle(3, 1, 0, epaVolume));  //[1] support1->B->C
                // insert triangle 3
                entryBuffer.push_back(
                    EPATriangle(3, 0, 2, epaVolume));  //[2] support1->C->A

                // link these 3 triangles
                entryBuffer[0].link(2, &(entryBuffer[1]),
                                    0);  // edge support1->A
                entryBuffer[1].link(2, &(entryBuffer[2]),
                                    0);  // edge support1->B
                entryBuffer[2].link(2, &(entryBuffer[0]),
                                    0);  // edge support1->C

                // insert triangle 4
                entryBuffer.push_back(
                    EPATriangle(4, 2, 0, epaVolume));  //[3] support2->A->C
                // insert triangle 5
                entryBuffer.push_back(
                    EPATriangle(4, 0, 1, epaVolume));  //[4] support2->C->B
                // insert triangle 6
                entryBuffer.push_back(
                    EPATriangle(4, 1, 2, epaVolume));  //[5] support2->B->A

                // link these 3 triangles
                entryBuffer[3].link(2, &(entryBuffer[4]),
                                    0);  // edge support2->C
                entryBuffer[4].link(2, &(entryBuffer[5]),
                                    0);  // edge support2->B
                entryBuffer[5].link(2, &(entryBuffer[3]),
                                    0);  // edge support2->A

                // link the two pyramids
                entryBuffer[0].link(1, &(entryBuffer[5]), 1);  // edge A->B
                entryBuffer[1].link(1, &(entryBuffer[4]), 1);  // edge B->C
                entryBuffer[2].link(1, &(entryBuffer[3]), 1);  // edge C->A
              }

              break;
            }
            case 4: {
              createInitialTetrahedron(3, 2, 1, 0, epaVolume, entryBuffer);
              break;
            }
            default: {
              ELASTICA_ASSERT(false, "invalid number of simplex points in EPA");
              break;
            }
          }
        }
        //*************************************************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
