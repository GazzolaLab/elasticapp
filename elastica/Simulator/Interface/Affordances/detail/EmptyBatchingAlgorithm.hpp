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
      template <typename, typename>
      struct EmptyBatchingAlgorithm {
       public:
        //**Batching algorithm**************************************************
        //! Algorithm for batching
        using BatchingAlgorithm = tmpl::list<>;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
