//==============================================================================
/*!
//  \file
//  \brief Type traits for the Domain configuration
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

#include "Simulator/Configuration/Protocols/Domain.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Check whether a given type `DC` is a DomainConfiguration
     * \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `DC` conforms to
     * ::elastica::configuration::protocols::DomainConfiguration, otherwise
     * inherits from std::false_type.
     *
     * \usage
     * For any type `DC`,
     * \code
     * using result = IsDomainConfiguration<DC>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `DC` publicly inherits from
     * ::elastica::configuration::protocols::DomainConfiguration then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \tparam DC : the type to check
     *
     * \see ::elastica::configuration::protocols::DomainConfiguration
     */
    template <typename DC>
    struct IsDomainConfiguration
        : ::tt::conforms_to<
              DC, ::elastica::configuration::protocols::DomainConfiguration> {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsDomainConfiguration type
     * trait.
     * \ingroup config_type_traits
     *
     * This variable template provides a convenient shortcut to access the
     * nested \a value of the IsDomainConfiguration class template. For
     * instance, given the type \a T the following two statements are identical:
     * \code
     * constexpr bool value1 = elastica::tt::IsDomainConfiguration<T>::value;
     * constexpr bool value2 = elastica::tt::is_domain_configuration_v<T>;
     * \endcode
     *
     * \see IsDomainConfiguration
     */
    template <typename DC>
    constexpr bool is_domain_configuration_v = IsDomainConfiguration<DC>::value;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Check whether a given type `DC` is a valid DomainConfiguration
     * \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `DC` strictly conforms to
     * ::elastica::configuration::protocols::DomainConfiguration, otherwise
     * inherits from std::false_type.
     *
     * \usage
     * For any type `DC`,
     * \code
     * using result = IsValidDomainConfiguration<DC>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `DC` strictly conforms to
     * ::elastica::configuration::protocols::DomainConfiguration, that is it
     * publicly inherits from the protocol and passes all the requirements
     * imposed by the protocol, then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \tparam DC : the type to check
     *
     * \see ::elastica::configuration::protocols::DomainConfiguration
     */
    template <typename DC>
    struct IsValidDomainConfiguration
        : public ::tt::detail::AssertConformsToImpl<
              DC, ::elastica::configuration::protocols::DomainConfiguration> {};
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica