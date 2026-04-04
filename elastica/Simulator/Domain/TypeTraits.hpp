//==============================================================================
/*!
//  \file
//  \brief Type traits for the Domain interface
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

#include "Simulator/Domain/Protocols.hpp"
#include "Simulator/Domain/Types.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Check whether a given type `D` is a Domain
     * \ingroup domain
     *
     * \details
     * Inherits from std::true_type if `D` conforms to
     * ::elastica::protocols::ComputationalDomain, otherwise inherits from
     * std::false_type.
     *
     * \usage
     * For any type `D`,
     * \code
     * using result = IsDomain<D>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `D` inherits from elastica::protocols::ComputationalDomain,
     * then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \example
     * \snippet Domain/Test_TypeTraits.cpp is_domain_example
     *
     * \tparam D : the type to check
     *
     * \see Domain
     */
    template <typename D>
    struct IsDomain
        : public ::tt::conforms_to<D,
                                   ::elastica::protocols::ComputationalDomain> {
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsDomain type trait.
     * \ingroup domain
     *
     * The is_domain_v variable template provides a convenient shortcut to
     * access the nested \a value of the IsDomain class template. For
     * instance, given the type \a T the following two statements are
     * identical:
     * \example
     * \code
     * using namespace elastica::tt;
     * constexpr bool value1 = IsDomain<T>::value;
     * constexpr bool value2 = is_domain_v<T>;
     * \endcode
     *
     * \see IsDomain
     */
    template <typename D>
    constexpr bool is_domain_v = IsDomain<D>::value;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Check whether a given type `D` is a valid Domain
     * \ingroup domain
     *
     * \details
     * Inherits from std::true_type if `D` strictly conforms to
     * ::elastica::protocols::ComputationalDomain, otherwise inherits from
     * std::false_type.
     *
     * \usage
     * For any type `D`,
     * \code
     * using result = IsValidDomain<D>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `D` strictly conforms to
     * elastica::protocols::ComputationalDomain, that is it
     * inherits from the protocol and passes all the requirements imposed by the
     * protocol, then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \example
     * \snippet Domain/Test_TypeTraits.cpp is_valid_domain_example
     *
     * \tparam D : the type to check
     *
     * \see Domain
     */
    template <typename D>
    struct IsValidDomain : public ::tt::detail::AssertConformsToImpl<
                               D, ::elastica::protocols::ComputationalDomain> {
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Obtains domain type for the whole simulation.
     * \ingroup domain
     *
     * \details
     * Obtains the whole domain type (periodic or unbounded) gicen the boundary
     * types along each direction.
     *
     * \usage
     * For any number of boundary types `BoundaryTypesAlong`,
     * \code
     * using result = SimulationDomainType<BoundaryTypesAlong...>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If at least one BoundaryTypeAlong is Periodic, then the simulation domain
     * is periodic else unbounded.
     * \code
     * result = PeriodicDomain;
     * \endcode
     * otherwise
     * \code
     * result = UnboundedDomain;
     * \endcode
     *
     * \note
     * Code can be simplified to just at the expense of readability
     * std::disjunction_v<BoundaryTypes...>
     *
     * \example
     * \snippet Domain/Test_TypeTraits.cpp SimulationDomainType_eg
     *
     * \tparam BoundaryTypesAlong Boundary types along different directions
     *
     * \see Domain, IsPeriodic, IsUnbounded
     */
    template <typename... BoundaryTypesAlong>
    using SimulationDomainType =
        std::conditional_t<std::disjunction_v<BoundaryTypesAlong...>,
                           PeriodicDomain, UnboundedDomain>;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
