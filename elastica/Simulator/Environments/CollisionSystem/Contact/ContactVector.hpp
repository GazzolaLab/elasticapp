#pragma once

#include <vector>

#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    // TODO : Customize later
    template <typename C>  // Type of the contact
    class ContactVector {
     private:
      using ContactType = C;
      using PointerType = C*;
      using StorageType = std::vector<PointerType>;

     public:
      using value_type = typename StorageType::value_type;
      using pointer = typename StorageType::pointer;
      using allocator_type = typename StorageType::allocator_type;
      using size_type = typename StorageType::size_type;
      using difference_type = typename StorageType::difference_type;
      using iterator = typename StorageType::iterator;
      using const_iterator = typename StorageType::const_iterator;

      explicit inline ContactVector(size_type initCapacity = 100);

      constexpr auto begin() noexcept { return v_.begin(); }
      constexpr auto end() noexcept { return v_.end(); }
      void clear() noexcept { v_.clear(); }
      inline auto empty() const noexcept -> bool { return v_.empty(); }
      inline auto size() const noexcept -> size_type { return v_.size(); }
      inline auto data() noexcept {return v_.data();}

      //**Contact setup
      // functions*********************************************************************
      /*!\name Contact setup functions */
      //@{
      inline void addVertexFaceContact(GeomID g1, GeomID g2, const Vec3& gpos,
                                       const Vec3& normal, real_t dist);
      inline void addEdgeEdgeContact(GeomID g1, GeomID g2, const Vec3& gpos,
                                     const Vec3& normal, const Vec3& e1,
                                     const Vec3& e2, real_t dist);
      //@}
      //**********************************************************************************************

     private:
      StorageType v_;
    };

    //=================================================================================================
    //
    //  CONSTRUCTORS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Standard constructor for ContactVector.
     *
     * \param initCapacity The initial capacity of the contact vector.
     */
    template <typename C>  // Type of the contact
    inline ContactVector<C>::ContactVector(size_type initCapacity)
        : v_(initCapacity)  // Base class initialization
    {}
    //*************************************************************************************************

    //=================================================================================================
    //
    //  CONTACT SETUP FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Adding a new vertex/face contact to the contact vector.
     *
     * \param g1 The first colliding geometric primitive.
     * \param g2 The second colliding geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The normal of the contact.
     * \param dist The distance between the surfaces of the two colliding rigid
     * bodies. \return void
     *
     * Creating a vertex/face contact between the superordinate bodies of the
     * (potentially) subordinate bodies \a b1 and \a b2 and adding it to the
     * contact vector.
     */
    template <typename C>  // Type of the contact
    inline void ContactVector<C>::addVertexFaceContact(GeomID g1, GeomID g2,
                                                       const Vec3& gpos,
                                                       const Vec3& normal,
                                                       real_t dist) {
      // Creating a new vertex/face contact
      // TODO : Doesn't abide by usual destruction rules : the caller is
      // responsible for managing memory : use policies from PtrVector here
      v_.push_back(new ContactType(
          g1,      // First colliding geometric primitive
          g2,      // Second colliding geometric primitive
          gpos,    // Global position of the contact
          normal,  // Normal of the contact (from body 2 to body 1)
          dist     // Distance between the surfaces (negative distance means
                   // penetration)
          ));
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Adding a new edge/edge contact to the contact vector.
     *
     * \param g1 The first colliding geometric primitive.
     * \param g2 The second colliding geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The normal of the contact.
     * \param e1 Edge direction of the colliding edge of the first colliding
     * rigid body. \param e2 Edge direction of the colliding edge of the second
     * colliding rigid body. \param dist The distance between the surfaces of
     * the two colliding rigid bodies. \return void
     *
     * Creating an edge/edge contact between the superordinate bodies of the
     * (potentially) subordinate bodies \a b1 and \a b2 and adding it to the
     * contact vector.
     */
    template <typename C>  // Type of the contact
    inline void ContactVector<C>::addEdgeEdgeContact(
        GeomID g1, GeomID g2, const Vec3& gpos, const Vec3& normal,
        const Vec3& e1, const Vec3& e2, real_t dist) {
      // Creating a new edge/edge contact
      v_.push_back(new ContactType(
          g1,      // First colliding geometric primitive
          g2,      // Second colliding geometric primitive
          gpos,    // Global position of the contact
          normal,  // Normal of the contact (from body 2 to body 1)
          e1,      // Edge direction of the colliding edge of body 1
          e2,      // Edge direction of the colliding edge of body 2
          dist     // Distance between the surfaces (negative distance means
                   // penetration)
          ));
    }
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
