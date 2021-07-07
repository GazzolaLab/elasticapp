//==============================================================================
/*!
//  \file
//  \brief Type traits for @ref cosserat_rod_component
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
#include "Utilities/TypeTraits/IsCRTP.hpp"

///
#include "Systems/CosseratRods/Components/helpers/Types.hpp"
///
#include "Systems/CosseratRods/Components/helpers/Component.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Check whether a given type `C` is a valid Component
       * \ingroup cosserat_rod_component
       *
       * \details
       * Inherits from std::true_type if `C` is a valid component template,
       * deriving from Component, otherwise inherits from std::false_type.
       *
       * \usage
       * For any type `C`,
       * \code
       * using result = IsComponent<C>;
       * \endcode
       *
       * \metareturns
       * cpp17::bool_constant
       *
       * \semantics
       * If the type `C` is derived from Component<C>, then
       * \code
       * typename result::type = std::true_type;
       * \endcode
       * otherwise
       * \code
       * typename result::type = std::false_type;
       * \endcode
       *
       * \example
       * \snippet helpers/Test_TypeTraits.cpp is_component_example
       *
       * \tparam C : the type to check
       *
       * \see Component
       */
      template <typename C>
      struct IsComponent : public ::tt::IsCRTP<Component, C> {};
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica