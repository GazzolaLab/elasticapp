//==============================================================================
/*!
//  \file
//  \brief All kinematic types
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

namespace elastica {

  namespace kinematics {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    namespace detail{
      template <typename... Kinematics_>
      struct GroupedTranslationKinematics;
    }

    // may need to refactor
    template <typename... Kinematics>
    using Translations = detail::GroupedTranslationKinematics<Kinematics...>;
    /*! \endcond */
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica
