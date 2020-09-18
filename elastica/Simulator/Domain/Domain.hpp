//==============================================================================
/*!
//  \file
//  \brief Domain implementation
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

#include <type_traits>

///
#include "Simulator/Domain/Types.hpp"
///
#include "Simulator/Domain/Policies/PeriodicDomainPolicy.hpp"
#include "Simulator/Domain/Policies/UnboundedDomainPolicy.hpp"
#include "Simulator/Domain/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Requires.hpp"

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Class template representing the Domain of the simulation
    // \ingroup domain
    //
    // The Domain class template represents an instantiation of the simulation
    // domain. Its behavior is customized by the type of the span (\tparam Real)
    // the policy of the domain (periodic, unbounded etc.).
    */
    template <typename Real,                           // Real type
              template <typename> class DomainPolicy,  // Policy of the domain
              Requires<std::is_floating_point<Real>::value>>  // Restricting
                                                              // condition
                                                              class Domain
        : public ::tt::ConformsTo<::elastica::protocols::ComputationalDomain>,
          public DomainPolicy<Real> {
     private:
      //**Type definitions******************************************************
      using Policy = DomainPolicy<Real>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      using bound_type = Real;  //!< Boundary type of the composed policy
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      using Policy::Policy;
      //@}
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

}  // namespace elastica