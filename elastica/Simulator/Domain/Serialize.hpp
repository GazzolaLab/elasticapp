//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Domain type
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

/// base serialization
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
///
#include "Simulator/Domain/Types.hpp"
///
#include "Simulator/Domain/Domain.hpp"
#include "Simulator/Domain/Span.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a Span
     * \ingroup domain
     */
    template <typename Numeric>
    struct Serialize<::elastica::Span<Numeric>> {
      using type = ::elastica::Span<Numeric>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& span) {
        ar(span.span_);
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& span) {
        ar(span.span_);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a Unbounded domain policy
     * \ingroup domain
     */
    template <typename Real>
    struct Serialize<::elastica::UnboundedDomainPolicy<Real>> {
      using type = ::elastica::UnboundedDomainPolicy<Real>;

      template <typename InArchive>
      static inline void load(InArchive& /*ar*/, type& /*policy*/) {}

      template <typename InArchive>
      static inline void save(InArchive& /*ar*/, type const& /*policy*/) {}
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a Periodic domain policy
     * \ingroup domain
     */
    template <typename Real>
    struct Serialize<::elastica::PeriodicDomainPolicy<Real>> {
      using type = ::elastica::PeriodicDomainPolicy<Real>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ar(policy.get_span_buffer());
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& policy) {
        ar(policy.get_span_buffer());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a Periodic domain policy
     * \ingroup domain
     */
    template <typename Real, template <typename> class DomainPolicy>
    struct Serialize<::elastica::detail::Domain<Real, DomainPolicy>> {
      using type = ::elastica::detail::Domain<Real, DomainPolicy>;
      using Policy = typename type::Policy;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ar(static_cast<Policy&>(policy));
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& policy) {
        ar(static_cast<Policy const&>(policy));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica