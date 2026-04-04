//==============================================================================
/*!
//  \file
//  \brief Type traits for the Forcing configuration
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

#include "Simulator/Configuration/Protocols/Forcing.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Check whether a given type `FC` is a valid
     * ForcingConfiguration
     * \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `FC` strictly conforms to
     * ::elastica::configuration::protocols::ForcingConfiguration, otherwise
     * inherits from std::false_type.
     *
     * \usage
     * For any type `FC`,
     * \code
     * using result = IsValidForcingConfiguration<FC>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `FC` strictly conforms to
     * ::elastica::configuration::protocols::ForcingConfiguration, that is
     * it publicly inherits from the protocol and passes all the requirements
     * imposed by the protocol, then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \example
     * \snippet TypeTraits/Test_Forcing.cpp is_valid_forcing_config_eg
     *
     * \tparam FC : the type to check
     *
     * \see ::elastica::configuration::protocols::ForcingConfiguration
     */
    template <typename FC>
    struct IsValidForcingConfiguration
        : public ::tt::detail::AssertConformsToImpl<
              FC, ::elastica::configuration::protocols::ForcingConfiguration> {
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
