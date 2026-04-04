//==============================================================================
/*!
//  \file
//  \brief Type traits for the Connections configuration
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
#include "Simulator/Configuration/Protocols/Connections.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Check whether a given type `CC` is a valid
     * ConnectionsConfiguration
     * \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `CC` strictly conforms to
     * ::elastica::configuration::protocols::ConnectionsConfiguration, otherwise
     * inherits from std::false_type.
     *
     * \usage
     * For any type `CC`,
     * \code
     * using result = IsValidConnectionsConfiguration<CC>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `CC` strictly conforms to
     * ::elastica::configuration::protocols::ConnectionsConfiguration, that is
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
     * \snippet TypeTraits/Test_Connections.cpp is_valid_connections_config_eg
     *
     * \tparam CC : the type to check
     *
     * \see ::elastica::configuration::protocols::ConnectionsConfiguration
     */
    template <typename CC>
    struct IsValidConnectionsConfiguration
        : public ::tt::detail::AssertConformsToImpl<
              CC,
              ::elastica::configuration::protocols::ConnectionsConfiguration> {
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica