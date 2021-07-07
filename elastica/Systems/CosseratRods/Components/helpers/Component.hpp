//==============================================================================
/*!
//  \file
//  \brief Helper to mark a valid component within @ref cosserat_rod_component
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
#include "Systems/CosseratRods/Components/helpers/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Base class for all component templates.
       * \ingroup cosserat_rod_component
       *
       * The Component class is the base class for all component templates
       * within \elastica. All classes that represent a component and that are
       * used within the @ref blocks environment of \elastica library have to
       * derive publicly from this class in order to qualify as a component.
       * Only in case a class is derived publicly from the Component base
       * class, the IsComponent type trait recognizes the class as valid
       * component.
       *
       * \tparam DerivedComponent A downstream component that derives from
       * the Component class using the CRTP pattern.
       */
      template <typename DerivedComponent>
      struct Component;
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialized of Component for Cosserat Rod components
       * \ingroup cosserat_rod_component
       */
      template <template <typename, typename, typename...>
                class DerivedComponent,
                typename Traits, typename InstantiatedBlock,
                typename... DerivedComponentMetaArgs>
      struct Component<DerivedComponent<Traits, InstantiatedBlock,
                                        DerivedComponentMetaArgs...>> {};
      /*! \endcond */
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
