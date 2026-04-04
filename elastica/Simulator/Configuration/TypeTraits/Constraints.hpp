//==============================================================================
/*!
//  \file
//  \brief Type traits for the Constraints configuration
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

#include "Simulator/Configuration/Protocols/Constraints.hpp"
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
     * ConstraintsConfiguration
     * \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `CC` strictly conforms to
     * ::elastica::configuration::protocols::ConstraintsConfiguration, otherwise
     * inherits from std::false_type.
     *
     * \usage
     * For any type `CC`,
     * \code
     * using result = IsValidConstraintsConfiguration<CC>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `CC` strictly conforms to
     * ::elastica::configuration::protocols::ConstraintsConfiguration, that is
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
     * \snippet TypeTraits/Test_Constraints.cpp is_valid_constraints_config_eg
     *
     * \tparam CC : the type to check
     *
     * \see ::elastica::configuration::protocols::ConstraintsConfiguration
     */
    template <typename CC>
    struct IsValidConstraintsConfiguration
        : public ::tt::detail::AssertConformsToImpl<
              CC,
              ::elastica::configuration::protocols::ConstraintsConfiguration> {
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
