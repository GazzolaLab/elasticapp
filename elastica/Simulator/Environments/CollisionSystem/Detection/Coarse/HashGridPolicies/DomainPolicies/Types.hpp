//==============================================================================
/*!
//  \file
//  \brief Types of all domain policies
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {
          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          template <typename SimulatorConfig, typename DomainType>
          class DomainPolicyImpl;

          // For static members only
          class DomainPolicyBase;
          /*! \endcond */
          //********************************************************************
        }

        //**********************************************************************
        /*!\brief The policy for the domain for hash grids
         *
         * \tparam SimulatorConfig Configuration for the simulation
         */
        template <typename SimulatorConfig>
        using DomainPolicy =
            detail::DomainPolicyImpl<SimulatorConfig,
                                     typename SimulatorConfig::DomainType>;
        //**********************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
