#pragma once

namespace elastica {

  namespace collision {

    // Forward declarations
    class GeomPrimitive;
    class RigidBodyPrimitive;
    class Contact;

    using GeomID = GeomPrimitive*;
    using ConstGeomID = const GeomPrimitive*;

    using BodyID = RigidBodyPrimitive*;
    using ConstBodyID = const RigidBodyPrimitive*;

    //*************************************************************************************************
    /*!\brief Handle for a contact between two rigid bodies.
     * \ingroup core
     */
    using ContactID = Contact*;
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Handle for a constant contact between two rigid bodies.
     * \ingroup core
     */
    using ConstContactID = const Contact*;
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
