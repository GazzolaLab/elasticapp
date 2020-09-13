#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cstddef>

#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Trait definiing unique ID for different directions
    // \ingroup domain
    //
    // Directions is a Trait defining the type and number of directions in 3D.
    // It serves as a strong type for Directions within the Elastica++ library.
    */
    struct Directions : public NonCreatable {
     public:
      //**Type definitions******************************************************
      using DirectionType = std::uint8_t;  //!< The type of direction ID
      //************************************************************************

      //**Static members********************************************************
      static constexpr DirectionType X = 0;  //!< unique ID for X direction
      static constexpr DirectionType Y = 1;  //!< unique ID for Y direction
      static constexpr DirectionType Z = 2;  //!< unique ID for Z direction
      static constexpr DirectionType Dimension = 3; //!< Dimension of simulation
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

}  // namespace elastica
