//==============================================================================
/*!
//  \file
//  \brief A class for recording Contacts
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
**/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactTrait.hpp"
#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Vec3.hpp"
//#include "Utilities/MemoryPool.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\defgroup contact Contact
     * \ingroup collision_system
     * \brief Data-structures for registering contacts between two bodies
     */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Contact between rigid bodies.
     * \ingroup contact
     *
     * The Contact class records all types of contacts between
     * rigid bodies in the simulation system. Contacts between rigid bodies are
     * classified depending on the relative velocity between the touching rigid
     * bodies, as seen in ::elastica::collision::ContactType.
     */
    class Contact : public detail::ContactTrait<CollisionConfig> {
     private:
      //**Type definitions******************************************************
      //! The type of the parent class.
      using Parent = ContactTrait<CollisionConfig>;
      //! Contact Pool for contacts.
      // using ContactPool = MemoryPool<Contact, 10000>;
      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      explicit Contact(GeomID g1, GeomID g2, Vec3 const& gpos,
                       Vec3 const& normal, real_t dist);
      explicit Contact(GeomID g1, GeomID g2, Vec3 const& gpos,
                       Vec3 const& normal, Vec3 const& e1, Vec3 const& e2,
                       real_t dist);
      Contact(Contact&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      inline ~Contact() = default;
      //@}
      //************************************************************************

      //**Memory management functions*******************************************
      /*!\name Memory management functions */
      //@{
      // static inline void* operator new(::std::size_t size);
      // static inline void operator delete(void* rawMemory);
      //@}
      //************************************************************************

      //**Impulse and force functions*******************************************
      /*!\name Impulse and force functions */
      //@{
      inline void apply_force(Vec3 const& f) const;
      //      inline void apply_impulse(const Vec3& j) const;
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Memory pool for contacts.
      // static ContactPool contactPool_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  MEMORY MANAGEMENT FUNCTIONS
    //
    //==========================================================================

    // //**************************************************************************
    // /*!\brief Dynamic allocation of a single Contact object.
    //  *
    //  * \param size (Implicit) size argument of the new operator.
    //  * \return Pointer to the raw memory.
    //  */
    // inline void* Contact::operator new(::std::size_t /*size*/) {
    //   return contactPool_.malloc();
    // }
    // //**************************************************************************

    // //**************************************************************************
    // /*!\brief Dynamic deallocation of a single Contact object.
    //  *
    //  * \param rawMemory Pointer to the raw memory.
    //  * \return void
    //  */
    // inline void Contact::operator delete(void* rawMemory) {
    //   contactPool_.free(rawMemory);
    // }
    // //**************************************************************************

    //==========================================================================
    //
    //  IMPULSE AND FORCE FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Force transfer to both contacting rigid bodies.
     *
     * \param f The acting force on the first contacting rigid body.
     * \return void
     */
    inline void Contact::apply_force(Vec3 const& f) const {
      b1_->add_force_at_position(f, gpos_);
      b2_->add_force_at_position(-f, gpos_);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Impulse transfer to both contacting rigid bodies.
     *
     * \param j The acting impulse on the first contacting rigid body.
     * \return void
     */
    //    inline void Contact::apply_impulse(const Vec3& j) const {
    //      b1_->addImpulseAtPos(j, gpos_);
    //      b2_->addImpulseAtPos(-j, gpos_);
    //    }
    //**************************************************************************

    //==========================================================================
    //
    //  GLOBAL OPERATORS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Global output operator for contacts.
     * \ingroup contact
     g
     * \param os Reference to the output stream.
     * \param c Reference to a constant contact object.
     * \return Reference to the output stream.
     */
    inline ::std::ostream& operator<<(::std::ostream& os, Contact const& c) {
      char const* tab = "";
      os << "--"
         << "CONTACT PARAMETERS"
         << "---------------------------------------------------------\n";
      using std::setw;

      os << tab << " Contact "  // << c.get_ID()
         << "at position " << c.get_position() << "\n";

      os << tab
         << "   Body 1            = " << c.get_first_body()->getSystemID()
         << "\n"
         << tab
         << "   Body 2            = " << c.get_second_body()->getSystemID()
         << "\n"
         << tab << "   Normal            = " << c.get_normal() << "\n"
         << tab << "   Distance          = " << c.get_distance() << "\n";
      /*
      if (verboseMode) {
        os << tab << "   Restitution       = " << c.getRestitution() <<
        "\n"
           << tab << "   Stiffness         = " << c.getStiffness() << "\n"
           << tab << "   Damping_n         = " << c.getDampingN() << "\n"
           << tab << "   Damping_t         = " << c.getDampingT() << "\n"
           << tab << "   Friction coeff.   = " << c.getFriction() << "\n"
           << tab << "   Contact type      = "
           << (c.getType() == colliding
                   ? "colliding"
                   : (c.getType() == resting ? "resting" : "separating"))
           << "\n"
           << tab << "   Rel. normal vel.  = " << c.getNormalRelVel() <<
           "\n";
      }
      */
      os << "------------------------------------------------------------------"
            "--"
            "------------\n"
         << std::endl;
      return os;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Global output operator for contact handles.
     * \ingroup contact
     *
     * \param os Reference to the output stream.
     * \param c Constant contact handle.
     * \return Reference to the output stream.
     */
    inline std::ostream& operator<<(std::ostream& os, Contact const* c) {
      return operator<<(os, *c);
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
