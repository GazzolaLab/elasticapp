//*************************************************************************************************
/*!
 *  \file pe/core/contact/Contact.h
 *  \brief Header file for the Contact class
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
//*************************************************************************************************

#pragma once

//*************************************************************************************************
// Includes
//*************************************************************************************************

//#include <pe/core/Configuration.h>
//#include <pe/core/Types.h>
//#include <pe/core/contact/ContactTrait.h>
//#include <pe/core/rigidbody/GeomPrimitive.h>
//#include <pe/system/Precision.h>
//#include <pe/system/VerboseMode.h>
//#include <pe/util/ColorMacros.h>
//#include <pe/util/MemoryPool.h>
//#include <pe/util/Types.h>

#include "elastica/Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Contact/ContactTrait.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/ContactType.hpp"
#include "elastica/Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/MemoryPool.hpp"

namespace elastica {

  namespace collision {

    //=================================================================================================
    //
    //  CLASS DEFINITION
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\defgroup contact Contact
     * \ingroup core
     */
    /*!\brief Contact between rigid bodies.
     * \ingroup contact
     *
     * The Contact class is the base class for all types of contacts between
     * rigid bodies in the simulation system. Contacts between rigid bodies are
     * classified depending on the relative velocity between the touching rigid
     * bodies:
     *
     *  - \f$ v_{rel} \f$ > 0: separating contact
     *  - \f$ v_{rel} \f$ = 0: resting contact
     *  - \f$ v_{rel} \f$ < 0: colliding contact\n
     *
     * (in order to classify the contact, pe::collisionThreshold is used as
     * tolerance level).
     */
    class Contact : public ContactTrait<CollisionConfig> {
     private:
      //**Type
      // definitions****************************************************************************
      //!< The type of the parent class.
      using Parent = ContactTrait<CollisionConfig>;

      //! Contact Pool for contacts.
      using ContactPool = MemoryPool<Contact, 10000>;
      //**********************************************************************************************

     public:
      //**Constructors********************************************************************************
      /*!\name Constructors */
      //@{
      // TODO : Move semantics here : critical for efficiency
      explicit Contact(GeomID g1, GeomID g2, const Vec3& gpos,
                       const Vec3& normal, real_t dist);
      explicit Contact(GeomID g1, GeomID g2, const Vec3& gpos,
                       const Vec3& normal, const Vec3& e1, const Vec3& e2,
                       real_t dist);
      //@}
      //**********************************************************************************************

      //**Destructor**********************************************************************************
      /*!\name Destructor */
      //@{
      inline ~Contact();
      //@}
      //**********************************************************************************************

      //**Memory management
      // functions*****************************************************************
      /*!\name Memory management functions */
      //@{
      static inline void* operator new(::std::size_t size);
      static inline void operator delete(void* rawMemory);
      //@}
      //**********************************************************************************************

      //**Impulse and force
      // functions*****************************************************************
      /*!\name Impulse and force functions */
      //@{
      inline void apply_force(const Vec3& f) const;
      //      inline void apply_impulse(const Vec3& j) const;
      //@}
      //**********************************************************************************************

     private:
      //**Member
      // variables****************************************************************************
      /*!\name Member variables */
      //@{
      static ContactPool contactPool_;  //!< Memory pool for contacts.
                                        //@}
      //**********************************************************************************************
    };
    //*************************************************************************************************

    //=================================================================================================
    //
    //  DESTRUCTOR
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Destructor for the Contact class.
     */
    inline Contact::~Contact() {}
    //*************************************************************************************************

    //=================================================================================================
    //
    //  MEMORY MANAGEMENT FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Dynamic allocation of a single Contact object.
     *
     * \param size (Implicit) size argument of the new operator.
     * \return Pointer to the raw memory.
     */
    inline void* Contact::operator new(::std::size_t /*size*/) {
      return contactPool_.malloc();
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Dynamic deallocation of a single Contact object.
     *
     * \param rawMemory Pointer to the raw memory.
     * \return void
     */
    inline void Contact::operator delete(void* rawMemory) {
      contactPool_.free(rawMemory);
    }
    //*************************************************************************************************

    //=================================================================================================
    //
    //  IMPULSE AND FORCE FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Force transfer to both contacting rigid bodies.
     *
     * \param f The acting force on the first contacting rigid body.
     * \return void
     */
    inline void Contact::apply_force(const Vec3& f) const {
      b1_->add_force_at_position(f, gpos_);
      b2_->add_force_at_position(-f, gpos_);
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Impulse transfer to both contacting rigid bodies.
     *
     * \param j The acting impulse on the first contacting rigid body.
     * \return void
     */
    //    inline void Contact::apply_impulse(const Vec3& j) const {
    //      b1_->addImpulseAtPos(j, gpos_);
    //      b2_->addImpulseAtPos(-j, gpos_);
    //    }
    //*************************************************************************************************

    //=================================================================================================
    //
    //  GLOBAL OPERATORS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Global output operator for contacts.
     * \ingroup contact
     *
     * \param os Reference to the output stream.
     * \param c Reference to a constant contact object.
     * \return Reference to the output stream.
     */
    //    inline ::std::ostream& operator<<(::std::ostream& os, const Contact&
    //    c) {
    //      os << std::endl;
    //    }
    inline ::std::ostream& operator<<(::std::ostream& os, const Contact& c) {
      const char* tab = "";
      os << "--"
         << "CONTACT PARAMETERS"
         << "---------------------------------------------------------\n";
      using std::setw;

      os << tab << " Contact " << c.get_ID() << " at position "
         << c.get_position() << "\n";

      os << tab << "   Body 1            = " << c.getBody1()->getSystemID()
         << "\n"
         << tab << "   Body 2            = " << c.getBody2()->getSystemID()
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
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Global output operator for contact handles.
     * \ingroup contact
     *
     * \param os Reference to the output stream.
     * \param c Constant contact handle.
     * \return Reference to the output stream.
     */
    inline std::ostream& operator<<(std::ostream& os, ConstContactID c) {
      return operator<<(os, *c);
    }
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
