//==============================================================================
/*!
//  \file
//  \brief Cosserat rod name
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

#include "Systems/common/Components/NameComponent.hpp"
//
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Provides a name to CosseratRod
     * \ingroup cosserat_rod_component
     */
    template <typename /* Traits */, typename /* Block */, typename NameImpl>
    struct NameAdapter : public NameImpl {
     protected:
      //**Type definitions******************************************************
      //! List of computed variables
      using ComputedVariables = tmpl::list<>;
      //! List of initialized variables
      using InitializedVariables = tmpl::list<>;
      //! List of all variables
      using Variables = tmpl::list<>;
      //************************************************************************

      //************************************************************************
      /*!\brief Initialize method for the current component
       *
       * \tparam DownstreamBlock The final block-like object which is
       * derived from the current component
       * \tparam CosseratInitializer An Initializer corresponding to the
       * Cosserat rod hierarchy
       */
      template <typename DownstreamBlock, typename CosseratInitializer>
      static void initialize(DownstreamBlock&, CosseratInitializer&&) noexcept {
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Human-readable name of the current plugin and all derivates
       *
       * \note This is intended to work with pretty_type::name<>
       */
      using NameImpl::name;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace detail

}  // namespace elastica
