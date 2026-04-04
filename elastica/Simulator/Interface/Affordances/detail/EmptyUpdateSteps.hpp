//==============================================================================
/*!
//  \file
//  \brief Empty update steps to use with affordance modules
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
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename Traits, typename>
      struct EmptyUpdateSteps {
       public:
        //**Algorithmic order***************************************************
        /*!\name Algorithmic order of steps within a temporal loop*/
        //@{
        //! Algorithms to be carried out after the kinematic update step
        using PostKinematicUpdateSteps = tmpl::list<>;
        //! Algorithms to be carried out before the dynamic update step
        using PreDynamicUpdateSteps = tmpl::list<>;
        //! Algorithms to be carried out after the dynamic update step
        using PostDynamicUpdateSteps = tmpl::list<>;
        //! Algorithms to be carried out as an update step
        using UpdateSteps = tmpl::list<>;
        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica