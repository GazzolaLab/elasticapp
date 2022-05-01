//==============================================================================
/*!
//  \file
//  \brief Empty reset to use with affordance modules
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

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename Traits, typename>
      class EmptyResetModule {
       protected:
        //**Reset methods*******************************************************
        /*!\name Reset methods*/
        //@{

        //**********************************************************************
        /*!\brief Resets the current module
         *
         * \param time_v The time of reset
         */
        void reset_module(typename Traits::TimeType /*time_v*/) noexcept {};
        //**********************************************************************

        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
