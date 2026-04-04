//==============================================================================
/*!
//  \file
//  \brief A vector for storing contacts
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

#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Utilities/Math/Vec3.hpp"
//
#include <iostream>
#include <memory>
#include <vector>

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief An adapter for vector types for registering contacts between rigid
     * bodies.
     * \ingroup contact
     *
     * A vector of contacts between rigid bodies in the simulation system.
     */
    template <typename C = Contact,  // Type of the contact
              typename Container = std::vector<C, std::allocator<C>>>
    class ContactVectorAdapter {
     private:
      using ContactType = C;
      using PointerType = C*;
      // revert to value semantics for now
      using container_type = Container;

     public:
      using value_type = typename container_type::value_type;
      using pointer = typename container_type::pointer;
      using allocator_type = typename container_type::allocator_type;
      using size_type = typename container_type::size_type;
      using difference_type = typename container_type::difference_type;
      using iterator = typename container_type::iterator;
      using const_iterator = typename container_type::const_iterator;
      using reference = typename container_type::reference;
      using const_reference = typename container_type::const_reference;

      explicit inline ContactVectorAdapter(size_type initCapacity = 100);

      constexpr auto begin() noexcept { return v_.begin(); }
      constexpr auto begin() const noexcept { return v_.begin(); }
      constexpr auto end() noexcept { return v_.end(); }
      constexpr auto end() const noexcept { return v_.end(); }
      constexpr auto cbegin() const noexcept { return v_.cbegin(); }
      constexpr auto cend() const noexcept { return v_.cend(); }

      // to work without explicit qualification, can be removed if unnecessary
      friend decltype(auto) begin(
          ContactVectorAdapter<C, Container>& c) noexcept {
        return c.begin();
      }
      friend decltype(auto) begin(
          ContactVectorAdapter<C, Container> const& c) noexcept {
        return c.begin();
      }
      friend decltype(auto) end(
          ContactVectorAdapter<C, Container>& c) noexcept {
        return c.end();
      }
      friend decltype(auto) end(
          ContactVectorAdapter<C, Container> const& c) noexcept {
        return c.end();
      }
      friend decltype(auto) cbegin(
          ContactVectorAdapter<C, Container>& c) noexcept {
        // begin(c)
        return c.cbegin();
      }
      friend decltype(auto) cend(
          ContactVectorAdapter<C, Container> const& c) noexcept {
        // end(c);
        return c.cend();
      }

      void reserve(size_type new_capacity) { v_.reserve(new_capacity); }
      void clear() noexcept { v_.clear(); }
      void shrink_to_fit() noexcept { v_.shrink_to_fit(); }

      // for testing
      inline reference operator[](size_type n) noexcept { return v_[n]; }
      inline const_reference operator[](size_type n) const noexcept {
        return v_[n];
      }

      inline reference at(size_type n) /*noexcept*/ { return v_.at(n); }
      inline const_reference at(size_type n) const /*noexcept*/ {
        return v_.at(n);
      }

      // needed in some tests
      inline reference back() /*noexcept*/ { return v_.back(); }
      inline const_reference back() const /*noexcept*/ { return v_.back(); }

      inline auto empty() const noexcept -> bool { return v_.empty(); }
      inline auto size() const noexcept -> size_type { return v_.size(); }
      inline decltype(auto) data() & noexcept { return v_.data(); }
      inline decltype(auto) data() const& noexcept { return v_.data(); }

      //**Contact setup functions***********************************************
      /*!\name Contact setup functions */
      //@{
      inline void add_vertex_face_contact(GeomID g1, GeomID g2,
                                          Vec3 const& gpos, Vec3 const& normal,
                                          real_t dist);
      inline void add_edge_edge_contact(GeomID g1, GeomID g2, Vec3 const& gpos,
                                        Vec3 const& normal, Vec3 const& e1,
                                        Vec3 const& e2, real_t dist);
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Storage for the contacts
      container_type v_;
      //@}
      //************************************************************************
    };

    //==========================================================================
    //
    //  CONSTRUCTORS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Standard constructor for ContactVector.
     *
     * \param initCapacity The initial capacity of the contact vector.
     */
    template <typename C,          // Type of the contact
              typename Container>  // Container
    inline ContactVectorAdapter<C, Container>::ContactVectorAdapter(
        size_type initCapacity)
        : v_() {
      reserve(initCapacity);
    }
    //**************************************************************************

    //==========================================================================
    //
    //  CONTACT SETUP FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Adding a new vertex/face contact to the contact vector.
     *
     * \param g1 The first colliding geometric primitive.
     * \param g2 The second colliding geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The normal of the contact.
     * \param dist The distance between the surfaces of the two colliding rigid
     * bodies.
     * \return void
     *
     * Creating a vertex/face contact between the superordinate bodies of the
     * (potentially) subordinate bodies \a b1 and \a b2 and adding it to the
     * contact vector.
     */
    template <typename C,          // Type of the contact
              typename Container>  // Container
    inline void ContactVectorAdapter<C, Container>::add_vertex_face_contact(
        GeomID g1, GeomID g2, Vec3 const& gpos, Vec3 const& normal,
        real_t dist) {
      // Creating a new vertex/face contact
      v_.emplace_back(g1,      // First colliding geometric primitive
                      g2,      // Second colliding geometric primitive
                      gpos,    // Global position of the contact
                      normal,  // Normal of the contact (from body 2 to body 1)
                      dist  // Distance between the surfaces (negative distance
                            // means penetration)
      );
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Adding a new edge/edge contact to the contact vector.
     *
     * \param g1 The first colliding geometric primitive.
     * \param g2 The second colliding geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The normal of the contact.
     * \param e1 Edge direction of the colliding edge of the first colliding
     * rigid body.
     * \param e2 Edge direction of the colliding edge of the second
     * colliding rigid body.
     * \param dist The distance between the surfaces of
     * the two colliding rigid bodies.
     * \return void
     *
     * Creating an edge/edge contact between the superordinate bodies of the
     * (potentially) subordinate bodies \a b1 and \a b2 and adding it to the
     * contact vector.
     */
    template <typename C,          // Type of the contact
              typename Container>  // Container
    inline void ContactVectorAdapter<C, Container>::add_edge_edge_contact(
        GeomID g1, GeomID g2, Vec3 const& gpos, Vec3 const& normal,
        Vec3 const& e1, Vec3 const& e2, real_t dist) {
      // Creating a new edge/edge contact
      v_.emplace_back(g1,      // First colliding geometric primitive
                      g2,      // Second colliding geometric primitive
                      gpos,    // Global position of the contact
                      normal,  // Normal of the contact (from body 2 to body 1)
                      e1,      // Edge direction of the colliding edge of body 1
                      e2,      // Edge direction of the colliding edge of body 2
                      dist  // Distance between the surfaces (negative distance
                            // means penetration)
      );
    }
    //**************************************************************************

    template <typename C, typename Container>
    inline ::std::ostream& operator<<(
        ::std::ostream& os, ContactVectorAdapter<C, Container> const& c) {
      os << c.data();
      return os;
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
