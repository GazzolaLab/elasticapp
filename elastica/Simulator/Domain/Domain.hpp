//==============================================================================
/*!
//  \file
//  \brief Domain implementation
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

#include <type_traits>

///
#include "Simulator/Domain/Types.hpp"
///
#include "Simulator/Domain/Policies/PeriodicDomainPolicy.hpp"
#include "Simulator/Domain/Policies/UnboundedDomainPolicy.hpp"
#include "Simulator/Domain/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Requires.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"

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
     * \ingroup domain
     *
     * The Domain class template represents an instantiation of the simulation
     * domain. Its behavior is customized by the type of the span (\tparam Real)
     * the policy of the domain (periodic, unbounded etc.).
     *
     * \tparam Real The real type (float or double)
     * \tparam DomainPolicy Policy template of the domain, one of
     * UnboundedPolicy or PeriodicPolicy
     */
    template <typename Real, template <typename> class DomainPolicy,
              Requires<std::is_floating_point<Real>::value>>
    class Domain
        : public ::tt::ConformsTo<::elastica::protocols::ComputationalDomain>,
          public DomainPolicy<Real> {
     private:
      //**Type definitions******************************************************
      using Policy = DomainPolicy<Real>;
      //************************************************************************

      //**Friendships***********************************************************
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<Domain<Real, DomainPolicy>>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      using bound_type = Real;  //!< Boundary type of the composed policy
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      template <typename... Args>
      explicit Domain(Args&&... args) : Policy(std::forward<Args>(args)...) {}
      //@}
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

}  // namespace elastica