//==============================================================================
/*!
//  \file
//  \brief Serialization support for a Blocking policy
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
///
#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
///
#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/IterationPolicy.hpp"
///
#include <exception>
#include <stdexcept>
#include <string>

#include "Simulator/Configuration/ModuleConfiguration/common/Serialize.hpp"
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a BlockingPolicyBase
     * \ingroup config
     *
     * \note Serialization only works for the registered types
     */
    template <typename Registrars>
    struct Serialize<
        ::elastica::configuration::detail::BlockingPolicyBase<Registrars>> {
      using type =
          ::elastica::configuration::detail::BlockingPolicyBase<Registrars>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        return call_with_dynamic_type<void, typename type::creatable_classes>(
            &policy,
            [&ar](auto* const blocking_policy) { ar(*blocking_policy); });
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        return call_with_dynamic_type<void, typename type::creatable_classes>(
            &policy,
            [&ar](auto const* const blocking_policy) { ar(*blocking_policy); });
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a BlockingPolicy
     * \ingroup config
     */
    template <typename Criterion, typename Registrars>
    struct Serialize<
        ::elastica::configuration::BlockingPolicy<Criterion, Registrars>> {
      using type =
          ::elastica::configuration::BlockingPolicy<Criterion, Registrars>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ar(policy.criterion_);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        ar(policy.criterion_);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a RestrictSizeOf policy
     * \ingroup config
     */
    template <typename Block>
    struct Serialize<::elastica::configuration::RestrictSize<Block>> {
      using type = ::elastica::configuration::RestrictSize<Block>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ::elastica::detail::load_impl(ar, policy);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        ::elastica::detail::save_impl(ar, policy);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a LooselyRestrictSizeOf
     * policy \ingroup config
     */
    template <typename Block>
    struct Serialize<::elastica::configuration::LooselyRestrictSize<Block>> {
      using type = ::elastica::configuration::LooselyRestrictSize<Block>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ::elastica::detail::load_impl(ar, policy);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        ::elastica::detail::save_impl(ar, policy);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a AlwaysSame policy
     * \ingroup config
     */
    template <typename Block>
    struct Serialize<::elastica::configuration::AlwaysSame<Block>> {
      using type = ::elastica::configuration::AlwaysSame<Block>;

      template <typename InArchive>
      static inline void load(InArchive&, type&) {}

      template <typename OutArchive>
      static inline void save(OutArchive&, type const&) {}
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a AlwaysNew policy
     * \ingroup config
     */
    template <typename Block>
    struct Serialize<::elastica::configuration::AlwaysNew<Block>> {
      using type = ::elastica::configuration::AlwaysNew<Block>;

      template <typename InArchive>
      static inline void load(InArchive&, type&) {}

      template <typename OutArchive>
      static inline void save(OutArchive&, type const&) {}
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a AcrossBlocks template
     * policy
     * \ingroup config
     */
    template <template <typename> class SizeBasedRestriction>
    struct Serialize<
        ::elastica::configuration::AcrossBlockTypes<SizeBasedRestriction>> {
      using type =
          ::elastica::configuration::AcrossBlockTypes<SizeBasedRestriction>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ::elastica::detail::load_impl(ar, policy);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        ::elastica::detail::save_impl(ar, policy);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a IterationPolicy
     * \ingroup config
     */
    template <typename ExecutionStrategyOverSystems,
              typename ExecutionStrategyForEachSystem, typename Criterion>
    struct Serialize<::elastica::configuration::IterationPolicy<
        ExecutionStrategyOverSystems, ExecutionStrategyForEachSystem,
        Criterion>> {
      using type = ::elastica::configuration::IterationPolicy<
          ExecutionStrategyOverSystems, ExecutionStrategyForEachSystem,
          Criterion>;

      template <typename InArchive>
      static inline void load(InArchive&, type&) {}

      template <typename OutArchive>
      static inline void save(OutArchive&, type const&) {}
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a IterationPolicy
     * \ingroup config
     */
    template <typename ExecutionStrategyOverSystems, typename Criterion>
    struct Serialize<::elastica::configuration::IterationPolicy<
        ExecutionStrategyOverSystems, ::elastica::configuration::hybrid_policy,
        Criterion>> {
      using type = ::elastica::configuration::IterationPolicy<
          ExecutionStrategyOverSystems,
          ::elastica::configuration::hybrid_policy, Criterion>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ar(static_cast<Criterion&>(policy));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        ar(static_cast<Criterion const&>(policy));
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for a SerialBelow
     * policy
     * \ingroup config
     */
    template <typename Block>
    struct Serialize<::elastica::configuration::SerialBelow<Block>> {
      using type = ::elastica::configuration::SerialBelow<Block>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ::elastica::detail::load_impl(ar, policy);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        ::elastica::detail::save_impl(ar, policy);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of AllSerialize template for a AllSerialBelow
     * policy
     * \ingroup config
     */
    template <>
    struct Serialize<::elastica::configuration::AllSerialBelow> {
      using type = ::elastica::configuration::AllSerialBelow;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& policy) {
        ::elastica::detail::load_impl(ar, policy);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& policy) {
        ::elastica::detail::save_impl(ar, policy);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica