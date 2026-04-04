//==============================================================================
/*!
//  \file
//  \brief All kinematic types
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
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
