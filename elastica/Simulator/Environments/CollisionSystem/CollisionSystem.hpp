#pragma once

namespace elastica {

  namespace collision {

    //=================================================================================================
    //
    //  CLASS DEFINITION
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Collision system of the rigid body simulation world.
     * \ingroup core
     *
     * The CollisionSystem class represents the collision system of the rigid
     body simulation world,
     * which is a combination of all collision related algorithms. The collision
     system provides all
     * functionality for the coarse collision detection (see
     pe::pe_COARSE_COLLISION_DETECTOR), the
     * fine collision detection (see pe::pe_FINE_COLLISION_DETECTOR), the batch
     generation process
     * (see pe::pe_BATCH_GENERATOR), and the collision response (see
     pe::pe_CONSTRAINT_SOLVER). The
     * CollisionSystem class is closely coupled to the World class and is
     implemented in terms of
     * the world's body storage and contact storage.\n
     * The CollisionSystem needs to be instantiated with a single template
     template argument of the
     * following structure:

       \code
       template< template<typename> class CD                      // Type of the
     coarse collision detection algorithm , typename FD // Type of the fine
     collision detection algorithm , template<typename> class BG // Type of the
     batch generation algorithm , template<typename,typename,typename> class CR
     >
     // Type of the collision response algorithm struct Configuration; \endcode

     * The given template template argument must be take four template
     arguments, where the first
     * argument resembles the currently selected coarse collision detection
     algorithm, the second
     * represents the selected fine collision detection algorithm, the third is
     the selected
     * batch generation algorithm, and the fourth represents the selected
     collision response
     * algorithm. The attempt to instantiate the CollisionSystem with any other
     type of template
     * argument results in a compile time error!
     */
    template <class SimulatorConfig, class CollisionConfig>  // Type of the configuration
    class CollisionSystem;
    //*************************************************************************************************


  }  // namespace collision

}  // namespace elastica

#include "elastica/Simulator/Environments/CollisionSystem/DEMSolver.hpp"