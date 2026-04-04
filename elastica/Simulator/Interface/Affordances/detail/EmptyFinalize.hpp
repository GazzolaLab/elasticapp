//==============================================================================
/*!
//  \file
//  \brief Empty finalize to use with affordance modules
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

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename Traits, typename>
      class EmptyFinalize {
       protected:
        //**********************************************************************
        /*!\brief The finalize method.
         *
         * \details
         * The finalize method is the interface to finalize the module
         * of elastica before launching into temporal integration.
         *
         * \param time_v The time to finalize, usually 0.0
         * \exception std::runtime_error If any Connection information is
         * missing/faulty
         */
        void finalize(typename Traits::TimeType /*time_v*/) noexcept {};
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
