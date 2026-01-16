//==============================================================================
/*!
//  \file
//  \brief SymplecticSteppable protocols
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <utility>

#include "Time/SymplecticSteppers.hpp"
#include "Time/Types.hpp"

namespace elastica {

  namespace timestepper {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Symplectic steppable protocol.
       * \ingroup time
       *
       * Class to enforce adherence to a Symplectic steppable protocol. Any
       * steppable class within the \elastica library should (publicly) inherit
       * from this class to indicate it qualifies as a Domain. Only in case a
       * class is derived publicly from this base class, the  tt::conforms_to
       * and tt::assert_conforms_to type traits recognizes the class as valid
       * to be stepped with a symplectic stepper.
       *
       * Requires that a conforming type `ConformingType` has these member
       * functions:
       * \snippet this expected_functions
       *
       * \example
       * The following shows an example of minimal conformance to this protocol.
       * \snippet Protocols/Test_SymplecticSteppable.cpp symplectic_steppable_eg
       */
      struct SymplecticSteppable {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          /// [expected_functions]
          // Try calling the `ConformingType::step` templated member function
          using span_return_type =
              decltype(std::declval<ConformingType const&>().step(
                  std::declval<SymplecticStepper<PositionVerlet> const&>(),
                  std::declval<Time&>(), std::declval<TimeDelta const>()));
          static_assert(std::is_same_v<span_return_type, void>,
                        R"error(
Not a conforming SymplecticSteppable class, doesn't properly implement function
`template <typename SymplecticAlgorithm>
void step(SymplecticStepper<SymplecticAlgorithm> const& time_stepper,
          Time& time_v,
          TimeDelta const dt
) const`)error");
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace timestepper

}  // namespace elastica